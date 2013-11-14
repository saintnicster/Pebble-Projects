#include <pebble.h>

Window *window;
TextLayer *text_layer;
Layer *battery_layer;


uint8_t last_percentage;

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

  text_layer = text_layer_create( GRect(10, 10, bounds.size.w - 20, 50) );
  text_layer_set_text_alignment ( text_layer,   GTextAlignmentCenter );
  text_layer_set_font           ( text_layer,   fonts_get_system_font( FONT_KEY_GOTHIC_28 ) );
  text_layer_set_text           ( text_layer,   "Level - 00%" );
  text_layer_set_text_color     ( text_layer,   GColorBlack);
  layer_add_child               ( window_layer, text_layer_get_layer(text_layer) );

  battery_layer = layer_create(GRect(57, 60, 30, 80 ));
  layer_set_update_proc       (battery_layer, (LayerUpdateProc) draw_battery_layer);
  layer_add_child( window_layer, battery_layer );
}

static void window_unload(Window *window) {
  text_layer_destroy( text_layer );
  layer_destroy     ( battery_layer );
}
void handle_battery(BatteryChargeState charge_state) {
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  char time_text[] = "[YYYY-MM-DD 00:00:00]";
  strftime(time_text, sizeof(time_text), "%Y-%m-%d %T", current_time);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%s - Handling battery state... charge_percent = %d%%, is_charging = %i, is_plugged = %i", time_text, charge_state.charge_percent, charge_state.is_charging, charge_state.is_plugged);


  static char level_text[] = "Level - 100%";

  if (last_percentage != charge_state.charge_percent) {
    last_percentage = charge_state.charge_percent;
    layer_mark_dirty( battery_layer );
  }
  
  snprintf( level_text, sizeof(level_text), "Level - %d%%", last_percentage);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", level_text);
  text_layer_set_text( text_layer,  level_text );
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
