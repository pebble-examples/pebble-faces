import Image
import urllib2
import ctypes
import argparse
import os

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

    if PebbleType == 1:
        #converting to 64 color Pebble scheme and dithering image
        dithered_im = im_smaller.convert(mode='P',
          colors=64,
          palette=Image.FLOYDSTEINBERG)
    else:
        #converting to graysclae and then dithering to black and white
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
