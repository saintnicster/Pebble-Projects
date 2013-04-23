#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "mini-printf.h"
#define ARROW_UP_ID 1
#define ARROW_DOWN_ID 2
#define MY_UUID { 0xC7, 0xE2, 0xF4, 0x51, 0xF5, 0x05, 0x49, 0x2C, 0x8F, 0x53, 0x33, 0x28, 0x32, 0xB6, 0x67, 0x78 }

PBL_APP_INFO(MY_UUID,
             "Scorekeeper", "Nick Fajardo",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_STANDARD_APP);
Window window; BmpContainer button_plus; BmpContainer button_minus; BmpContainer arrow;

struct CommonWordsData {
  int top_counter;
  int bottom_counter;
  int arrow_direction;
  TextLayer top_counter_label;
  TextLayer bottom_counter_label;
  TextLayer blank_layer;
  char top_counter_text[10];
  char bottom_counter_text[10];
} s_data;

void increment_counter() {
  int *counter;
  if (s_data.arrow_direction == ARROW_UP_ID) {
    counter = &s_data.top_counter;
  } else {
    counter = &s_data.bottom_counter;
  }

  if (*counter + 1 > 9999) {
    *counter = 9999;
  } else {
    *counter+= 1;
  }
}

void decrement_counter() {
  int *counter;
  if (s_data.arrow_direction == ARROW_UP_ID) {
    counter = &s_data.top_counter;
  } else {
    counter = &s_data.bottom_counter;
  }

  if (*counter - 1 < -9999) {
    *counter = -9999;
  } else {
    *counter -= 1;
  }
}

void toggle_active_counter() {
  layer_remove_from_parent( &(arrow.layer).layer);
  bmp_deinit_container(&arrow);

  if (s_data.arrow_direction == ARROW_DOWN_ID) {
    s_data.arrow_direction = ARROW_UP_ID;
    bmp_init_container(RESOURCE_ID_IMAGE_ARROW_UP, &arrow);
  } else {
    s_data.arrow_direction = ARROW_DOWN_ID;
    bmp_init_container(RESOURCE_ID_IMAGE_ARROW_DOWN, &arrow);
  }

  GRect frame = layer_get_frame( &(arrow.layer).layer);
  frame.origin.x = 53;
  frame.origin.y = 70;
  layer_set_frame( &(arrow.layer.layer), frame);

  layer_add_child(&window.layer, &(arrow.layer).layer);
}

void refresh_labels() {
  mini_snprintf(s_data.top_counter_text, 10, "%d", s_data.top_counter);
  text_layer_set_text(&s_data.top_counter_label, s_data.top_counter_text);

  mini_snprintf(s_data.bottom_counter_text, 10, "%d", s_data.bottom_counter);
  text_layer_set_text(&s_data.bottom_counter_label, s_data.bottom_counter_text);
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer; (void)window;
  increment_counter(); refresh_labels();
}
void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer; (void)window;
  decrement_counter(); refresh_labels();
}
void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer; (void)window;

  toggle_active_counter();
}
void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer; (void)window;
  s_data.top_counter=0; s_data.bottom_counter=0;
  refresh_labels();
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

void init_images() {
  bmp_init_container( RESOURCE_ID_IMAGE_BUTTON_PLUS, &button_plus);
  GRect frame = layer_get_frame( &(button_plus.layer).layer );
  frame.origin.x = 119;
  frame.origin.y = 0;
  layer_set_frame( &(button_plus.layer).layer, frame );
  layer_add_child(&window.layer, &(button_plus.layer).layer);

  bmp_init_container( RESOURCE_ID_IMAGE_BUTTON_MINUS, &button_minus);
  frame = layer_get_frame( &(button_minus.layer).layer);
  frame.origin.x = 118;
  frame.origin.y = 127;
  layer_set_frame( &(button_minus.layer).layer, frame);
  layer_add_child(&window.layer, &(button_minus.layer).layer);
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;
  
  bmp_deinit_container(&button_plus);
  bmp_deinit_container(&button_minus);
  bmp_deinit_container(&arrow);
}
void init_counter_label(Window window, TextLayer *layer, GRect frame) {
  text_layer_init( layer, frame);

  text_layer_set_text_alignment( layer, GTextAlignmentCenter );
  text_layer_set_background_color( layer, GColorBlack );
  text_layer_set_text_color( layer, GColorWhite );
  text_layer_set_font( layer, fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD) );

  layer_add_child(&window.layer, &(*layer).layer );
}

void handle_init(AppContextRef ctx) {
  (void)ctx;
  resource_init_current_app(&SCOREKEEPER_IMAGES);
  s_data.top_counter     = 0;
  s_data.bottom_counter  = 0;
  s_data.arrow_direction = ARROW_DOWN_ID;

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
  
  init_images();

  init_counter_label( window, &s_data.top_counter_label, GRect(0,15,125,46) );
  init_counter_label( window, &s_data.bottom_counter_label, GRect(0,94,125,50) );

  refresh_labels();

  text_layer_init( &s_data.blank_layer, GRect(0, 63, 144, 37) );
  text_layer_set_background_color( &s_data.blank_layer, GColorWhite );
  layer_add_child( &window.layer, &s_data.blank_layer.layer );

  toggle_active_counter();
}
void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler   = &handle_init,
    .deinit_handler = &handle_deinit,
  };
  app_event_loop(params, &handlers);
}
