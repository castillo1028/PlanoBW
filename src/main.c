#include <pebble.h>
  
static Window *s_main_window; 


static TextLayer *s_time_layer;
static TextLayer *s_sec_layer;
static TextLayer *s_date_layer;
static TextLayer *s_up_layer;
static TextLayer *s_day_layer;
static GFont s_time_font;
static Layer *s_battery_layer;
static int s_battery_level;

static void battery_callback(BatteryChargeState state) {
  
s_battery_level = state.charge_percent;
  
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
GRect bounds = layer_get_bounds(layer);
  // Find the width of the bar
  int width = (int)(float)(((float)s_battery_level / 100.0F) * 114.0F);
    
   // Draw the background
   graphics_context_set_fill_color(ctx, GColorWhite);
   graphics_fill_rect(ctx, bounds, 0, GCornerNone);
                            
                            
   // Draw the Bar
   graphics_context_set_fill_color(ctx, GColorWhite);
   graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), 0, GCornerNone);
                            
  layer_mark_dirty(s_battery_layer);                      
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  
// Create a long-lived buffer
  static char buffer[] = "00 00";
  static char sec_buffer[] = "00";
  static char date_buffer[] = "000 00";
  static char up_buffer[] = "00";
  static char day_buffer[] = "000000000";
  
  
  
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
// Use 24 hour format
  strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else 
  strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  strftime(sec_buffer, sizeof("00"), "%S", tick_time);
  strftime(date_buffer, sizeof("000 00"), "%b %e", tick_time);
  strftime(up_buffer, sizeof("00"), "%p", tick_time); 
  strftime(day_buffer, sizeof("000000000"), "%A", tick_time); 
   
  
// Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (buffer[0] == '0')) 
  memmove(buffer, &buffer[1], sizeof(buffer) - 1); 
     
  
// Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_sec_layer, sec_buffer);
  text_layer_set_text(s_day_layer, day_buffer);
  text_layer_set_text(s_date_layer, date_buffer);
  text_layer_set_text(s_up_layer, up_buffer);
  
}

static void main_window_load(Window *window) {
  

// Create time TextLayer
//Create GFont 
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PALAMECIA_50));
  s_time_layer = text_layer_create(GRect(0, 20, 144, 168));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  
  s_sec_layer = text_layer_create(GRect(72, 0, 72, 20));
  text_layer_set_background_color(s_sec_layer, GColorClear);
  text_layer_set_text_color(s_sec_layer, GColorWhite);
  
  
  s_day_layer = text_layer_create(GRect(0, 100, 144, 20));
  text_layer_set_background_color(s_day_layer, GColorClear);
  text_layer_set_text_color(s_day_layer, GColorWhite);
  
  
  s_up_layer = text_layer_create(GRect(0, 0, 72, 20));
  text_layer_set_background_color(s_up_layer, GColorClear);
  text_layer_set_text_color(s_up_layer, GColorWhite);
  
  
  s_date_layer = text_layer_create(GRect(0, 120, 144, 35));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  
  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  text_layer_set_font(s_sec_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_sec_layer, GTextAlignmentCenter);
  
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  
  text_layer_set_font(s_up_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_up_layer, GTextAlignmentCenter);
  
  text_layer_set_font(s_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
  
  s_battery_layer = layer_create(GRect(6, 160, 132, 2));
  layer_set_update_proc(s_battery_layer, battery_update_proc);

  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_sec_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_up_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_layer));
  layer_add_child(window_get_root_layer(window), s_battery_layer);
  
}

static void main_window_unload(Window *window) {
  
    fonts_unload_custom_font(s_time_font);
  
  // Destroy TextLayer
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_sec_layer);
    text_layer_destroy(s_date_layer);
    text_layer_destroy(s_up_layer);
    text_layer_destroy(s_day_layer);
    layer_destroy(s_battery_layer);
  
  
}


static void init() {
// Create main Window element and assign to pointer
  s_main_window = window_create();

                             
  // Set handlers to manage the elements inside the Window//
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
   
  window_set_background_color(s_main_window, GColorBlack);
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register for battery service
  battery_state_service_subscribe(battery_callback);
  
  // Battery Layer from the start
  battery_callback(battery_state_service_peek());
  
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);  
 
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
  
