var transferInProgress = false;

/*Global URL var for image conversion server
  Replace with your server */
var toServer = "http://35418b8a.ngrok.com";

Pebble.addEventListener("ready", function(e) {
  console.log("NetDownload JS Ready");
});

Pebble.addEventListener("appmessage", function(e) {
  console.log("Got message: " + JSON.stringify(e));

  if ('NETDL_URL' in e.payload) {
    if (transferInProgress == false) {
      transferInProgress = true;
      downloadBinaryResource(e.payload['NETDL_URL'], function(bytes) {
        transferImageBytes(bytes, e.payload['NETDL_CHUNK_SIZE'],
          function() { console.log("Done!"); transferInProgress = false; },
          function(e) { console.log("Failed! " + e); transferInProgress = false; }
        );
      },
      function(e) {
        console.log("Download failed: " + e); transferInProgress = false;
      });
    }
    else {
      console.log("Ignoring request to download " + e.payload['NETDL_URL'] + " because another download is in progress.");
    }
  }
});

function downloadBinaryResource(imageURL, callback, errorCallback) {


    /*POSTing the image URL to the server for conversion.
      This conversion is only for Pebble Time. On an original
      Pebble the dithering should convert the image to have 
      to 2 colors*/
    var req1 = new XMLHttpRequest();
    req1.open("POST",toServer, false);
    imageURL = "imageurl=" + imageURL+"&PebbleType=0";
    req1.setRequestHeader("Content-type", "imageURL");
    req1.setRequestHeader("Content-length", imageURL.length);
    req1.setRequestHeader("Connection", "close");    
    req1.send(imageURL);

    //getting converted image, which is always at <server>/Pebble_image.png
    var req = new XMLHttpRequest();
    var img_return = toServer+ "/Pebble_image.png"; 
    req.open("GET", img_return, true); //do get from image url which is standard
    req.responseType = "arraybuffer";
    req.onload = function(e) {
        console.log("loaded");
        var buf = req.response;
        if(req.status == 200 && buf) {
            var byteArray = new Uint8Array(buf);
            var arr = [];
            for(var i=0; i<byteArray.byteLength; i++) {
                arr.push(byteArray[i]);
            }

            console.log("Downloaded file with " + byteArray.length + " bytes.");
            callback(arr);
        }
        else {
          errorCallback("Request status is " + req.status);
        }
    }
    req.onerror = function(e) {
      errorCallback(e);
    }
    req.send(null);
}

function transferImageBytes(bytes, chunkSize, successCb, failureCb) {
  var retries = 0;

  success = function() {
    console.log("Success cb=" + successCb);
    if (successCb != undefined) {
      successCb();
    }
  };
  failure = function(e) {
    console.log("Failure cb=" + failureCb);
    if (failureCb != undefined) {
      failureCb(e);
    }
  };

  // This function sends chunks of data.
  sendChunk = function(start) {
    var txbuf = bytes.slice(start, start + chunkSize);
    console.log("Sending " + txbuf.length + " bytes - starting at offset " + start);
    Pebble.sendAppMessage({ "NETDL_DATA": txbuf },
      function(e) {
        // If there is more data to send - send it.
        if (bytes.length > start + chunkSize) {
          sendChunk(start + chunkSize);
        }
        // Otherwise we are done sending. Send closing message.
        else {
          Pebble.sendAppMessage({"NETDL_END": "done" }, success, failure);
        }
      },
      // Failed to send message - Retry a few times.
      function (e) {
        if (retries++ < 3) {
          console.log("Got a nack for chunk #" + start + " - Retry...");
          sendChunk(start);
        }
        else {
          failure(e);
        }
      }
    );
  };

  // Let the pebble app know how much data we want to send.
  Pebble.sendAppMessage({"NETDL_BEGIN": bytes.length },
    function (e) {
      // success - start sending
      sendChunk(0);
    }, failure);

}
