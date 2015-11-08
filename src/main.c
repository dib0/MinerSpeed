#include <pebble.h>

Window *my_window;
TextLayer *text_layer;

void handle_init(void) {
  my_window = window_create();

  text_layer = text_layer_create(GRect(20, 50, 100, 20));
  text_layer_set_text(text_layer, "Miner stats:");
  
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(text_layer));
  
  window_stack_push(my_window, true);
}

void handle_deinit(void) {
  text_layer_destroy(text_layer);
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
