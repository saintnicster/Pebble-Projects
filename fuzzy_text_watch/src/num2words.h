#pragma once

#include "string.h"
#include <stdbool.h>

void fuzzy_time_to_words(int hours, int minutes, char* words, size_t length);

bool time_to_semi_fuzzy_words(int hours, int minutes, char* hour_words, size_t hour_length, char* minute_words, size_t minute_length, bool is_24h_style);
