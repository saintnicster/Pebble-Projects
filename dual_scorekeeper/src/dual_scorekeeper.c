#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xC7, 0xE2, 0xF4, 0x51, 0xF5, 0x05, 0x49, 0x2C, 0x8F, 0x53, 0x33, 0x28, 0x32, 0xB6, 0x67, 0x78 }
PBL_APP_INFO(MY_UUID,
             "Dual Scorekeeper", "Nick Fajardo",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;
BmpContainer button_plus;
BmpContainer button_minus;
BmpContainer arrow;

void init_images() {
  bmp_init_container(RESOURCE_ID_IMAGE_BUTTON_PLUS, &button_plus);
  bmp_init_container(RESOURCE_ID_IMAGE_BUTTON_MINUS, &button_minus);
  bmp_init_container(RESOURCE_ID_IMAGE_ARROW_UP, &arrow);

  GRect frame = layer_get_frame( &(button_plus.layer).layer );
  frame.origin.x = 119;
  frame.origin.y = 0;
  layer_set_frame( &(button_plus.layer).layer, frame );

  frame = layer_get_frame( &(button_minus.layer).layer);
  frame.origin.x = 118;
  frame.origin.y = 127;
  layer_set_frame( &(button_minus.layer).layer, frame);

  frame = layer_get_frame( &(arrow.layer).layer);
  frame.origin.x = 62;
  frame.origin.y = 62;
  layer_set_frame( &(arrow.layer.layer), frame);

  layer_add_child(&window.layer, &(button_plus.layer).layer);
  layer_add_child(&window.layer, &(button_minus.layer).layer);
  layer_add_child(&window.layer, &(arrow.layer).layer);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;
  resource_init_current_app(&SCOREKEEPER_IMAGES);

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);

  init_images();
}
void handle_deinit(AppContextRef ctx) {
  (void)ctx;
  
  bmp_deinit_container(&button_plus);
  bmp_deinit_container(&button_minus);
  bmp_deinit_container(&arrow);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler   = &handle_init,
    .deinit_handler = &handle_deinit,
  };
  app_event_loop(params, &handlers);
}
