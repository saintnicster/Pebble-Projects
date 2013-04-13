#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x26, 0xDE, 0x49, 0xF2, 0x28, 0x00, 0x44, 0xB2, 0x89, 0x3D, 0xBE, 0x8A, 0x61, 0xA5, 0xB3, 0x59 }
PBL_APP_INFO(MY_UUID,
             "FuzzyTextWatch", "Nick Fajardo",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
