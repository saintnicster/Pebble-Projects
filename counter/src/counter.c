#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "mini-printf.h"

#define ONES_SELECTED 1
#define TENS_SELECTED 10
#define HUNDRED_SELECTED 100

#define MY_UUID { 0xE9, 0xFA, 0x71, 0xA1, 0xD0, 0x27, 0x43, 0xB7, 0xAB, 0xE6, 0xFC, 0xE1, 0xB7, 0x1F, 0xCC, 0xC8 }
PBL_APP_INFO(MY_UUID,
             "Counter App", "Nick Fajardo",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;

struct CommonWordsData {
  TextLayer counter_label;
  short counter;
  int selected_digit;
} s_data;
void reset_counter() {
  s_data.counter = 0;
}
void move_pointer() {
  switch (s_data.selected_digit) {
    case ONES_SELECTED:
      s_data.selected_digit = TENS_SELECTED;
      break;
    case TENS_SELECTED:
      s_data.selected_digit = HUNDRED_SELECTED;
      break;
    case HUNDRED_SELECTED:
      s_data.selected_digit = ONES_SELECTED;
      break;
  }
}

void set_counter_label_text() {
  char counter_text[5];
  mini_snprintf(counter_text, 5, "%u", s_data.counter);
  text_layer_set_text(&s_data.counter_label, counter_text);
}

void decrement_counter(short how_much) {
  s_data.counter -= how_much;
}
void increment_counter(short how_much) {
  s_data.counter += how_much;
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  increment_counter(s_data.selected_digit);
  set_counter_label_text();
}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  decrement_counter(s_data.selected_digit);
  set_counter_label_text();
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  move_pointer();
  set_counter_label_text();
}


void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  reset_counter();
  set_counter_label_text();
}

void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;

  config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}

void handle_init(AppContextRef ctx) {
  (void)ctx;
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);

  text_layer_init(&s_data.counter_label, GRect(0,0,40,144));
  text_layer_set_font(&s_data.counter_label, fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD));
  layer_add_child(&window.layer, &s_data.counter_label.layer);

  set_counter_label_text();
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
