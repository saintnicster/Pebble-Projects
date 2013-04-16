// This file was copied from the Pebble SDK Examples,
// specifically from the "fuzzy_time" watch.
// https://github.com/pebble/pebble-sdk-examples/blob/ac679812cc633599a319ab6c9062de3c0fbfb817/watches/fuzzy_time/src/num2words.c
// 
// I have modified it for my own uses

#include "num2words.h"
#include "string.h"
#include "pebble_os.h"

static const char* const ONES[] = {
  "",
  "one",
  "two",
  "three",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine"
};

static const char* const TEENS[] = {
  "ten",
  "eleven",
  "twelve",
  "thirteen",
  "fourteen",
  "fifteen",
  "sixteen",
  "seventeen",
  "eighteen",
  "nineteen"
};

static const char* const TENS[] = {
  "",
  "ten",
  "twenty",
  "thirty",
  "forty",
  "fifty",
  "sixty",
  "seventy",
  "eighty",
  "ninety"
};

static const char* STR_OH_CLOCK = "o'clock";
static const char* STR_NOON = "noon";
static const char* STR_MIDNIGHT = "midnight";
static const char* STR_QUARTER = "quarter";
static const char* STR_TO = "til";
static const char* STR_PAST = "past";
static const char* STR_HALF = "half";
static const char* STR_AFTER = "after";

static size_t append_number(char* words, int num) {
  int tens_val = num / 10 % 10;
  int ones_val = num % 10;

  size_t len = 0;

  if (tens_val > 0) {
    if (tens_val == 1 && num != 10) {
      strcat(words, TEENS[ones_val]);
      return strlen(TEENS[ones_val]);
    }
    strcat(words, TENS[tens_val]);
    len += strlen(TENS[tens_val]);
    if (ones_val > 0) {
      strcat(words, " ");
      len += 1;
    }
  }

  if (ones_val > 0 || num == 0) {
    strcat(words, ONES[ones_val]);
    len += strlen(ONES[ones_val]);
  }
  return len;
}

static size_t append_string(char* buffer, const size_t length, const char* str) {
  strncat(buffer, str, length);

  size_t written = strlen(str);
  return (length > written) ? written : length;
}

int time_to_semi_fuzzy_words(int hours, int minutes, char* hour_words, size_t hour_length, char* minute_words, size_t minute_length) {
  //Inspiration taken from the original 'fuzzy_time_to_words' method.
  //  There are surely more efficient ways to do this
  int fuzzy_hours = hours;
  int fuzzy_minutes = minutes;
  int is_fuzzy = 0;
  int modifier_length = minute_length;

  memset(hour_words, 0, hour_length);
  memset(minute_words, 0, minute_length);

  char modifier[modifier_length];
  size_t remaining_hour = hour_length;
  size_t remaining_minute = minute_length;

  if ( (fuzzy_minutes > 0 && fuzzy_minutes < 10) || (fuzzy_minutes >= 50) || (fuzzy_minutes % 15 == 0) ) {
    is_fuzzy = 1;

    if (fuzzy_minutes > 30) {
      modifier_length -= append_string(modifier, minute_length, STR_TO);
      if (fuzzy_minutes >= 50) {
        fuzzy_minutes = 60 - fuzzy_minutes;
      }

      fuzzy_hours += 1;
      if (fuzzy_hours > 23) {
        fuzzy_hours = 0;
      }
    } else {
        modifier_length -= append_string(modifier, minute_length, STR_PAST);
    }

    if (fuzzy_minutes == 15 || fuzzy_minutes == 45) {
      remaining_minute -= append_string(minute_words, remaining_minute, STR_QUARTER);
    } else if (fuzzy_minutes == 30) {
      remaining_minute -= append_string(minute_words, remaining_minute, STR_HALF);
    } else {
      remaining_minute -= append_number(minute_words, fuzzy_minutes);
    }

    if (is_fuzzy) {
      remaining_minute -= append_string(minute_words, remaining_minute, " ");
      remaining_minute -= append_string(minute_words, remaining_minute, modifier);
    }
  } else if ( fuzzy_minutes == 0 && (fuzzy_hours != 0 || fuzzy_hours != 12) ) {
      remaining_minute -= append_string(minute_words, remaining_minute, STR_OH_CLOCK);
  } else {
      remaining_minute -= append_number(minute_words, fuzzy_minutes);
  }
        //if ( !clock_is_24h_style() ) {
      //  fuzzy_hours = fuzzy_hours % 12;
      //}
      if (fuzzy_hours == 0) {
        remaining_hour -= append_string(hour_words, remaining_hour, STR_MIDNIGHT);
      } else if (fuzzy_hours == 12) {
        remaining_hour -= append_string(hour_words, remaining_hour, STR_NOON);
      } else {
        remaining_hour -= append_number(hour_words, fuzzy_hours);
      }
  return is_fuzzy;
}

      /*
  if (fuzzy_hours == 0 || fuzzy_hours == 12) {
    if ((fuzzy_minutes == 0 || is_fuzzy)) {
      if (fuzzy_hours == 0) {
        remaining_hour -= append_string(hour_words, remaining_hour, STR_MIDNIGHT);
      } else if (fuzzy_hours == 12) {
        remaining_hour -= append_string(hour_words, remaining_hour, STR_NOON);
      }
    }
  }
  if (!hour_words) {
  }*/

