/*
  Prerequisites:

    1. Install node.js and npm (sudo apt-get install npm)
    2. npm install ws

  See also,

    http://einaros.github.com/ws/

  To run,

    node mock-webdash-server.js
*/

"use strict"; // http://ejohn.org/blog/ecmascript-5-strict-mode-json-and-more/
var WebSocketServer = require('ws').Server;
var http = require('http');

var server = http.createServer();
var wss = new WebSocketServer({server: server, path: '/'});
wss.on('connection', function(ws) {
    console.log('client connected');
    ws.on('message', function(data, flags) {
        if (flags.binary) { return; }
        console.log('>>> ' + data);
    });
    ws.on('close', function() {
      console.log('Connection closed!');
    });
    ws.on('error', function(e) {
    });
});
server.listen(8126);
console.log('Listening on port 8126...');
