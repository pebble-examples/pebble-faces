#include "netimage.h"

NetImageContext* netimage_create_context(NetImageCallback callback) {
  NetImageContext *ctx = malloc(sizeof(NetImageContext));

  ctx->length = 0;
  ctx->index = 0;
  ctx->data = NULL;
  ctx->callback = callback;

  return ctx;
}

void netimage_destroy_context(NetImageContext *ctx) {
  if (ctx->data) {
    free(ctx->data);
  }
  free(ctx);
}

void netimage_initialize(NetImageCallback callback) {
  NetImageContext *ctx = netimage_create_context(callback);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "NetImageContext = %p", ctx);
  app_message_set_context(ctx);

  app_message_register_inbox_received(netimage_receive);
  app_message_register_inbox_dropped(netimage_dropped);
  app_message_register_outbox_sent(netimage_out_success);
  app_message_register_outbox_failed(netimage_out_failed);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Max buffer sizes are %li / %li", app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void netimage_deinitialize() {
  netimage_destroy_context(app_message_get_context());
  app_message_set_context(NULL);
}

void netimage_request(char *url) {
  DictionaryIterator *outbox;
  app_message_outbox_begin(&outbox);
  // Tell the javascript how big we want each chunk of data - 8 is the dictionary overhead
  uint32_t chunk_size = app_message_inbox_size_maximum() - 8;
  dict_write_int(outbox, NETIMAGE_CHUNK_SIZE, &chunk_size, sizeof(uint32_t), false);
  // Send the URL
  dict_write_cstring(outbox, NETIMAGE_URL, url);

  app_message_outbox_send();
}

void netimage_receive(DictionaryIterator *iter, void *context) {
  NetImageContext *ctx = (NetImageContext*) context;

  Tuple *tuple = dict_read_first(iter);
  if (!tuple) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Got a message with no first key! Size of message: %li", (uint32_t)iter->end - (uint32_t)iter->dictionary);
    return;
  }
  switch (tuple->key) {
    case NETIMAGE_DATA:
      if (ctx->index + tuple->length <= ctx->length) {
        memcpy(ctx->data + ctx->index, tuple->value->data, tuple->length);
        ctx->index += tuple->length;
      }
      else {
        APP_LOG(APP_LOG_LEVEL_WARNING, "Not overriding rx buffer. Bufsize=%li BufIndex=%li DataLen=%i",
          ctx->length, ctx->index, tuple->length);
      }
      break;
    case NETIMAGE_BEGIN:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Start transmission. Size=%lu", tuple->value->uint32);
      if (ctx->data != NULL) {
        free(ctx->data);
      }
      ctx->data = malloc(tuple->value->uint32);
      if (ctx->data != NULL) {
        ctx->length = tuple->value->uint32;
        ctx->index = 0;
      }
      else {
        APP_LOG(APP_LOG_LEVEL_WARNING, "Unable to allocate memory to receive image.");
        ctx->length = 0;
        ctx->index = 0;
      }
      break;
    case NETIMAGE_END:
      if (ctx->data && ctx->length > 0 && ctx->index > 0) {
        GBitmap *bitmap = gbitmap_create_with_data(ctx->data);
        if (bitmap) {
          ctx->callback(bitmap);
          // We have transfered ownership of this memory to the app. Make sure we dont free it.
          ctx->data = NULL;
          ctx->index = ctx->length = 0;
        }
        else {
          APP_LOG(APP_LOG_LEVEL_DEBUG, "Unable to create GBitmap. Is this a valid PBI?");
          // free memory
          free(ctx->data);
          ctx->data = NULL;
          ctx->index = ctx->length = 0;
        }
      }
      else {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Got End message but we have no image...");
      }
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_WARNING, "Unknown key in dict: %lu", tuple->key);
      break;
  }
}

void netimage_dropped(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Dropped message! Reason given: %i", reason);
}

void netimage_out_success(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Message sent.");
}

void netimage_out_failed(DictionaryIterator *iter, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed to send message. Reason = %i", reason);
}

