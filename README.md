pebble-faces
============

This Pebble application downloads PNG images from the Internet, de-compress the
PNG into a bitmap and then display it on the screen of Pebble. It will display
pictures of the Pebble developer evangelism team!

It is designed to be easily reusable in your own application.

The main parts are:

 * The resource downloader (`netimage.h` / `netdownload.c` / `pebble-app.js`)
   
   This library provides a function `netdownload_request(char *url)` to download
   any URL from the Internet. The resource as to be small enough to fit in
   Pebble memory.

   You will first need to initialize the library with
   `netdownload_initialize(callback)`. The callback will be called with an array
   of bytes when the file has been downloaded.

 * The PNG image loader (`png.h`, `png.c`, `upng.h`, `upng.c`)

   This library is based on the fantastic
   [uPNG](https://github.com/elanthis/upng). The `png.h` and `png.c` provide a
   simple wrapper to make it easier to use on Pebble.

   To convert an array of bytes (for example one provided to the
   `NetDownloadCallback`) you can call `gbitmap_create_with_png_data()`.
   

[An earlier
version](https://github.com/pebble-hacks/pebble-faces/tree/legacy/pbi-loader) of
this example downloaded and loaded PBI images directly from the Internet. This
new version is superior because PNG is a much easier format to generate on your
servers and the images will be smaller on the network.

A downside of this version is the memory requirement: the uPNG library takes
some code space in your app and you will need enough memory to have the PNG and
the Bitmap loaded during decompression.

## Preparing images

To reduce size as much as possible you should prepare your PNGs to match the
size of the screen (or smaller) and to use only two colors. You should also
remove any extra information from the PNG.

Using [ImageMagick](http://www.imagemagick.org/) for example:

    convert myimage.png \
      -adaptive-resize '144x168>' \
      -fill '#FFFFFF00' -opaque none \
      -type Grayscale -colorspace Gray \
      -colors 2 -depth 1 \
      -define png:compression-level=9 -define png:compression-strategy=0 \
      -define png:exclude-chunk=all \
      myimage.pbl.png

Notes:

 - `-fill #FFFFFF00 -opaque none` makes the transparency white
 - `-adaptive-resize` with > at end means resize only if larger, and maintains aspect ration
 - we exclude png chunks to reduce size (like when image was made, author)

If you want to use [dithering](http://en.wikipedia.org/wiki/Dither) to simulate
Grey, you can use this command:

    convert myimage.png \
      -adaptive-resize '144x168>' \
      -fill '#FFFFFF00' -opaque none \
      -type Grayscale -colorspace Gray \
      -black-threshold 30% -white-threshold 70% \
      -ordered-dither 2x1 \
      -colors 2 -depth 1 \
      -define png:compression-level=9 -define png:compression-strategy=0 \
      -define png:exclude-chunk=all \
      myimage.pbl.png

### SDK 3.x

To convert your image to the palletized format (64 colors) used by Pebble Time, use:

    convert myimage.png \
      -adaptive-resize '144x168>' \
      -fill '#FFFFFF00' -opaque none \
      -dither FloydSteinberg \
      -remap pebble_colors_64.gif \
      -define png:compression-level=9 -define png:compression-strategy=0 \
      -define png:exclude-chunk=all \
      myimage.pbl.png

**Important:** You need ImageMagick to do the conversion above. Mac OS X ships
with GraphicsMagick which does not support the PNG options to compress and
remove un-needed informations.

## Resources

For more information about PNG on Pebbles, how to optimize memory usage and tips
on image processing, please refer to the [Advanced techniques
videos](https://developer.getpebble.com/events/developer-retreat-2014/) from the
Pebble Developer Retreat 2014.

