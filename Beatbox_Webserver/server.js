import http from 'http';
import fs from 'fs';
import mime from 'mime';
import path from 'path';
import { beagleboneServer } from './lib/beaglebone_server.js';

// var http = require('http');
var server = http.createServer(function(req, res) {
    var filePath = false;
    if (req.url == '/') {
        filePath = 'public/index.html';
    }
    else {
        filePath = 'public' + req.url;
    }
    var absPath = './' + filePath;
    serveStatic(res, absPath);
});

var PORT = 8088;
server.listen(PORT, function() {
    console.log('Server listening on port ' + PORT);
});

// var fs = require('fs');
function serveStatic(res, absPath) {
    fs.exists(absPath, function(exists) {
        if (exists) {
            fs.readFile(absPath, function(err, data) {
                if (err) {
                    send404(res);
                }
                else {
                    sendFile(res, absPath, data);
                }
            });
        }
        else {
            send404(res);
        }
    });
}

function send404(res) {
    res.writeHead(404, {'Content-Type': 'text/plain'});
    res.write('Error 404: resource not found.');
    res.end();
}

// var mime = require('mime');
// var path = require('path');
function sendFile(res, filePath, fileContents) {
    res.writeHead(200, {'Content-Type': mime.getType(path.basename(filePath)) || 'application/octet-stream'});
    res.end(fileContents);
}

beagleboneServer(server);