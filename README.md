# ComputerNetworks
# Distributed voice call using WebRTC
#### Shahzad Momayez 810100272, Amirali Shahriary 810100173

## 1. Introduction:
This project aims to develop a real-time communication(peer-to-peer voice call system) platform on QT framework using WebRTC, enhanced by the libdatachannel library for data transmission and the Opus codec for superior audio quality.

## 2. List any prerequisites or dependencies:
• Qt framework 

• WebRTC (with libdatachannel) 

• Opus

• Coturn

## 3.   Voice Capture:

For voice capture, we implemented an AudioInput class that captures audio directly from the microphone. Using the Qt QAudioSource API, this class continuously retrieves audio data in real-time, which is then passed to an Opus encoder for compression. This encoded data is lightweight and optimized for low-latency transmission over the network, ensuring that audio remains clear and responsive. By integrating this with WebRTC, we were able to stream the captured audio efficiently to the other peer, enabling smooth, real-time voice communication between instances.


## 4. Encoding and Decoding Audio using Opus:

In our project, we used the Opus codec for both encoding and decoding audio, optimizing for real-time communication. On the capturing end, the AudioInput class takes raw audio data from the microphone and passes it to the Opus encoder. This encoding process compresses the audio data significantly while preserving voice quality, which is essential for reducing bandwidth usage and minimizing transmission delays. The compressed data is then sent over WebRTC to the receiving peer.

On the receiving end, the AudioOutput class takes the incoming compressed audio packets and decodes them using the Opus decoder. This decoding restores the audio to a near-original quality format, which is then fed into the QAudioSink for playback. By using Opus for both encoding and decoding, we achieved efficient data transmission and maintained high-quality, low-latency audio, essential for a smooth and natural communication experience in our distributed voice call application.

### audioinput.cpp:
 
![audioinput.cpp](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles/audioinputCPP.png)

#### Constructor:
Sets the audio format, specifying: Sample rate (48,000 Hz). Mono channel (1 channel). Int16 sample format, which is standard for Opus. Creates a new QAudioSource with the given devinfo and specified format. Initializes the Opus encoder with opus_encoder_create, checking for errors.

#### start() Method:
Opens the QIODevice in write-only mode, indicating that audio data will be written to it. Starts capturing audio data with audioSource->start(this), setting this class as the audio receiver. Logs a message indicating the start of recording.

#### stop() Method:
Stops the audio source and closes the QIODevice. Logs a message indicating the end of recording.

### writeData() Method:
Validates that the Opus encoder has been initialized. Checks if the audio frame size matches Opus requirements (120, 240, 480, 960, 1920, or 2880 samples). Encodes the data: The opus_encode function compresses the audio input, writing it to outputBuffer. If encoding succeeds, the encoded data is wrapped in a QByteArray and emitted with newDataAvailable. Returns the length of input data processed, allowing seamless streaming. This class allows real-time audio data capture and encoding, handling both the low-level Opus encoding requirements and the high-level Qt audio input functionality.

## 5. WebRTC:

In this project, WebRTC (Web Real-Time Communication) played a central role in establishing peer-to-peer audio connections, allowing real-time voice data exchange between two instances of the application. By leveraging WebRTC’s data channels, we facilitated a direct connection without relying on a central server to relay audio streams, significantly reducing latency. Our implementation focused on handling connection setup, SDP (Session Description Protocol) exchanges, and ICE (Interactive Connectivity Establishment) candidates, which were essential for network traversal and establishing direct links between peers. We integrated WebRTC with Qt through a custom webrtc.cpp class, managing audio tracks and synchronizing connection events. WebRTC’s protocols provided us with robust support for low-latency, high-quality audio transmission, forming the backbone of our distributed voice call system. The integration also required close attention to STUN (Session Traversal Utilities for NAT) servers for NAT traversal, enabling the instances to discover each other even across different networks. This WebRTC setup allowed us to achieve real-time, resilient peer-to-peer communication.


## 6. SignalingServer:
For the signaling server, we implemented a solution to manage the critical connection setup between peers in our distributed voice call system. The signaling server, built using WebSocket technology with Node.js and socket.io, facilitates the exchange of necessary connection data, such as Session Description Protocol (SDP) information and Interactive Connectivity Establishment (ICE) candidates, between peers. This setup process, essential for establishing WebRTC connections, includes sharing offers, answers, and ICE candidates that allow peers to locate each other and establish a direct connection.

