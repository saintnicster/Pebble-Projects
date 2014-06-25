#include <pebble.h>
#include "num2words.h"
#include <stdbool.h>

Window    *window;
TextLayer *layer_label_1;
TextLayer *layer_label_2;

struct tm* t;
#define BUFFER_SIZE 44
static struct CommonWordsData {
  char hour_words[BUFFER_SIZE];
  char minute_words[BUFFER_SIZE];
} s_data;

static void bold_layer(TextLayer* bold_layer ) {
  text_layer_set_background_color( bold_layer, GColorBlack );
  text_layer_set_text_color      ( bold_layer, GColorWhite );
  text_layer_set_font            ( bold_layer, fonts_get_system_font( FONT_KEY_DROID_SERIF_28_BOLD ) );
}
static void light_layer(TextLayer* light_layer) {
  text_layer_set_background_color( light_layer, GColorBlack );
  text_layer_set_text_color      ( light_layer, GColorWhite );
  text_layer_set_font            ( light_layer, fonts_get_system_font( FONT_KEY_GOTHIC_28 ) ); 
}

static void update_time(struct tm* t) {
  bool is_fuzzy;
  is_fuzzy = time_to_semi_fuzzy_words(t->tm_hour, t->tm_min, s_data.hour_words, BUFFER_SIZE, s_data.minute_words, BUFFER_SIZE, clock_is_24h_style() );

  if (is_fuzzy) {
    light_layer        ( layer_label_1                      );
    text_layer_set_text( layer_label_1, s_data.minute_words );
    bold_layer         ( layer_label_2                      );
    text_layer_set_text( layer_label_2, s_data.hour_words   );
  } else {
    bold_layer         ( layer_label_1                      );
    text_layer_set_text( layer_label_1, s_data.hour_words   );
    light_layer        ( layer_label_2                      );
    text_layer_set_text( layer_label_2, s_data.minute_words );
  }
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time( tick_time );
}

void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer( window );

  window_set_background_color( window, GColorBlack );

  layer_label_1 = text_layer_create( GRect(0, 18, 144, 50) );
  text_layer_set_text( layer_label_1, "Layer 1");
  text_layer_set_background_color( layer_label_1, GColorBlack );
  
  layer_label_2 = text_layer_create( GRect(0, 68, 144, 50) );
  text_layer_set_text( layer_label_2, "Layer 2");
  text_layer_set_background_color( layer_label_2, GColorBlack );
  
  layer_add_child( window_layer, text_layer_get_layer( layer_label_1 ) );
  layer_add_child( window_layer, text_layer_get_layer( layer_label_2 ) );

  time_t now   = time(NULL);
  struct tm *t = localtime(&now);
  
  update_time( t );
}

void window_unload( Window *window ) {
  text_layer_destroy( layer_label_1 );
  text_layer_destroy( layer_label_2 );
}

void init() {
  window = window_create( );
  window_set_window_handlers( window, (WindowHandlers) {
    .load   = window_load,
    .unload = window_unload,
  });
  window_stack_push( window, true );

  tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) &handle_minute_tick);
}
void deinit() {
  window_destroy( window );
}


int main(void) {
  init();

  app_event_loop();

  deinit();
}
