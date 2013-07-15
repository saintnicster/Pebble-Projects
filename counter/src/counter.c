#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "mini-printf.h"

#define ONES_SELECTED 1
#define TENS_SELECTED 10
#define HUNDRED_SELECTED 100

#define MY_UUID { 0xE9, 0xFA, 0x71, 0xA1, 0xD0, 0x27, 0x43, 0xB7, 0xAB, 0xE6, 0xFC, 0xE1, 0xB7, 0x1F, 0xCC, 0xC8 }
PBL_APP_INFO(MY_UUID,
             "Counter", "Nick Fajardo",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;
BmpContainer button_bar;

struct CommonWordsData {
  TextLayer counter_label;
  TextLayer selector_hundred;
  TextLayer selector_ten;
  TextLayer selector_one;

  short counter;
  int selected_digit;
  char counter_text[10];
} s_data;

void init_selector_layer(Window window, TextLayer *text_layer, GRect frame, char* label) {
  text_layer_init(text_layer, frame);

  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text(text_layer, label);

  layer_add_child(&window.layer, &(*text_layer).layer);
}

void toggle_selected_layer() {
  TextLayer *selected;
  TextLayer *unselected;
  switch (s_data.selected_digit) {
    case ONES_SELECTED:
      selected   = &s_data.selector_one;
      unselected = &s_data.selector_hundred;
      break;
    case TENS_SELECTED:
      selected   = &s_data.selector_ten;
      unselected = &s_data.selector_one;
      break;
    case HUNDRED_SELECTED:
      selected   = &s_data.selector_hundred;
      unselected = &s_data.selector_ten;
      break;
    default:
      return;
      break;
  }
  if (selected) {
    text_layer_set_background_color(selected, GColorWhite);
    text_layer_set_text_color(selected, GColorBlack);
  }
  if (unselected) {
    text_layer_set_background_color(unselected, GColorBlack);
    text_layer_set_text_color(unselected, GColorWhite); 
  }
}

void set_counter_label_text() {
  mini_snprintf(s_data.counter_text, 10, "%u", s_data.counter);
  text_layer_set_text(&s_data.counter_label, s_data.counter_text);
}

void decrement_counter(short how_much) {
  if (s_data.counter - how_much < -9999) {
    s_data.counter = -9999;
  } else {
    s_data.counter -= how_much;
  }
  
}
void increment_counter(short how_much) {
  if (s_data.counter + how_much > 9999) {
    s_data.counter = 9999;
  } else {
    s_data.counter += how_much;
  }
}

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

  toggle_selected_layer();
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

  s_data.selected_digit = ONES_SELECTED;

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
  window_set_background_color(&window, GColorBlack);
  resource_init_current_app(&COUNTER_IMAGES);
  bmp_init_container(RESOURCE_ID_IMAGE_BUTTON_BAR, &button_bar);
  layer_add_child(&window.layer, &button_bar.layer.layer);

  text_layer_init(&s_data.counter_label, GRect(0,20,144,50));
  text_layer_set_font(&s_data.counter_label, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(&s_data.counter_label, GTextAlignmentCenter);
  text_layer_set_background_color(&s_data.counter_label, GColorBlack);
  text_layer_set_text_color(&s_data.counter_label, GColorWhite);
  layer_add_child(&window.layer, &s_data.counter_label.layer);

  init_selector_layer(window, &s_data.selector_hundred, GRect(11, 85, 40, 40), "100");
  init_selector_layer(window, &s_data.selector_ten, GRect(52, 85, 40, 40), "10");
  init_selector_layer(window, &s_data.selector_one, GRect(93, 85, 40, 40), "1");


  toggle_selected_layer();

  set_counter_label_text();
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  // Note: Failure to de-init this here will result in instability and
  //       unable to allocate memory errors.
  bmp_deinit_container(&button_bar);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit
  };
  app_event_loop(params, &handlers);
}
