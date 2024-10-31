const express = require('express');
const http = require('http');
const socketIO = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = socketIO(server);

// Store connected peers
const peers = {};

io.on('connection', (socket) => {
  console.log(`Client connected: ${socket.id}`);
  peers[socket.id] = socket;

  // Handle SDP offer/answer
  socket.on('sdp', (data) => {
    const { targetId, sdp } = data;
    console.log(`Received SDP from ${socket.id} for ${targetId}`);
    if (peers[targetId]) {
      peers[targetId].emit('sdp', { sdp, senderId: socket.id });
    }
  });

  // Handle ICE candidate
  socket.on('ice-candidate', (data) => {
    const { targetId, candidate } = data;
    console.log(`Received ICE candidate from ${socket.id} for ${targetId}`);
    if (peers[targetId]) {
      peers[targetId].emit('ice-candidate', { candidate, senderId: socket.id });
    }
  });

  socket.on('disconnect', () => {
    console.log(`Client disconnected: ${socket.id}`);
    delete peers[socket.id];
  });
});

server.listen(3000, () => {
  console.log('Signaling server is running on port 3000');
});
