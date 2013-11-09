#include <pebble.h>

#define PERSIST_KEY_TOP_COUNTER     -1
#define PERSIST_KEY_ARROW            0
#define PERSIST_KEY_BOTTOM_COUNTER   1
#define ARROW_UP_ID    -1
#define ARROW_DOWN_ID   1
	
Window *main_window;

Layer       *arrow_bar_layer;

BitmapLayer *button_top_white; 	  BitmapLayer *button_top_black;
BitmapLayer *button_bottom_white; BitmapLayer *button_bottom_black;
BitmapLayer *button_arrow;

GBitmap   *bmp_plus_white;  GBitmap   *bmp_plus_black;
GBitmap   *bmp_minus_white; GBitmap   *bmp_minus_black;
GBitmap   *bmp_arrow_up;  	GBitmap   *bmp_arrow_down;

TextLayer *counter_lbl_top; TextLayer *counter_lbl_bottom;

struct CommonData {
	int top_counter;
	int bottom_counter;
	int arrow_direction;
	char top_counter_text[10];
	char bottom_counter_text[10];
} s_data;

void increment_counter() {
	int *counter;
	if (s_data.arrow_direction == ARROW_UP_ID)	counter = &s_data.top_counter;
	else										counter = &s_data.bottom_counter;
	
	if (*counter + 1 > 9999)	*counter = 9999;
	else						*counter+= 1;
}
void decrement_counter() {
	int *counter;
	if (s_data.arrow_direction == ARROW_UP_ID)	counter = &s_data.top_counter;
	else										counter = &s_data.bottom_counter;
	
	if (*counter - 1 < -9999) *counter  = -9999;
	else                      *counter -= 1;
}
void populate_counter_labels( ) {
	snprintf( s_data.top_counter_text,    10, "%d", s_data.top_counter    );
	snprintf( s_data.bottom_counter_text, 10, "%d", s_data.bottom_counter );
	
	text_layer_set_text( counter_lbl_top,    s_data.top_counter_text    );
	text_layer_set_text( counter_lbl_bottom, s_data.bottom_counter_text );	
}

static void init_counter_lbl( Layer *window_layer, TextLayer *txt_layer ) {
	text_layer_set_background_color( txt_layer, GColorBlack          );
    text_layer_set_text_color      ( txt_layer, GColorWhite          );
	text_layer_set_text_alignment  ( txt_layer, GTextAlignmentCenter );
	text_layer_set_font            ( txt_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD) );
	
	layer_add_child                ( window_layer, text_layer_get_layer(txt_layer) );
}
static void set_button_graphic( Layer *window_layer, BitmapLayer *layer_white, BitmapLayer *layer_black,
							                         GBitmap     *bmp_white,   GBitmap 	   *bmp_black   ) {
	bitmap_layer_set_bitmap          ( layer_white,  bmp_white );
	bitmap_layer_set_compositing_mode( layer_white,  GCompOpOr );
	bitmap_layer_set_bitmap          ( layer_black,  bmp_black    );
	bitmap_layer_set_compositing_mode( layer_black,  GCompOpClear );
	
	layer_add_child                  ( window_layer, bitmap_layer_get_layer(layer_white) );
	layer_add_child                  ( window_layer, bitmap_layer_get_layer(layer_black) );
}
void toggle_counter( ) {
	s_data.arrow_direction = ( s_data.arrow_direction == ARROW_DOWN_ID ) ? ARROW_UP_ID : ARROW_DOWN_ID;
}
void set_button_arrow( ) {
	layer_remove_from_parent( bitmap_layer_get_layer(button_arrow) );
	
	GBitmap *bmp = ( s_data.arrow_direction == ARROW_DOWN_ID ) ? bmp_arrow_down : bmp_arrow_up;
	
	bitmap_layer_set_bitmap(button_arrow, bmp);
	layer_add_child( window_get_root_layer(main_window), bitmap_layer_get_layer(button_arrow) );
}
void update_arrow_bar_layer_bg(struct Layer *layer, GContext *ctx) {
	graphics_context_set_fill_color  ( ctx, GColorWhite);
	graphics_context_set_stroke_color( ctx, GColorWhite);
	graphics_draw_rect               ( ctx, layer_get_bounds(layer)                 );
	graphics_fill_rect               ( ctx, layer_get_bounds(layer), 0, GCornerNone );
}

