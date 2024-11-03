const express = require('express');
const http = require('http');
const socketIo = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = socketIo(server);

let clients = {};
let sdpBuffs = {}; // Buffer for SDPs when the counterpart is not yet connected

app.get('/', (req, res) => {
    res.send('Signaling server is running');
});

io.on('connection', (socket) => {
    console.log(`Client connected: ${socket.id}`);

    socket.on('sdp_offer', (data) => {
        try {
            const sdp = data;

            console.log(`Received SDP Offer from socket: ${socket.id}`);

            clients["offer"] = socket; // Store the offerer

            // Send offer to the answerer if available
            if (clients["answer"]) {
                console.log(`Sending SDP Offer to answerer: ${clients["answer"].id}`);
                clients["answer"].emit("sdp_offer", JSON.stringify({ sdp: sdp })); // Emit with event name
            } else {
                console.log(`Buffering offer SDP since answerer is not connected`);
                sdpBuffs["offer"] = sdp; // Buffer offer if answerer is not connected
            }

            // Send buffered answer if available
            if (sdpBuffs["answer"]) {
                console.log(`Sending buffered answer SDP to offerer: ${socket.id}`);
                socket.emit("sdp_answer", JSON.stringify({ sdp: sdpBuffs["answer"] })); // Emit buffered answer
                delete sdpBuffs["answer"]; // Clear the buffered answer
            }
        } catch (error) {
            console.error("Error parsing SDP offer:", error);
        }
    });

    socket.on('sdp_answer', (data) => {
        try {
            const sdp = data;

            console.log(`Received SDP Answer from socket: ${socket.id}`);

            clients["answer"] = socket; // Store the answerer

            // Send answer to the offerer if available
            if (clients["offer"]) {
                console.log(`Sending SDP Answer to offerer: ${clients["offer"].id}`);
                clients["offer"].emit("sdp_answer", JSON.stringify({ sdp: sdp })); // Emit with event name
            } else {
                console.log(`Buffering answer SDP since offerer is not connected`);
                sdpBuffs["answer"] = sdp; // Buffer answer if offerer is not connected
            }

            // Send buffered offer if available
            if (sdpBuffs["offer"]) {
                console.log(`Sending buffered offer SDP to answerer: ${socket.id}`);
                socket.emit("sdp_offer", JSON.stringify({ sdp: sdpBuffs["offer"] })); // Emit buffered offer
                delete sdpBuffs["offer"]; // Clear the buffered offer
            }
        } catch (error) {
            console.error("Error parsing SDP answer:", error);
        }
    });

    socket.on('disconnect', () => {
        console.log(`Client disconnected: ${socket.id}`);
        // Clean up clients
        if (clients["offer"] && clients["offer"].id === socket.id) {
            delete clients["offer"];
        }
        if (clients["answer"] && clients["answer"].id === socket.id) {
            delete clients["answer"];
        }
    });
});

const PORT = 3000;
server.listen(PORT, () => {
    console.log(`Signaling server is running on port ${PORT}`);
});
