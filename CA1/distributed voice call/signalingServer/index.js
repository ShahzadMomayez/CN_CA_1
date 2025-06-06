const express = require('express');
const http = require('http');
const socketIo = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = socketIo(server);

let clients = {};
let sdpBuffs = {}; //with logs

app.get('/', (req, res) => {
    res.send('Signaling server is running');
});

io.on('connection', (socket) => {
    console.log(`Client connected: ${socket.id}`);

    socket.on('sdp_offer', (data) => {
        try {
            const sdp = data;

            console.log(`Received SDP Offer from socket: ${socket.id}`);

            clients["offer"] = socket; 

            if (clients["answer"]) {
                console.log(`Sending SDP Offer to answerer: ${clients["answer"].id}`);
                clients["answer"].emit("sdp_offer",sdp); 
                console.log("aaaaa");
            } else {
                console.log(`Buffering offer SDP since answerer is not connected`);
                sdpBuffs["offer"] = sdp; 
            }

            if (sdpBuffs["answer"]) {
                console.log(`Sending buffered answer SDP to offerer: ${socket.id}`);
                socket.emit("sdp_answer",  sdpBuffs["answer"] );
                delete sdpBuffs["answer"]; 
            }
        } catch (error) {
            console.error("Error parsing SDP offer:", error);
        }
    });
    socket.on('answer_event', (data)=>{
        console.log("Connection");
        clients["answer"] = socket;
    }
  );
    socket.on('sdp_answer', (data) => {
        try {
            const sdp = data;

            console.log(`Received SDP Answer from socket: ${socket.id}`);

            clients["answer"] = socket; 

            
            if (clients["offer"]) {
                console.log(`Sending SDP Answer to offerer: ${clients["offer"].id}`);
                clients["offer"].emit("sdp_answer", sdp ); 
            } else {
                console.log(`Buffering answer SDP since offerer is not connected`);
                sdpBuffs["answer"] = sdp; 
            }

            if (sdpBuffs["offer"]) {
                console.log(`Sending buffered offer SDP to answerer: ${socket.id}`);
                socket.emit("sdp_offer",  sdpBuffs["offer"] ); 
                delete sdpBuffs["offer"]; 
            }
        } catch (error) {
            console.error("Error parsing SDP answer:", error);
        }
    });

    socket.on('disconnect', () => {
        console.log(`Client disconnected: ${socket.id}`);
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
