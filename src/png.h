#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <pebble.h>

GBitmap* gbitmap_create_with_png_resource(uint32_t resource_id);
GBitmap* gbitmap_create_with_png_data(uint8_t *data, int data_bytes);

