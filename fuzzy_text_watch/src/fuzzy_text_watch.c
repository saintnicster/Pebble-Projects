#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "num2words.h"

#define DEBUG 1
#define MY_UUID { 0x26, 0xDE, 0x49, 0xF2, 0x28, 0x00, 0x44, 0xB2, 0x89, 0x3D, 0xBE, 0x8A, 0x61, 0xA5, 0xB3, 0x59 }
PBL_APP_INFO(MY_UUID,
             "FuzzyTextWatch", "Nick Fajardo",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
#if DEBUG
             APP_INFO_STANDARD_APP
#else
       APP_INFO_WATCH_FACE
#endif
       );

PblTm t;
#define BUFFER_SIZE 86
static struct CommonWordsData {
  TextLayer label1;
  TextLayer label2;
  Window window;
  char buffer[BUFFER_SIZE];
} s_data;

static void bold_layer(TextLayer* bold_layer ) {
  text_layer_set_background_color(bold_layer, GColorBlack);
  text_layer_set_text_color(bold_layer, GColorWhite);
  text_layer_set_font(bold_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
}
static void light_layer(TextLayer* light_layer) {
  text_layer_set_background_color(light_layer, GColorBlack);
  text_layer_set_text_color(light_layer, GColorWhite);
  text_layer_set_font(light_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24)); 
}

static void update_time(PblTm* t) {
  char* hour_words = "hour"; char* minute_words = "minute"; int is_fuzzy = 0;
  time_to_semi_fuzzy_words(t->tm_hour, t->tm_min, hour_words, BUFFER_SIZE, minute_words, BUFFER_SIZE, &is_fuzzy);

  if (is_fuzzy) {
    light_layer(&s_data.label1);
    text_layer_set_text(&s_data.label1,  minute_words);
    bold_layer(&s_data.label2);
    text_layer_set_text(&s_data.label2, hour_words);
  } else {
    light_layer(&s_data.label2);
    text_layer_set_text(&s_data.label2, minute_words);
    bold_layer(&s_data.label1);
    text_layer_set_text(&s_data.label1, hour_words);
  }
  
}

static void handle_minute_tick(AppContextRef app_ctx, PebbleTickEvent* e) {
  update_time(e->tick_time);
}

/** 
 * Debug methods. For quickly debugging enable debug macro on top to transform the watchface into
 * a standard app and you will be able to change the time with the up and down buttons
 */ 
#if DEBUG

PblTm t;
void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  t.tm_min += 1;
  if (t.tm_min >= 60) {
    t.tm_min = 0;
    t.tm_hour += 1;

    if (t.tm_hour >= 24) {
      t.tm_hour = 0;
    }
  }
  update_time(&t);
}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  t.tm_min -= 1;
  if (t.tm_min < 0) {
    t.tm_min = 59;
    t.tm_hour -= 1;
  }
  update_time(&t);
}

void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}

#endif

void handle_init(AppContextRef ctx) {
  (void)ctx;
  
  window_init(&s_data.window, "FuzzyTextWatch");

  window_set_background_color(&s_data.window, GColorBlack);
  window_stack_push(&s_data.window, true /* Animated */);

  text_layer_init(&s_data.label1, GRect(0, 20, s_data.window.layer.frame.size.w, 45));
  text_layer_init(&s_data.label2, GRect(0, 70, s_data.window.layer.frame.size.w, 45));
  layer_add_child(&s_data.window.layer, &s_data.label1.layer);
  layer_add_child(&s_data.window.layer, &s_data.label2.layer);

  get_time(&t);
  update_time(&t);

  #if DEBUG
  // Button functionality
  window_set_click_config_provider(&s_data.window, (ClickConfigProvider) click_config_provider);
  #endif
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
