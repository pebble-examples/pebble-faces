from URLtoPebblePng import get_pebble_png
import SimpleHTTPServer
import SocketServer
import cgi
import sys
import urlparse

PORT = 8000

class ServerHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):

    def do_GET(self):
    	# For a get request, just return the Pebble_image.png
    	# This is taken care of by the simeple HTTP server
    	# request handler
        return SimpleHTTPServer.SimpleHTTPRequestHandler.do_GET(self)

    def do_POST(self):

     	# Get the length of the image URL from the HTTP headers
    	length = int(self.headers['Content-length'])

    	# Storing data in a post data structure.
    	# Can send multiple segments of data which can be decoded and
    	# stored in post_data.
    	post_data = urlparse.parse_qs(self.rfile.read(length).decode('utf-8'))

    	# Image url is stored at the imageurl key
    	ImageURL = str(post_data["imageurl"])
        print ImageURL

        # Get type of Pebble you want to do the conversion for
        PebbleType = str(post_data["PebbleType"])
        print PebbleType

	  	# Using substring to remove excess characters
    	ImageURL = ImageURL[3:-2]
        PebType = int(PebbleType[3:-2])

    	#performing the conversion for that image
    	get_pebble_png(ImageURL,PebType)
    	#sending an OK response to the caller
    	self.send_response(200)




Handler = ServerHandler

httpd = SocketServer.TCPServer(("", PORT), Handler)

# port that the server is running on
print "Serving at port 8000 forever!"
httpd.serve_forever()
