const express = require('express');
const http = require('http');
const socketIO = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = socketIO(server);

// Store connected peers
const peers = {};

// Handle incoming WebSocket connections
io.on('connection', (socket) => {
  console.log(`Client connected: ${socket.id}`);

  // Store the newly connected peer
  peers[socket.id] = socket;

  // When an SDP offer or answer is received
  socket.on('sdp', (data) => {
    const { targetId, sdp } = data;
    if (peers[targetId]) {
      peers[targetId].emit('sdp', { sdp, senderId: socket.id });
    }


  // When an ICE candidate is received
 
  });

  // Handle peer disconnection
  socket.on('disconnect', () => {
    console.log(`Client disconnected: ${socket.id}`);
    delete peers[socket.id];
  });
});

server.listen(3000, () => {
  console.log('Signaling server is running on port 3000');
});