void fuzzy_time_to_words(int hours, int minutes, char* words, size_t length) {
  int fuzzy_hours = hours;
  int fuzzy_minutes = ((minutes + 2) / 5) * 5;

  // Handle hour & minute roll-over.
  if (fuzzy_minutes > 55) {
    fuzzy_minutes = 0;
    fuzzy_hours += 1;
    if (fuzzy_hours > 23) {
      fuzzy_hours = 0;
    }
  }

  size_t remaining = length;
  memset(words, 0, length);

  if (fuzzy_minutes != 0 && (fuzzy_minutes >= 10 || fuzzy_minutes == 5 || fuzzy_hours == 0 || fuzzy_hours == 12)) {
    if (fuzzy_minutes == 15) {
      remaining -= append_string(words, remaining, STR_QUARTER);
      remaining -= append_string(words, remaining, " ");
      remaining -= append_string(words, remaining, STR_AFTER);
      remaining -= append_string(words, remaining, " ");
    } else if (fuzzy_minutes == 45) {
      remaining -= append_string(words, remaining, STR_QUARTER);
      remaining -= append_string(words, remaining, " ");
      remaining -= append_string(words, remaining, STR_TO);
      remaining -= append_string(words, remaining, " ");

      fuzzy_hours = (fuzzy_hours + 1) % 24;
    } else if (fuzzy_minutes == 30) {
      remaining -= append_string(words, remaining, STR_HALF);
      remaining -= append_string(words, remaining, " ");
      remaining -= append_string(words, remaining, STR_PAST);
      remaining -= append_string(words, remaining, " ");
    } else if (fuzzy_minutes < 30) {
      remaining -= append_number(words, fuzzy_minutes);
      remaining -= append_string(words, remaining, " ");
      remaining -= append_string(words, remaining, STR_AFTER);
      remaining -= append_string(words, remaining, " ");
    } else {
      remaining -= append_number(words, 60 - fuzzy_minutes);
      remaining -= append_string(words, remaining, " ");
      remaining -= append_string(words, remaining, STR_TO);
      remaining -= append_string(words, remaining, " ");

      fuzzy_hours = (fuzzy_hours + 1) % 24;
    }
  }

  if (fuzzy_hours == 0) {
    remaining -= append_string(words, remaining, STR_MIDNIGHT);
  } else if (fuzzy_hours == 12) {
    remaining -= append_string(words, remaining, STR_NOON);
  } else {
    remaining -= append_number(words, fuzzy_hours % 12);
  }

  if (fuzzy_minutes == 5 && !(fuzzy_hours == 0 || fuzzy_hours == 12)) {
    remaining -= append_string(words, remaining, " ");
    remaining -= append_string(words, remaining, STR_OH_CLOCK);
  }
}