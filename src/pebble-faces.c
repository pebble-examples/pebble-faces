#include <pebble.h>
#include "netimage.h"

static Window *window;
static TextLayer *text_layer;
static BitmapLayer *bitmap_layer;
static NetImage *current_image;

static char *images[] = {
  "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-js/alex.png.pbi",
  "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-js/andrew.png.pbi",
  "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-js/asad.png.pbi",
  "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-js/brad.png.pbi",
  "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-js/chris.png.pbi",
  "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-js/eric.png.pbi",
  "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-js/joseph.png.pbi",
  "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-js/thomas.png.pbi"
};
static uint image = 0;

void show_next_image() {
  netimage_request(images[image]);

  image++;
  if (image >= sizeof(images)/sizeof(char*)) {
    image = 0;
  }

  // show that we are loading...
  bitmap_layer_set_bitmap(bitmap_layer, NULL);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Loading...");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  bitmap_layer = bitmap_layer_create(bounds);
  layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer));
  current_image = NULL;

  // Let's get started!
  show_next_image();
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
  bitmap_layer_destroy(bitmap_layer);
  netimage_destroy(current_image);
}

void image_loaded_handler(NetImage *image) {
  bitmap_layer_set_bitmap(bitmap_layer, image->bmp);
  // Free the memory used by the previous image
  netimage_destroy(current_image);
  // Keep a pointer to this image data so we can free it later.
  current_image = image;
}

void tap_handler(AccelAxisType accel, int32_t direction) {
  show_next_image();
}

static void init(void) {
  // Need to initialize this first to make sure it is there when
  // the window_load function is called by window_stack_push.
  netimage_initialize(image_loaded_handler);

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  accel_tap_service_subscribe(tap_handler);
}

static void deinit(void) {
  netimage_deinitialize(); // call this to avoid 20B memory leak
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