The server does not handle the media or voice data itself but operates purely as an intermediary for exchanging control messages, making it lightweight and efficient. The signalingserver class in our project manages these interactions by connecting to the WebSocket server, sending and receiving messages related to SDP and ICE candidates, and triggering the necessary functions to set up and update peer-to-peer connections. By centralizing the signaling process, we streamlined the initial connection setup, allowing peers to connect directly for voice data exchange without the server's involvement in the actual call. This approach supports scalability and maintains low-latency communication for real-time voice calls.

## 7. Building the Server

The server we are going to build will be able to connect two users together who are not located on the same computer. We will create our own signaling mechanism. Our signaling server will allow one user to call another. Once a user has called another, the server passes the offer, answer, ICE candidates between them and setup a WebRTC connection.
///image build server
The above diagram is the messaging flow between users when using the signaling server. First of all, each user registers with the server. In our case, this will be a simple string username. Once users have registered, they are able to call each other. User 1 makes an offer with the user identifier he wishes to call. The other user should answers. Finally, ICE candidates are sent between users until they can make a connection.

To create a WebRTC connection clients have to be able to transfer messages without using a WebRTC peer connection. This is where we will use HTML5 WebSockets – a bidirectional socket connection between two endpoints – a web server and a web browser. 

##8. ICE Candidates

The final part is handling ICE candidate between users. We use the same technique just passing messages between users. The main difference is that candidate messages might happen multiple times per user in any order.

## Step-by-Step Guide to Creating a WebRTC Peer Connection
Creating an RTCPeerConnection Object
Start by creating an RTCPeerConnection object with the configuration for STUN servers. STUN servers help you discover your public IP address.

Setting up the onicecandidate Event
Set a handler for the onicecandidate event to receive and send ICE candidates. ICE candidates are network addresses that might be used to establish a connection with another peer.

Setting up the ontrack Event
Set a handler for the ontrack event to receive media streams from the other peer. This event is triggered when the other peer sends a media stream (like audio or video).

Adding Local Media Streams
Capture local media streams (such as audio and video) and add their tracks to the RTCPeerConnection.

Managing ICE Candidates
Gathering and Sending ICE Candidates: When your browser finds an ICE candidate, the onicecandidate event is triggered. You should send these candidates to the other peer, typically via a signaling server (like Socket.IO).

Receiving and Adding ICE Candidates: On the receiving side, accept ICE candidates via the signaling server and add them to the RTCPeerConnection.

Exchanging SDP (Session Description Protocol)
Creating an SDP Offer: The peer initiating the call creates an SDP offer, which includes media parameters that can be sent and received.

Handling the SDP Offer: The receiving peer sets the SDP offer as the remote description, then creates an SDP answer and sets it as the local description.

Handling the SDP Answer: The initiating peer sets the received SDP answer as the remote description.

Managing RTP (Real-time Transport Protocol)
Adding Media Tracks: Add media tracks (audio and video) to the RTCPeerConnection. These tracks are automatically transmitted using RTP.

Receiving Media Tracks: Configure the ontrack event to receive media tracks from the other peer and display them in the user interface.


