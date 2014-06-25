#include <pebble.h>

#define ONES_SELECTED 1
#define TENS_SELECTED 10
#define HUNDRED_SELECTED 100

Window       *window;
GBitmap      *bmp_button_bar;
BitmapLayer  *layer_button_bar;

TextLayer *layer_counter; 
TextLayer *layer_selector_hundred; 
TextLayer *layer_selector_ten;
TextLayer *layer_selector_one;


struct CommonData {
  short counter;
  int   selected_digit;
  char  counter_text[10];
} s_data;

void init_selector_layer(Layer *layer_parent, TextLayer *text_layer, char* label) {
  text_layer_set_text_alignment  ( text_layer, GTextAlignmentCenter                        );
  text_layer_set_background_color( text_layer, GColorBlack                                 );
  text_layer_set_text_color      ( text_layer, GColorWhite                                 );
  text_layer_set_font            ( text_layer, fonts_get_system_font( FONT_KEY_GOTHIC_28 ) );
  text_layer_set_text            ( text_layer, label                                       );

  layer_add_child(layer_parent, text_layer_get_layer(text_layer));
}
void set_unselected( TextLayer *text_layer) {
    text_layer_set_background_color (text_layer, GColorBlack);
    text_layer_set_text_color       (text_layer, GColorWhite); 
}
void toggle_selected_layer() {
  TextLayer *selected;

  switch (s_data.selected_digit) {
    case ONES_SELECTED:
      selected   = layer_selector_one;
      break;
    case TENS_SELECTED:
      selected   = layer_selector_ten;
      break;
    case HUNDRED_SELECTED:
      selected   = layer_selector_hundred;
      break;
    default:
      return;
      break;
  }

  set_unselected( layer_selector_one     );
  set_unselected( layer_selector_ten     );
  set_unselected( layer_selector_hundred );

  if (selected) {
    text_layer_set_background_color (selected, GColorWhite);
    text_layer_set_text_color       (selected, GColorBlack);
  }
}

void set_counter_label_text() {
  snprintf           ( s_data.counter_text, 10, "%i", s_data.counter      );
  text_layer_set_text( layer_counter,                 s_data.counter_text );
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

  s_data.selected_digit = ONES_SELECTED;
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
  increment_counter( s_data.selected_digit ); set_counter_label_text();
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  decrement_counter( s_data.selected_digit ); set_counter_label_text();
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  move_pointer(); 
}
void select_long_click_handler_up(ClickRecognizerRef recognizer, Window *window) {
  toggle_selected_layer(); reset_counter(); set_counter_label_text(); toggle_selected_layer();
}

void click_config_provider(void *context) {
  window_single_click_subscribe          ( BUTTON_ID_SELECT, (ClickHandler) select_single_click_handler ) ;
  window_long_click_subscribe            ( BUTTON_ID_SELECT, 500, NULL, (ClickHandler) select_long_click_handler_up );
  window_single_repeating_click_subscribe( BUTTON_ID_UP,     100, (ClickHandler) up_single_click_handler      );
  window_single_repeating_click_subscribe( BUTTON_ID_DOWN,   100, (ClickHandler) down_single_click_handler    );
}

void window_load(Window *window) {
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer( window );
  bmp_button_bar      = gbitmap_create_with_resource ( RESOURCE_ID_IMAGE_BUTTON_BAR );
  
  layer_button_bar = bitmap_layer_create( (GRect) { .origin = { 0, 0 }, .size = {142, 152} } );
  bitmap_layer_set_bitmap( layer_button_bar, bmp_button_bar );
  layer_add_child        ( window_layer, bitmap_layer_get_layer( layer_button_bar ) );

  layer_selector_hundred = text_layer_create( GRect(11, 85, 40, 40) );
  layer_selector_ten     = text_layer_create( GRect(52, 85, 40, 40) );
  layer_selector_one     = text_layer_create( GRect(93, 85, 40, 40) );

  init_selector_layer( window_layer, layer_selector_hundred, "100" );
  init_selector_layer( window_layer, layer_selector_ten,     "10"  );
  init_selector_layer( window_layer, layer_selector_one,     "1"   );

  layer_counter = text_layer_create( GRect(0,20,144,50) );
  text_layer_set_font              (layer_counter, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment    (layer_counter, GTextAlignmentCenter);
  text_layer_set_background_color  (layer_counter, GColorBlack);
  text_layer_set_text_color        (layer_counter, GColorWhite);
  layer_add_child( window_layer, text_layer_get_layer( layer_counter ) );

  toggle_selected_layer();

  set_counter_label_text();

}
void window_unload( Window *window ) {
  text_layer_destroy  ( layer_counter          );
  text_layer_destroy  ( layer_selector_one     );
  text_layer_destroy  ( layer_selector_ten     );
  text_layer_destroy  ( layer_selector_hundred );
  gbitmap_destroy     ( bmp_button_bar         );
  bitmap_layer_destroy( layer_button_bar       );
}
void init() {
  s_data.selected_digit = ONES_SELECTED;

  window = window_create();

  window_set_click_config_provider( window, click_config_provider);
  window_set_window_handlers      ( window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
}
void deinit() {
  window_destroy(window);
}

int main(void) {
  init          ();
  app_event_loop();
  deinit        ();
}