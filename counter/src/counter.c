#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#define BUFFER_SIZE 50
#define ONES_SELECTED 1
#define TENS_SELECTED 10
#define HUNDRED_SELECTED 100

#define MY_UUID { 0xE9, 0xFA, 0x71, 0xA1, 0xD0, 0x27, 0x43, 0xB7, 0xAB, 0xE6, 0xFC, 0xE1, 0xB7, 0x1F, 0xCC, 0xC8 }
PBL_APP_INFO(MY_UUID,
             "Template App", "Your Company",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;

struct CommonWordsData {
  bool is_negative;
  short counter;
  int selected_digit;
} s_data;

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

void decrement_counter(short how_much) {
  s_data.counter -= how_much;
}
void increment_counter(short how_much) {
  s_data.counter += how_much;
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  //text_layer_set_text(&textLayer, "Select!");
}


void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;


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
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
