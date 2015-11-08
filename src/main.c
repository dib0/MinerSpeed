#include <pebble.h>

Window *my_window;
TextLayer *text_layer;
static bool s_dataInited;
static bool s_wasFirstMsg;

enum {
  QUOTE_KEY_INIT = 0x0,
  QUOTE_KEY_FETCH = 0x1,
  QUOTE_KEY_SYMBOL = 0x2,
  QUOTE_KEY_PRICE = 0x03,
};

static void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *init_tuple = dict_find(iter, QUOTE_KEY_INIT);
  Tuple *symbol_tuple = dict_find(iter, QUOTE_KEY_SYMBOL);
  Tuple *price_tuple = dict_find(iter, QUOTE_KEY_PRICE);

  if (init_tuple) {
    // only accept one initial tuple; the second may be a server reply to
    // an out-of-date action on our part
    if (s_dataInited) {
      return;
    } else {
      s_dataInited = true;
    }
  }
  if (symbol_tuple) {
    //strncpy(s_symbol, symbol_tuple->value->cstring, 5);
    //text_layer_set_text(s_symbol_layer, s_symbol);
  }
  if (price_tuple) {
    //strncpy(s_price, price_tuple->value->cstring, 10);
    //text_layer_set_text(s_price_layer, s_price);
  }
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped!");
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  if (s_wasFirstMsg && s_dataInited) {
    // Ignore, was successful
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Failed to Send!");
  }

  s_wasFirstMsg = false;
}

void handle_init(void) {
  // Register message handlers
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_failed(out_failed_handler);
  
  // Init buffers
  app_message_open(64, 64);
  
  my_window = window_create();
  
  // Window setup
  window_set_background_color(my_window, GColorFromRGB(85, 85, 85));

  text_layer = text_layer_create(GRect(20, 50, 100, 20));
  //text_layer_set_text_color(text_layer, GColorFromRGB(85, 85, 85));
  text_layer_set_text_color(text_layer, GColorFromRGB(0, 0, 210));
  
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
