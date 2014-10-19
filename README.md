pebble-faces
============

This Pebble application downloads PNG images from the Internet, de-compress the PNG into a bitmap and then display it on the screen of Pebble.

It is designed to be easily reusable in your own application.

The main parts are:

 * The resource downloader (`netimage.h` / `netdownload.c` / `pebble-app.js`)
   
   This library provides a function `netdownload_request(char *url)` to download any URL from the Internet. The resource as to be small enough to fit in Pebble memory.
   You will first need to initialize the library with `netdownload_initialize(callback)`. The callback will be called with an array of bytes when the file has been downloaded.

 * The PNG image loader (`png.h`, `png.c`, `upng.h`, `upng.c`)

   This library is based on the fantastic [uPNG](https://github.com/elanthis/upng). The `png.h` and `png.c` provide a simple wrapper to make it easier to use on Pebble.

   To convert an array of bytes (for example one provided to the `NetDownloadCallback`) you can call `gbitmap_create_with_png_data()`.
   

[An earlier version](https://github.com/pebble-hacks/pebble-faces/tree/legacy/pbi-loader) of this example downloaded and loaded PBI images directly from the Internet. This new version is superior because PNG is a much easier format to generate on your servers and the images will be smaller on the network.

A downside of this version is the memory requirement: the uPNG library takes some code space in your app and you will need enough memory to have the PNG and the Bitmap loaded during decompression.

