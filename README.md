pebble-faces
============

A simple app that shows how to fetch PBI images from the internet and display them on Pebble (with PebbleKit JavaScript)


## How to prepare pbi images?

This app can only download and display `.pbi` images (Pebble bitmap format). To convert a PNG to a PBI file, you can use the `bitmapgen.py` program included with the Pebble SDK:

    cd ~/pebble-dev/PebbleSDK-2.0.1
    python Pebble/tools/bitmapgen.py pbi test.png test.pbi