void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);

	arrow_bar_layer = layer_create(GRect(0, 64, 144, 36));
	layer_set_update_proc( arrow_bar_layer, (LayerUpdateProc) update_arrow_bar_layer_bg );
	layer_add_child( window_layer, arrow_bar_layer);
	
	bmp_plus_white  = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_BUTTON_PLUS_WHITE  );
	bmp_plus_black  = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_BUTTON_PLUS_BLACK  );
	bmp_minus_white = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_BUTTON_MINUS_WHITE );
	bmp_minus_black = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_BUTTON_MINUS_BLACK );
	
	bmp_arrow_up    = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_ARROW_UP   );
	bmp_arrow_down  = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_ARROW_DOWN );

	button_top_white    = bitmap_layer_create( GRect(119, 0,   26, 26) );
	button_top_black    = bitmap_layer_create( GRect(119, 0,   26, 26) );
	button_bottom_white = bitmap_layer_create( GRect(119, 127, 26, 26) );
	button_bottom_black = bitmap_layer_create( GRect(119, 127, 26, 26) );
	
	button_arrow = bitmap_layer_create( GRect(53, 70, 20, 21) );
	set_button_arrow();
	
	set_button_graphic( window_layer, button_top_white,    button_top_black,    bmp_plus_white,  bmp_plus_black  );
	set_button_graphic( window_layer, button_bottom_white, button_bottom_black, bmp_minus_white, bmp_minus_black );
	
	counter_lbl_top    = text_layer_create( GRect(0, 10,  125, 46) );
	counter_lbl_bottom = text_layer_create( GRect(0, 100, 125, 46) );
	init_counter_lbl( window_layer, counter_lbl_top    );
	init_counter_lbl( window_layer, counter_lbl_bottom );
	
	populate_counter_labels(  );	
	
	window_set_background_color(window, GColorBlack);
}
void window_unload( ) {	
	gbitmap_destroy( bmp_plus_white  ); gbitmap_destroy( bmp_plus_black  );
	gbitmap_destroy( bmp_minus_white ); gbitmap_destroy( bmp_minus_black );
	gbitmap_destroy( bmp_arrow_up    ); gbitmap_destroy( bmp_arrow_down  );
	
	bitmap_layer_destroy( button_top_white    ); bitmap_layer_destroy( button_top_black    );
	bitmap_layer_destroy( button_bottom_white ); bitmap_layer_destroy( button_bottom_black );
	bitmap_layer_destroy( button_arrow        );
	
	text_layer_destroy(counter_lbl_top); text_layer_destroy(counter_lbl_bottom);
}
void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Up button");
	increment_counter(); populate_counter_labels();
}
void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Down button");
	decrement_counter(); populate_counter_labels();
}
void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Select button (single click)");
	toggle_counter( ); set_button_arrow( );
}
void select_long_click_down_handler(ClickRecognizerRef recognizer, Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Select button (long click, down)");
}
void select_long_click_up_handler(ClickRecognizerRef recognizer, Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Select button (long click, up)");
}
void select_double_click_handler(ClickRecognizerRef recognizer, Window *window) {
    const uint16_t count = click_number_of_clicks_counted(recognizer);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Select button (double click) count:%u", count);
	s_data.top_counter = 0; s_data.bottom_counter = 0; populate_counter_labels();
}
static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe( BUTTON_ID_UP,     100, (ClickHandler) up_single_click_handler         );
  window_single_repeating_click_subscribe( BUTTON_ID_DOWN,   100, (ClickHandler) down_single_click_handler       );
  window_single_click_subscribe          ( BUTTON_ID_SELECT, (ClickHandler) select_single_click_handler          );
  window_multi_click_subscribe           ( BUTTON_ID_SELECT, 2, 2, 200, true,                                    (ClickHandler) select_double_click_handler  );
  window_long_click_subscribe            ( BUTTON_ID_SELECT, 500, (ClickHandler) select_long_click_down_handler, (ClickHandler) select_long_click_up_handler );
}

void handle_init( ) {
	s_data.top_counter     = persist_read_int( PERSIST_KEY_TOP_COUNTER    );
	s_data.bottom_counter  = persist_read_int( PERSIST_KEY_BOTTOM_COUNTER );
	
	s_data.arrow_direction = persist_exists( PERSIST_KEY_ARROW ) ? persist_read_int( PERSIST_KEY_ARROW ) : ARROW_UP_ID ;
	
	main_window = window_create();
	window_set_window_handlers(main_window, (WindowHandlers) {
		.load   = window_load,
		.unload = window_unload
	});
	window_set_click_config_provider( main_window, click_config_provider );
	window_stack_push               ( main_window, true                  );
}

void handle_deinit() {
	persist_write_int(PERSIST_KEY_TOP_COUNTER, 		s_data.top_counter    );
	persist_write_int(PERSIST_KEY_BOTTOM_COUNTER, 	s_data.bottom_counter );
	persist_write_int(PERSIST_KEY_ARROW,            s_data.arrow_direction);
	
	window_destroy(main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}