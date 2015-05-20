from PIL import Image
import urllib2
import ctypes
import argparse
import os
import png

# Create pebble 64 colors-table (r, g, b - 2 bits per channel)
def pebble_get_64color_palette():
    pebble_palette = []
    for i in xrange(0, 64):
        pebble_palette.append(((i >> 4) & 0x3) * 85)   # R
        pebble_palette.append(((i >> 2) & 0x3) * 85)   # G
        pebble_palette.append(((i     ) & 0x3) * 85)   # B
    return pebble_palette

#Get image from URL and generate pebble compliant png
def get_pebble_png(input_url, PebbleType):
    img = urllib2.urlopen(input_url)
    localFile = open('TempImage.jpg', 'wb')
    localFile.write(img.read())
    localFile.close()
    #Getting current aspect ratio of image
    tempIm = Image.open('TempImage.jpg')
    #first index is the width
    width = tempIm.size[0]
    #second indes is the height
    height = tempIm.size[1]
    #maintaining aspect ratio of image
    ratio = min(144/float(width),168/float(height))
    size = int(float(width*ratio)),int(float(height*ratio))
    #resizing image to fit Pebble screen
    im_smaller = tempIm.resize(size,Image.ANTIALIAS)
    Palet = pebble_get_64color_palette()
    if PebbleType == 1:
        # Two step conversion process for using Pebble Time palette
        # and then dithering image
        paletteIm = Image.new('P',size)
        paletteIm.putpalette(Palet * 4)
        dithered_im = im_smaller.convert(mode='RGB',
                                         dither=1,
                                         palette=paletteIm)

        dithered_im = dithered_im.convert(mode='P',
                                         dither=1,
                                         colors=64,
                                         palette=Image.FLOYDSTEINBERG)
    else:
        #converting to grayscale and then dithering to black and white
        im_tempo = im_smaller.convert('LA')
        dithered_im = im_smaller.convert('1')
    #saving dithered image as PNG
    dithered_im.save("Pebble_image.png","PNG")
    os.remove("TempImage.jpg")

def main():
    parser = argparse.ArgumentParser(
        description='Get image from URL and convert to 64-color palettized PNG for Pebble Time')
    parser.add_argument('input_url', type=str, help='URL from which to convert image')
    parser.add_argument('PebbleType', type=int, help='0 is OG Pebble, 1 is Pebble Time')
    args = parser.parse_args()
    get_pebble_png(args.input_url, args.PebbleType)

if __name__ == '__main__':
    main()
