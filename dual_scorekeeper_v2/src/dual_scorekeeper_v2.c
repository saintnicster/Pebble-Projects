#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xFF, 0x08, 0x29, 0x37, 0xC1, 0xA2, 0x4A, 0xF7, 0x8F, 0xED, 0xF8, 0x5C, 0xCE, 0x2D, 0x36, 0x51 }
PBL_APP_INFO(MY_UUID,
             "Scorekeeper v2", "Nick Fajardo",
             2, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window; BmpContainer button_top; BmpContainer button_bottom; 

struct ScoreData {
  int button_top_state;
  int button_bottom_state;
  int count_top;
  int count_bottom;
  TextLayer counter_top_label;
  TextLayer counter_bottom_label;
  char counter_top_text[10];
  char counter_bottom_text[10];

} s_data;
void increment_count( int *count ) {
  if (*count + 1 > 9999) {
    *count = 9999;
  } else {
    *count += 1;
  }
}
void decrement_count( int *count ) {
  if (*count - 1 < -9999) {
    *count = -9999;
  } else {
    *count  -= 1;
  }
}

void refresh_label(TextLayer *layer, int *count, char *text) {
  snprintf(text, 10, "%d", *count);
  text_layer_set_text(layer, text);
}

void arrow_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  ButtonId button = click_recognizer_get_button_id( recognizer );
  APP_LOG( APP_LOG_LEVEL_DEBUG, "arrow_single_click_handler, button: %i");
  int *state_buffer, *count_buffer;
  char *text_buffer;
  TextLayer *layer_buffer;
  switch (button) {
    case BUTTON_ID_UP:
      state_buffer = &s_data.button_top_state;
      count_buffer = &s_data.count_top;
      layer_buffer = &s_data.counter_top_label;
      text_buffer  = s_data.counter_top_text;
      break;
    case BUTTON_ID_DOWN:
      state_buffer = &s_data.button_bottom_state;
      count_buffer = &s_data.count_bottom;
      layer_buffer = &s_data.counter_bottom_label;
      text_buffer  = s_data.counter_bottom_text;
      break;
    default:
      return;
      break;
  }
  if (*state_buffer == BUTTON_ID_UP) {
    increment_count( count_buffer );
  } else {
    decrement_count( count_buffer );
  }
  refresh_label( layer_buffer, count_buffer, text_buffer);
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer; (void)window;
  APP_LOG( APP_LOG_LEVEL_DEBUG, "select_single_click_handler");
}
void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer; (void)window;
  APP_LOG( APP_LOG_LEVEL_DEBUG, "select_long_click_handler");
  s_data.count_top=0; s_data.count_bottom=0;
  refresh_label( &s_data.counter_top_label, &s_data.count_top, s_data.counter_top_text);
  refresh_label( &s_data.counter_bottom_label, &s_data.count_bottom, s_data.counter_bottom_text);
}

void set_button_images( ) {
  APP_LOG( APP_LOG_LEVEL_DEBUG, "set_button_images");
  int resource_to_load = (s_data.button_top_state == BUTTON_ID_UP) ? RESOURCE_ID_IMAGE_BUTTON_PLUS : RESOURCE_ID_IMAGE_BUTTON_MINUS;

  layer_remove_from_parent( &(button_top.layer).layer);
  bmp_deinit_container( &button_top );
  bmp_init_container( resource_to_load, &button_top );
  GRect frame = layer_get_frame( &(button_top.layer).layer );
  frame.origin.x = 119;
  frame.origin.y = 0;
  layer_set_frame( &(button_top.layer).layer, frame );
  layer_add_child(&window.layer, &(button_top.layer).layer);

  resource_to_load = (s_data.button_bottom_state == BUTTON_ID_UP) ? RESOURCE_ID_IMAGE_BUTTON_PLUS : RESOURCE_ID_IMAGE_BUTTON_MINUS;
  layer_remove_from_parent( &(button_bottom.layer).layer);
  bmp_deinit_container( &button_bottom );
  bmp_init_container( resource_to_load, &button_bottom );
  frame = layer_get_frame( &(button_bottom.layer).layer);
  frame.origin.x = 118;
  frame.origin.y = 127;
  layer_set_frame( &(button_bottom.layer).layer, frame);
  layer_add_child(&window.layer, &(button_bottom.layer).layer);
}

void multi_click_toggle_handler(ClickRecognizerRef recognizer, Window *window) {
  ButtonId button = click_recognizer_get_button_id( recognizer );
  APP_LOG( APP_LOG_LEVEL_DEBUG, "multi_click_toggle_handler, button: %i");
  int *state_buffer;
  switch (button) {
    case BUTTON_ID_UP:
      state_buffer = &s_data.button_top_state;
      break;
    case BUTTON_ID_DOWN:
      state_buffer = &s_data.button_bottom_state;
      break;
    default:
      return;
      break;
  }
  
  *state_buffer = (*state_buffer == BUTTON_ID_UP) ? BUTTON_ID_DOWN : BUTTON_ID_UP;

  set_button_images();
}

void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
  config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) arrow_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;
  config[BUTTON_ID_UP]->multi_click.handler = (ClickHandler) multi_click_toggle_handler;
  config[BUTTON_ID_UP]->multi_click.min = 2;
  config[BUTTON_ID_UP]->multi_click.max = 2;
  config[BUTTON_ID_UP]->multi_click.last_click_only = true;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) arrow_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
  config[BUTTON_ID_DOWN]->multi_click.handler = (ClickHandler) multi_click_toggle_handler;
  config[BUTTON_ID_DOWN]->multi_click.min = 2;
  config[BUTTON_ID_DOWN]->multi_click.max = 2;
  config[BUTTON_ID_DOWN]->multi_click.last_click_only = true;

}
void init_counter_label(Window window, TextLayer *layer, GRect frame) {
  text_layer_init( layer, frame);

  text_layer_set_text_alignment( layer, GTextAlignmentCenter );
  text_layer_set_background_color( layer, GColorBlack );
  text_layer_set_text_color( layer, GColorWhite );
  text_layer_set_font( layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT) );

  layer_add_child(&window.layer, &(*layer).layer );
}

void handle_init(AppContextRef ctx) {
  (void)ctx;
  resource_init_current_app(&SCOREKEEPER_IMAGES);

  s_data.button_top_state = BUTTON_ID_UP;
  s_data.button_bottom_state = BUTTON_ID_UP;

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);

  set_button_images();

  init_counter_label( window, &s_data.counter_top_label, GRect(0,15,125,46) );
  init_counter_label( window, &s_data.counter_bottom_label, GRect(0,94,125,50) );

  s_data.count_top=0; s_data.count_bottom=0;
  refresh_label( &s_data.counter_top_label, &s_data.count_top, s_data.counter_top_text);
  refresh_label( &s_data.counter_bottom_label, &s_data.count_bottom, s_data.counter_bottom_text);
}
void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  bmp_deinit_container(&button_top);
  bmp_deinit_container(&button_bottom);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,
  };
  app_event_loop(params, &handlers);
}
