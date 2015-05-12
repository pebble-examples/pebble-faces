import png
import feedparser
import itertools
import Image
import urllib2
import ctypes
from HTMLParser import HTMLParser
from PIL import ImageEnhance
import argparse

#Get image from URL and generate pebble compliant png
def get_pebble_png(input_url):
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

    #converting to 64 color Pebble scheme and dithering image
    dithered_im = im_smaller.convert(mode='P',
     colors=64,
     palette=Image.FLOYDSTEINBERG)

    #saving dithered image as PNG
    dithered_im.save("Pebble_image.png","PNG")


def main():
    parser = argparse.ArgumentParser(
        description='Get image from URL and convert to 64-color palettized PNG for Pebble Time')
    parser.add_argument('input_url', type=str, help='URL from which to convert image')
    args = parser.parse_args()
    get_pebble_png(args.input_url, args.output_filename)

if __name__ == '__main__':
    main()
