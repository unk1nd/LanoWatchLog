#include <pebble.h>
  
#define KEY_ACTION 0
#define KEY_NICK 1
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_logg_layer;
static TextLayer *s_action_layer;
static TextLayer *s_output_layer;
static TextLayer *s_bt_layer;

static GFont s_time_font;
static GFont s_logg_font;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static const uint32_t const segments[] = { 200, 100, 400 };
  VibePattern pat = {
  .durations = segments,
  .num_segments = ARRAY_LENGTH(segments),
};

static void Shake() {
  // makes it shake shake like a motherfucker
  APP_LOG(APP_LOG_LEVEL_ERROR, "Shake Shake!");
  vibes_enqueue_custom_pattern(pat);
}

static void bt_handler(bool connected) {
  // Show current connection state
  if (connected) {
    text_layer_set_text(s_bt_layer, "O");
  } else {
    text_layer_set_text(s_bt_layer, "X");
  }
}

static void battery_handler(BatteryChargeState new_state) {
  // Write to buffer and display
  static char s_battery_buffer[32];
  
  BatteryChargeState charge_state = battery_state_service_peek();
  if (charge_state.is_charging) {
  snprintf(s_battery_buffer, sizeof(s_battery_buffer), "charging");
} else {
  snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", new_state.charge_percent);
}
  text_layer_set_text(s_output_layer, s_battery_buffer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 24h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  
  // Update batteri status
  battery_handler(battery_state_service_peek());
}

static void main_window_load(Window *window) {
  //Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(5, 41, 139, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  
  //Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));

  //Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  // Create logg Layer
  s_logg_layer = text_layer_create(GRect(0, 120, 144, 45));
  text_layer_set_background_color(s_logg_layer, GColorClear);
  text_layer_set_text_color(s_logg_layer, GColorWhite);
  text_layer_set_text_alignment(s_logg_layer, GTextAlignmentCenter);
  text_layer_set_text(s_logg_layer, "Loading...");
  
  // Create action layer
  s_action_layer = text_layer_create(GRect(0, 100, 144, 45));
  text_layer_set_background_color(s_action_layer, GColorClear);
  text_layer_set_text_color(s_action_layer, GColorWhite);
  text_layer_set_text_alignment(s_action_layer, GTextAlignmentCenter);
  text_layer_set_text(s_action_layer, "Last Action");
    
  // Create second custom font, apply it and add to Window
  s_logg_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_12));
  text_layer_set_font(s_logg_layer, s_logg_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_logg_layer));
  
  // do the same to action layer
  text_layer_set_font(s_action_layer, s_logg_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_action_layer));
   
  // Create output for batteri
  s_output_layer = text_layer_create(GRect(0, 0, 144, 168));
  text_layer_set_font(s_output_layer, s_logg_font);
  text_layer_set_background_color(s_output_layer, GColorClear);
  text_layer_set_text_color(s_output_layer, GColorWhite);
  text_layer_set_text_alignment(s_output_layer, GTextAlignmentRight );
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_output_layer));
  
  // Create output for BT connection
  s_bt_layer = text_layer_create(GRect(0, 0, 144, 168));
  text_layer_set_font(s_bt_layer, s_logg_font);
  text_layer_set_background_color(s_bt_layer, GColorClear);
  text_layer_set_text_color(s_bt_layer, GColorWhite);
  text_layer_set_text_alignment(s_bt_layer, GTextAlignmentLeft);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bt_layer));

  // Show current connection state
  bt_handler(bluetooth_connection_service_peek());
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  //Unload GFont
  fonts_unload_custom_font(s_time_font);
  
  //Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  //Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
  // Destroy elements
  text_layer_destroy(s_logg_layer);
  fonts_unload_custom_font(s_logg_font);
  text_layer_destroy(s_action_layer);
  
  // Destroy batteri status text
  text_layer_destroy(s_output_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get logg update every 2 minutes
  if(tick_time->tm_min % 2 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char action_buffer[32];
  static char nick_buffer[32];
  static char logg_layer_buffer[32];
  static char old_layer_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_ACTION:
      snprintf(action_buffer, sizeof(action_buffer), "%s", t->value->cstring);
      break;
    case KEY_NICK:
      snprintf(nick_buffer, sizeof(nick_buffer), "%s", t->value->cstring);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized! DOH!!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // store old data for check
  strcpy(old_layer_buffer, logg_layer_buffer);
  
  // Assemble full string and display
  snprintf(logg_layer_buffer, sizeof(logg_layer_buffer), "%s\n%s", action_buffer, nick_buffer);
  text_layer_set_text(s_logg_layer, logg_layer_buffer);
  
  if(strcmp(old_layer_buffer, logg_layer_buffer) == 0)
  {
    APP_LOG(APP_LOG_LEVEL_ERROR, "No new data!");
  }
  else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "New Data!");
    Shake();
    light_enable_interaction();
  }
  
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  // Subscribe to the Battery State Service
  battery_state_service_subscribe(battery_handler);
  
  // Subscribe to Bluetooth updates
  bluetooth_connection_service_subscribe(bt_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}




