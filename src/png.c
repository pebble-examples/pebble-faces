#ifdef PBL_PLATFORM_APLITE
#include "upng.h"
#include "png.h"

#define MAX(A,B) ((A>B) ? A : B)
#define MIN(A,B) ((A<B) ? A : B)

inline static char reverse_byte(uint8_t input) {
  uint8_t result;
  __asm__ ("rev  %[input], %[result]\n\t"
           "rbit %[result], %[result]"
           : [result] "=r" (result)
           : [input] "r" (input));
  return result;
}

static bool gbitmap_from_bitmap(
    GBitmap* gbitmap, const uint8_t* bitmap_buffer, int width, int height) {

  // Limit PNG to screen size
  width = MIN(width, 144);
  height = MIN(height, 168);

  // Copy width and height to GBitmap
  gbitmap->bounds.size.w = width;
  gbitmap->bounds.size.h = height;
  // GBitmap needs to be word aligned per line (bytes)
  gbitmap->row_size_bytes = ((width + 31) / 32 ) * 4;
  //Allocate new gbitmap array
  gbitmap->addr = malloc(height * gbitmap->row_size_bytes); 
  if (gbitmap->addr == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "malloc gbitmap->addr failed");
    return false;
  }
  gbitmap->is_heap_allocated = true;

  for(int y = 0; y < height; y++) {
    memcpy(
      &(((uint8_t*)gbitmap->addr)[y * gbitmap->row_size_bytes]), 
      &(bitmap_buffer[y * ((width + 7) / 8)]), 
      (width + 7) / 8);
  }

  // GBitmap pixels are most-significant bit, so need to flip each byte.
  for(int i = 0; i < gbitmap->row_size_bytes * height; i++){
    ((uint8_t*)gbitmap->addr)[i] = reverse_byte(((uint8_t*)gbitmap->addr)[i]);
  }

  return true;
}

GBitmap* gbitmap_create_with_png_resource(uint32_t resource_id) {
  upng_t* upng = NULL;

  ResHandle rHdl = resource_get_handle(resource_id);
  int png_raw_size = resource_size(rHdl);

  //Allocate gbitmap
  GBitmap* gbitmap_ptr = malloc(sizeof(GBitmap));

  uint8_t* png_raw_buffer = malloc(png_raw_size); //freed by upng impl
  if (png_raw_buffer == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "malloc png resource buffer failed");
  }
  resource_load(rHdl, png_raw_buffer, png_raw_size);
  upng = upng_new_from_bytes(png_raw_buffer, png_raw_size);
  if (upng == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "UPNG malloc error"); 
  }
  if (upng_get_error(upng) != UPNG_EOK) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "UPNG Loaded:%d line:%d", 
      upng_get_error(upng), upng_get_error_line(upng));
  }
  if (upng_decode(upng) != UPNG_EOK) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "UPNG Decode:%d line:%d", 
      upng_get_error(upng), upng_get_error_line(upng));
  }

  gbitmap_from_bitmap(gbitmap_ptr, upng_get_buffer(upng),
    upng_get_width(upng), upng_get_height(upng));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "converted to gbitmap");

  // Free the png, no longer needed
  upng_free(upng);
  upng = NULL;

  return gbitmap_ptr;
}

// This function will free the source memory to save on memory usage
GBitmap* gbitmap_create_with_png_data(uint8_t *data, int data_bytes) {
  upng_t* upng = NULL;

  //Allocate gbitmap
  GBitmap* gbitmap_ptr = malloc(sizeof(GBitmap));
  if (gbitmap_ptr == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Unable to malloc a new GBitmap!");
    return false;
  }

  upng = upng_new_from_bytes(data, data_bytes);
  if (upng == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "UPNG malloc error"); 
  }
  if (upng_get_error(upng) != UPNG_EOK) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "UPNG Loaded: Error=%d line:%d", 
      upng_get_error(upng), upng_get_error_line(upng));
  }
  if (upng_decode(upng) != UPNG_EOK) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "UPNG Decode: Error=%d line:%d", 
      upng_get_error(upng), upng_get_error_line(upng));
  }

  gbitmap_from_bitmap(gbitmap_ptr, upng_get_buffer(upng),
    upng_get_width(upng), upng_get_height(upng));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "converted to gbitmap");

  // Free the png, no longer needed
  upng_free(upng);
  upng = NULL;

  return gbitmap_ptr;
}

#endif // ifdef PBL_PLATFORM_APLITE