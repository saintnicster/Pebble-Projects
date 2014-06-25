#include <pebble.h>

Window      *window;
TextLayer   *layer_text;
Layer       *layer_battery;
BitmapLayer *layer_usb_white;
BitmapLayer *layer_usb_black;
GBitmap     *bmp_usb_white;
GBitmap     *bmp_usb_black;

uint8_t last_percentage;
int is_charging;

void draw_usb_icon() {
  if (!bmp_usb_white) {
    bmp_usb_white = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_USB_WHITE );
    bmp_usb_black = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_USB_BLACK );
  }
  if (!layer_usb_white) {
    layer_usb_white = bitmap_layer_create( GRect(102, 74, 24, 50) );
    layer_usb_black = bitmap_layer_create( GRect(102, 74, 24, 50) );
    bitmap_layer_set_bitmap          ( layer_usb_white,  bmp_usb_white );
    bitmap_layer_set_compositing_mode( layer_usb_white,  GCompOpOr );
    bitmap_layer_set_bitmap          ( layer_usb_black,  bmp_usb_black    );
    bitmap_layer_set_compositing_mode( layer_usb_black,  GCompOpClear );
  }

  Layer *window_layer = window_get_root_layer( window );
  layer_add_child                            ( window_layer, bitmap_layer_get_layer(layer_usb_white) );
  layer_add_child                            ( window_layer, bitmap_layer_get_layer(layer_usb_black) );
}
void clear_usb() {
  if (layer_usb_white) {
    layer_remove_from_parent( bitmap_layer_get_layer( layer_usb_white ) );
    layer_remove_from_parent( bitmap_layer_get_layer( layer_usb_black ) );
  }
}

void draw_battery_layer(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds( layer );
  GRect filled_bounds = bounds;

  filled_bounds.size.h   = ((double) filled_bounds.size.h * (last_percentage / 100.0));
  filled_bounds.origin.y =           bounds.size.h        - filled_bounds.size.h;
  
  graphics_context_set_stroke_color( ctx, GColorBlack );
  graphics_draw_rect               ( ctx, bounds );
  graphics_fill_rect               ( ctx, filled_bounds, 0, GCornerNone );
}
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);


  layer_text = text_layer_create( GRect(10, 10, bounds.size.w - 20, 50) );
  text_layer_set_text_alignment ( layer_text,   GTextAlignmentCenter );
  text_layer_set_font           ( layer_text,   fonts_get_system_font( FONT_KEY_GOTHIC_28 ) );
  text_layer_set_text           ( layer_text,   "Level - 00%" );
  text_layer_set_text_color     ( layer_text,   GColorBlack);
  layer_add_child               ( window_layer, text_layer_get_layer(layer_text) );

  layer_battery = layer_create(GRect(57, 60, 30, 80 ));
  layer_set_update_proc       (layer_battery, (LayerUpdateProc) draw_battery_layer);
  layer_add_child( window_layer, layer_battery );
}

static void window_unload(Window *window) {

  if (bmp_usb_white)   gbitmap_destroy     ( bmp_usb_white );
  if (bmp_usb_black)   gbitmap_destroy     ( bmp_usb_black );
  if (layer_usb_white) bitmap_layer_destroy( layer_usb_white );
  if (layer_usb_black) bitmap_layer_destroy( layer_usb_black );

  text_layer_destroy  ( layer_text );
  layer_destroy       ( layer_battery );
}
void handle_battery(BatteryChargeState charge_state) {
  char time_text[] = "[YYYY-MM-DD 00:00:00]";
  static char level_text[] = "Level - 100%";

  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  strftime(time_text, sizeof(time_text), "%Y-%m-%d %T", current_time);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "%s - Battery state - percent = %d%%, charging = %i, plugged = %i", time_text, charge_state.charge_percent, charge_state.is_charging, charge_state.is_plugged);

  is_charging = charge_state.is_charging;
  if   (charge_state.is_plugged) draw_usb_icon();
  else clear_usb();

  if (last_percentage != charge_state.charge_percent) {
    last_percentage = charge_state.charge_percent;
    layer_mark_dirty( layer_battery );
  }
  
  snprintf( level_text, sizeof(level_text), "Level - %d%%", last_percentage);
  text_layer_set_text( layer_text,  level_text );
}
static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  battery_state_service_subscribe( (BatteryStateHandler)handle_battery );

  handle_battery(battery_state_service_peek());
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  app_event_loop();
  
  deinit();
}