## 6. here is our code:
![My Local Image](https://path-to-your-image.com/audioinputH.png)

## AudioInput
### Constructor:

Sets the audio format, specifying:
Sample rate (48,000 Hz).
Mono channel (1 channel).
Int16 sample format, which is standard for Opus.
Creates a new QAudioSource with the given devinfo and specified format.
Initializes the Opus encoder with opus_encoder_create, checking for errors.

#### start() Method:
Opens the QIODevice in write-only mode, indicating that audio data will be written to it.
Starts capturing audio data with audioSource->start(this), setting this class as the audio receiver.
Logs a message indicating the start of recording.

#### stop() Method:
Stops the audio source and closes the QIODevice.
Logs a message indicating the end of recording.

#### writeData() Method:
Validates that the Opus encoder has been initialized.
Checks if the audio frame size matches Opus requirements (120, 240, 480, 960, 1920, or 2880 samples).
Encodes the data:
The opus_encode function compresses the audio input, writing it to outputBuffer.
If encoding succeeds, the encoded data is wrapped in a QByteArray and emitted with newDataAvailable.
Returns the length of input data processed, allowing seamless streaming.
This class allows real-time audio data capture and encoding, handling both the low-level Opus encoding requirements and the high-level Qt audio input functionality.


## AudioOutput:
Here’s an in-depth explanation of the functions in the `.cpp` file for `AudioOutput`, focusing on each function’s role in managing audio playback and decoding:

### 1. **Constructor (`AudioOutput::AudioOutput(QObject *parent)`)**
   - **Purpose**: Initializes the audio playback setup and the Opus decoder.
   - **Key Steps**:
     - **Audio Format Setup**:
       - Creates a `QAudioFormat` instance to specify the audio properties:
         - **Sample Rate**: 48,000 Hz (matches the Opus codec standard for voice).
         - **Channel Count**: 1 (mono, as voice typically requires only a single channel).
         - **Sample Format**: `Int16` (16-bit integer samples), which Opus encoding and decoding expect.
     - **Audio Sink Initialization**:
       - Instantiates a `QAudioSink` with the specified `QAudioFormat`. This sink acts as the interface for audio playback.
       - Calls `audioSink->start()` to start the sink, which initializes a `QIODevice` (`audioDevice`) for writing the audio data directly to the audio output.
     - **Opus Decoder Initialization**:
       - Calls `opus_decoder_create`, which sets up the Opus decoder with:
         - **Sample Rate**: 48,000 Hz.
         - **Channel Count**: 1 (mono).
       - Stores any error encountered in `opusError` and logs a warning if initialization fails.

### 2. **`start()` Method**
   - **Purpose**: Starts the audio output for playback.
   - **Functionality**:
     - Calls `audioSink->start()` to (re)initialize `audioDevice` and begin audio playback.
     - This ensures `audioDevice` is set up and ready to receive audio data for output.
   - **Why It’s Needed**: This is useful for restarting playback if it was previously stopped or if reinitialization is required.

### 3. **`stop()` Method**
   - **Purpose**: Stops audio playback and cleans up resources.
   - **Functionality**:
     - Calls `audioSink->stop()` to halt audio output.
     - Destroys the Opus decoder using `opus_decoder_destroy`, freeing up resources allocated to the decoder.
   - **Why It’s Needed**: This method ensures that playback stops cleanly and any allocated resources are released, preventing memory leaks.

### 4. **`addData()` Slot**
   - **Purpose**: Processes incoming encoded audio data, decodes it, and writes the decoded data to the audio device for playback.
   - **Functionality**:
     - **Thread Safety**:
       - Uses `QMutexLocker locker(&mutex);` to ensure that only one thread at a time can execute this function. This is crucial in a real-time application where audio data may arrive concurrently.
     - **Decoding Data**:
       - Allocates a buffer `decodedData` of `opus_int16` type (16-bit samples) to hold the decoded output.
       - Calls `opus_decode` to decode the incoming Opus-encoded audio data (`data`):
         - `opus_decode` converts the compressed audio data in `data` into raw PCM audio samples, storing them in `decodedData`.
         - If decoding fails (i.e., `decodedSamples < 0`), it logs a warning and exits the function.
       - Converts the decoded data into a `QByteArray` (`rawData`) for easier handling.
       - Writes `rawData` directly to `audioDevice`, which plays the decoded audio through the audio sink.
   - **Why It’s Needed**: This function is called whenever new audio data arrives. It decodes the compressed data and sends it to the audio output, enabling real-time audio playback for a voice call.

### Summary
Each function in `AudioOutput.cpp` serves a specific role in enabling real-time, decoded audio playback:
- The **constructor** initializes necessary components (audio format, audio sink, and Opus decoder).
- **`start()`** and **`stop()`** manage the audio sink’s lifecycle for starting and stopping playback.
- **`addData()`** decodes incoming audio data and plays it in real time, supporting continuous, seamless playback in a voice communication setup.

## WebRTC:
//image
In a WebRTC application, the signaling server isn’t directly involved in transmitting media. Instead, it facilitates the exchange of metadata required to set up a direct connection between peers. Here’s a quick recap of how the SignalingServer class works as part of this process:

1. Connection: connectToServer() establishes the connection to the signaling server, creating the foundation for exchanging data.
2. Event Listeners: setupSocketHandlers() defines how the application should respond to incoming SDP and ICE data.
3. Data Sending: sendSDP() and sendICECandidate() send SDP and ICE candidate information to the server, which it then forwards to other peers.
4. Signals: The class emits sdpReceived and iceCandidateReceived signals to notify the application when new SDP or ICE candidate data is received from the server.
This SignalingServer class acts as an intermediary in the signaling process, bridging the initial connection between peers and enabling the exchange of SDP and ICE data necessary for setting up the WebRTC connection for our distributed voice call application.


