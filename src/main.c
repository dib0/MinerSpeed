#include <pebble.h>

Window *my_window;
TextLayer *text_layer;
TextLayer *speed_layer;
TextLayer *speedlabel_layer;
TextLayer *balance_layer;
TextLayer *balancelabel_layer;
TextLayer *time_layer;
static char s_time_buffer[16];
static int timer=0;

enum {
  MINER_KEY_SPEED = 0x0,
  MINER_KEY_BALANCE = 0x1,
};

static void send_to_phone() {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if (iter == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "null iter");
    return;
  }

  dict_write_end(iter);
  app_message_outbox_send();
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *speed_tuple = dict_find(iter, MINER_KEY_SPEED);
  Tuple *balance_tuple = dict_find(iter, MINER_KEY_BALANCE);
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG, speed_tuple->value->cstring);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, balance_tuple->value->cstring);
  text_layer_set_text(speed_layer, speed_tuple->value->cstring);
  text_layer_set_text(balance_layer, balance_tuple->value->cstring);
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped!");
}

static void load_data()
{
  text_layer_set_text(speed_layer, "Loading...");
  text_layer_set_text(balance_layer, "Loading...");
  send_to_phone();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // refresh
  load_data();
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  // refresh
  load_data();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, select_long_click_handler, NULL);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (clock_is_24h_style()) {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M", tick_time);
  } else {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%I:%M", tick_time);
  }

  text_layer_set_text(time_layer, s_time_buffer);
  
  // Refresh every 15 minutes
  timer++;
  if (timer > 14)
  {
    timer = 0;
    
    // refresh
    load_data();
  }
}

void handle_init(void) {
  // Register message handlers
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  
  // Init buffers
  app_message_open(64, 64);
  
  my_window = window_create();
  
  // Window setup
  window_set_background_color(my_window, GColorFromRGB(35, 35, 35));

  text_layer = text_layer_create(GRect(15, 50, 145, 20));
  text_layer_set_text_color(text_layer, GColorFromRGB(255, 255, 255));
  text_layer_set_text(text_layer, "Miner stats");
  
  speedlabel_layer = text_layer_create(GRect(15, 80, 70, 20));
  text_layer_set_text_color(speedlabel_layer, GColorFromRGB(125, 125, 255));
  text_layer_set_text(speedlabel_layer, "Speed:");

  speed_layer = text_layer_create(GRect(70, 80, 90, 20));
  text_layer_set_text_color(speed_layer, GColorFromRGB(0, 210, 0));
  text_layer_set_text(speed_layer, "Loading...");

  balancelabel_layer = text_layer_create(GRect(15, 100, 70, 20));
  text_layer_set_text_color(balancelabel_layer, GColorFromRGB(125, 125, 255));
  text_layer_set_text(balancelabel_layer, "Balance:");

  balance_layer = text_layer_create(GRect(70, 100, 90, 20));
  text_layer_set_text_color(balance_layer, GColorFromRGB(0, 210, 0));
  text_layer_set_text(balance_layer, "Loading...");

  time_layer = text_layer_create(GRect(0, 10, 150, 20));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_text_color(time_layer, GColorFromRGB(150, 150, 150));
  
  // Set background (comment for testing visibility)
  text_layer_set_background_color(time_layer, GColorFromRGB(35, 35, 35));
  text_layer_set_background_color(text_layer, GColorFromRGB(35, 35, 35));
  text_layer_set_background_color(speedlabel_layer, GColorFromRGB(35, 35, 35));
  text_layer_set_background_color(speed_layer, GColorFromRGB(35, 35, 35));
  text_layer_set_background_color(balancelabel_layer, GColorFromRGB(35, 35, 35));
  text_layer_set_background_color(balance_layer, GColorFromRGB(35, 35, 35));
  
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(time_layer));
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(text_layer));
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(speedlabel_layer));
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(speed_layer));
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(balancelabel_layer));
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(balance_layer));
  
  window_stack_push(my_window, true);
  
  // Set events
  window_set_click_config_provider(my_window, click_config_provider);  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  send_to_phone();
}

void handle_deinit(void) {
  tick_timer_service_unsubscribe();
  text_layer_destroy(time_layer);
  text_layer_destroy(text_layer);
  text_layer_destroy(speedlabel_layer);
  text_layer_destroy(speed_layer);
  text_layer_destroy(balancelabel_layer);
  text_layer_destroy(balance_layer);
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
