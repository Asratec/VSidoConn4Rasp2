var http = require('http');
var fs = require('fs');
var mjpegServer = require('./node-mjpeg-server');

http.createServer(function(req, res) {
  console.log("Got request");

  mjpegReqHandler = mjpegServer.createReqHandler(req, res);

  var i = 0;
  var timer = setInterval(updateJPG, 20);
  var sentName = '';

  function updateJPG() {
    fs.readdir('/mnt/vsido/video/marker.detect/',function(err, files) {
		//console.log(files);
		try {
			var jpegs=files.filter(function(file) { return file.substr(-5) === '.jpeg'; });
			//console.log(jpegs);
			if(jpegs[0] && jpegs[0] != sentName) {
				fs.readFile('/mnt/vsido/video/marker.detect/' +jpegs[0], sendJPGData);
				sentName = jpegs[0];
			}
		} catch (e) {
		  console.log(e);
		}
	});
	i++;
  }

  function sendJPGData(err, data) {
	try {
		mjpegReqHandler.write(data, function() {
		//	checkIfFinished();
		});
    } catch (e) {
      console.log(e);		
    }
  }

  function checkIfFinished() {
    if (i > (60*1000/30)) {
      clearInterval(timer);
      mjpegReqHandler.close();
      console.log('End Request');
    }
  }
}).listen(18083);

