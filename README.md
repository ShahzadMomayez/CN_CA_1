# ComputerNetworks
# Distributed voice call using WebRTC
#### Shahzad Momayez 810100272, Amirali Shahriary 810100173

## 1. Introduction:
This project aims to develop a real-time communication(peer-to-peer voice call system) platform on QT framework using WebRTC, enhanced by the libdatachannel library for data transmission and the Opus codec for superior audio quality.

## 2. List any prerequisites or dependencies:
• Qt framework 

• WebRTC (with libdatachannel) 

• Opus

• Coturn (Optional)

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

#### writeData() Method:
Validates that the Opus encoder has been initialized. Checks if the audio frame size matches Opus requirements (120, 240, 480, 960, 1920, or 2880 samples). Encodes the data: The opus_encode function compresses the audio input, writing it to outputBuffer. If encoding succeeds, the encoded data is wrapped in a QByteArray and emitted with newDataAvailable. Returns the length of input data processed, allowing seamless streaming. This class allows real-time audio data capture and encoding, handling both the low-level Opus encoding requirements and the high-level Qt audio input functionality.

NOTE :we use a header file for the audioinput class as part of organizing the code and defining the class interface.

### audiooutput.cpp:

![audiooutput.cpp](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles/audioOutputCPP.png)

#### 1.  Constructor (`AudioOutput::AudioOutput(QObject *parent)`) 
   -  Purpose : Initializes the audio playback setup and the Opus decoder.
   -  Key Steps :
     -  Audio Format Setup :
       - Creates a `QAudioFormat` instance to specify the audio properties:
         -  Sample Rate : 48,000 Hz (matches the Opus codec standard for voice).
         -  Channel Count : 1 (mono, as voice typically requires only a single channel).
         -  Sample Format : `Int16` (16-bit integer samples), which Opus encoding and decoding expect.
     -  Audio Sink Initialization :
       - Instantiates a `QAudioSink` with the specified `QAudioFormat`. This sink acts as the interface for audio playback.
       - Calls `audioSink->start()` to start the sink, which initializes a `QIODevice` (`audioDevice`) for writing the audio data directly to the audio output.
     -  Opus Decoder Initialization :
       - Calls `opus_decoder_create`, which sets up the Opus decoder with:
         - Sample Rate: 48,000 Hz.
         -  Channel Count : 1 (mono).
       - Stores any error encountered in `opusError` and logs a warning if initialization fails.

### 2.  `start()` Method 
   -  Purpose : Starts the audio output for playback.
   -  Functionality :
     - Calls `audioSink->start()` to (re)initialize `audioDevice` and begin audio playback.
     - This ensures `audioDevice` is set up and ready to receive audio data for output.
   -  Why It’s Needed : This is useful for restarting playback if it was previously stopped or if reinitialization is required.

### 3.  `stop()` Method 
   -  Purpose : Stops audio playback and cleans up resources.
   -  Functionality :
     - Calls `audioSink->stop()` to halt audio output.
     - Destroys the Opus decoder using `opus_decoder_destroy`, freeing up resources allocated to the decoder.
   -  Why It’s Needed : This method ensures that playback stops cleanly and any allocated resources are released, preventing memory leaks.

### 4.  `addData()` Slot 
   -  Purpose : Processes incoming encoded audio data, decodes it, and writes the decoded data to the audio device for playback.
   -  Functionality :
     -  Thread Safety :
       - Uses `QMutexLocker locker(&mutex);` to ensure that only one thread at a time can execute this function. This is crucial in a real-time application where audio data may arrive concurrently.
     -  Decoding Data :
       - Allocates a buffer `decodedData` of `opus_int16` type (16-bit samples) to hold the decoded output.
       - Calls `opus_decode` to decode the incoming Opus-encoded audio data (`data`):
         - `opus_decode` converts the compressed audio data in `data` into raw PCM audio samples, storing them in `decodedData`.
         - If decoding fails (i.e., `decodedSamples < 0`), it logs a warning and exits the function.
       - Converts the decoded data into a `QByteArray` (`rawData`) for easier handling.
       - Writes `rawData` directly to `audioDevice`, which plays the decoded audio through the audio sink.
   -  Why It’s Needed : This function is called whenever new audio data arrives. It decodes the compressed data and sends it to the audio output, enabling real-time audio playback for a voice call.


## 5. WebRTC:

In this project, WebRTC (Web Real-Time Communication) played a central role in establishing peer-to-peer audio connections, allowing real-time voice data exchange between two instances of the application. By leveraging WebRTC’s data channels, we facilitated a direct connection without relying on a central server to relay audio streams, significantly reducing latency. Our implementation focused on handling connection setup, SDP (Session Description Protocol) exchanges, and ICE (Interactive Connectivity Establishment) candidates, which were essential for network traversal and establishing direct links between peers. We integrated WebRTC with Qt through a custom webrtc.cpp class, managing audio tracks and synchronizing connection events. WebRTC’s protocols provided us with robust support for low-latency, high-quality audio transmission, forming the backbone of our distributed voice call system. The integration also required close attention to STUN (Session Traversal Utilities for NAT) servers for NAT traversal, enabling the instances to discover each other even across different networks. This WebRTC setup allowed us to achieve real-time, resilient peer-to-peer communication.

![schema](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles/webrtc-1.png)

The above diagram is the messaging flow between users when using the signaling server. First of all, each user registers with the server. In our case, this will be a simple string username. Once users have registered, they are able to call each other. User 1 makes an offer with the user identifier he wishes to call. The other user should answers. Finally, ICE candidates are sent between users until they can make a connection.

### webrtc.cpp:


#### Struct RtpHeader
 Purpose:  Defines the structure for the RTP (Real-time Transport Protocol) header, enabling efficient audio packet transmission.  
 Fields:   
-  first:  First byte of RTP header.  
-  marker, payloadType, sequenceNumber, timestamp, ssrc:  Fields specific to RTP header protocol.  
 Usage:  This structure is used when sending audio packets to peers.

#### Constructor and Destructor
 Constructor (WebRTC::WebRTC):   
Initializes m_audio, the audio description object, and establishes a signal connection for when ICE gathering completes.  
 Destructor (~WebRTC):   
Clears m_peerConnections on object destruction, ensuring proper cleanup of peer connections.

#### Public Methods

 init:   

![init](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles/WebrtcInit.png)


Initializes the WebRTC configuration with peer-specific details (isOfferer) and sets up ICE servers.  
Configures the audio track with the Opus codec and sets the direction (SendOnly/RecvOnly) based on whether the peer is the offerer.  
Clears previous connections and tracks.



 addPeer:   

![addpeer](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles/WebRTCaddpeer.png)

Adds a new peer connection, setting up several callbacks:  
-  Local Description Callback:  Generates SDP when local description is ready.  
-  Local Candidate Callback:  Handles local ICE candidates.  
-  State Change Callback:  Monitors and logs connection states.  
-  Gathering State Callback:  Indicates when ICE gathering is complete.  
-  Track Callback:  Monitors and opens the audio track when received from the remote peer.

 generateOfferSDP & generateAnswerSDP:  

![generateoffer](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles/WebRTC-generateOfferAndAnswerSDP-addAudioTrack.png)

Generates the local SDP offer or answer, setting it as the local description for the peer.

 addAudioTrack:   
Adds the audio track to a peer connection using the m_audio configuration, attaching it to m_peerTracks.  
Sets up message callbacks to handle incoming audio packets.

 sendTrack:   

![sendtrack](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles/WebRTCSendTrack.png)

Prepares and sends audio data to the peer by creating an RTP packet with the RtpHeader, appending the encoded audio data, and transmitting it via the WebRTC track.

#### Public Slots
 setRemoteDescription:   
Sets the remote description (SDP) provided by the peer, which includes essential session information such as codec and direction.

 setRemoteCandidate:   
Adds a remote ICE candidate after validating the candidate data and ensuring the remote description is set. This supports peer-to-peer connectivity.

#### Private Methods

![private_methods](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles/WebRTCPrivateMethods.png)

 readVariant:   
Parses the rtc::message_variant type into a QByteArray, handling different possible types like binary data or strings.

 descriptionToJson:   
Converts an rtc::Description object into JSON format, generating an SDP offer or answer in JSON for transmission.

 Bitrate and Codec Management:   
Methods like  setBitRate ,  resetBitRate ,  setPayloadType , and  resetPayloadType  control the bitrate and payload type for the audio codec.  
 setSsrc  and  resetSsrc  manage the SSRC (synchronization source identifier) for identifying the audio stream.

#### Signals and Slots
Signals such as  localDescriptionGenerated ,  localCandidateGenerated ,  gatheringComplited , and  packetRecieved  manage communication between components and signal important states (e.g., when ICE gathering is complete or audio packets are received).  
Slots support dynamic configuration during runtime, such as setting new bit rates or handling changes in payload types and SSRC.



## 6. SignalingServer:
For the signaling server, we implemented a solution to manage the critical connection setup between peers in our distributed voice call system. The signaling server, built using WebSocket technology with Node.js and socket.io, facilitates the exchange of necessary connection data, such as Session Description Protocol (SDP) information and Interactive Connectivity Establishment (ICE) candidates, between peers. This setup process, essential for establishing WebRTC connections, includes sharing offers, answers, and ICE candidates that allow peers to locate each other and establish a direct connection.

The server does not handle the media or voice data itself but operates purely as an intermediary for exchanging control messages, making it lightweight and efficient. The signalingserver class in our project manages these interactions by connecting to the WebSocket server, sending and receiving messages related to SDP and ICE candidates, and triggering the necessary functions to set up and update peer-to-peer connections. By centralizing the signaling process, we streamlined the initial connection setup, allowing peers to connect directly for voice data exchange without the server's involvement in the actual call. This approach supports scalability and maintains low-latency communication for real-time voice calls.

#### signalingserver.cpp

 Purpose:   
The `signalingserver.cpp` file implements the `SignalingServer` class, which handles signaling communication in a WebRTC-based voice call application. This class facilitates the exchange of Session Description Protocol (SDP) messages and Interactive Connectivity Establishment (ICE) candidates necessary for establishing peer-to-peer connections. The `signalingserver.cpp` file is essential for enabling communication between peers in the distributed voice call project. It manages the exchange of signaling information, such as SDP offers, SDP answers, and ICE candidates, facilitating the establishment of peer-to-peer connections using Socket.IO for reliable messaging.

![signalingserver](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles/signalingServerCPP.png)

 Constructor: 
-  SignalingServer(QObject *parent):   
  Initializes the `SignalingServer` object and sets up socket handlers for incoming signaling events.

 Methods: 

1.  connectToServer(const std::string& url):   
   Connects the signaling client to the specified server URL.

2.  setupSocketHandlers():   
   Configures event handlers for incoming signaling messages:
   -  SDP Offer Handling:  Emits the `sdpOfferReceived` signal when an SDP offer is received.
   -  SDP Answer Handling:  Emits the `sdpAnswerReceived` signal when an SDP answer is received.
   -  ICE Candidate Handling:  Emits the `iceCandidateReceived` signal when an ICE candidate is received, passing the sender's ID and candidate string.

3.  sendSDPOffer(const QString& sdp):   

![sendsdp](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles/signalingServer2.png)

    Sends an SDP offer to the signaling server. Constructs a message containing the SDP string and emits it.

5.  sendSDPAnswer(const QString& sdp):   
   Sends an SDP answer to the signaling server. Constructs a message with the SDP string and emits it.

6.  sendICECandidate(const std::string& targetId, const std::string& candidate):   
   Sends an ICE candidate to a target peer. Constructs a message containing the target peer's ID and the candidate string, then emits it.

7.  answerSocket():   
   Emits an answer event to indicate that an answer is being sent.


## 7. Building the signaling Server using NodeJs:

![schemaa](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles/BuildingServer.png)


 Purpose:   
The `signalingServer.js` file implements a signaling server using Node.js, Express, and Socket.IO. It facilitates the exchange of SDP offers, answers, and ICE candidates between clients, enabling WebRTC peer-to-peer connections.

 Dependencies: 
-  express:  Framework for building web applications.
-  http:  Module to create an HTTP server.
-  socket.io:  Library for real-time, bidirectional communication between web clients and servers.

 Server Setup: 
- Initializes an Express application and creates an HTTP server.
- Sets up Socket.IO for real-time communication.
- Defines two main objects: `clients` for tracking connected clients and `sdpBuffs` for buffering SDP messages.

 Routes: 
-  GET / :  
  Responds with a message indicating that the signaling server is running.

 Socket.IO Connection Handling: 
-  io.on('connection', (socket) => {...}):   
  Handles new client connections and sets up event listeners for each socket.

1.  'sdp_offer' Event:   
   - Listens for SDP offers from clients.
   - Logs the received SDP offer and associates it with the "offer" client.
   - If the "answer" client is connected, emits the SDP offer to that client.
   - If the "answer" client is not connected, buffers the offer for later use.
   - If a buffered answer is available, sends it to the "offer" client.

2.  'answer_event':   
   - Handles the connection event when a client identifies as the "answer" client.

3.  'sdp_answer' Event:   
   - Listens for SDP answers from clients.
   - Logs the received SDP answer and associates it with the "answer" client.
   - If the "offer" client is connected, emits the SDP answer to that client.
   - If the "offer" client is not connected, buffers the answer for later use.
   - If a buffered offer is available, sends it to the "answer" client.

4.  'disconnect' Event:   
   - Handles client disconnection.
   - Removes the client from the `clients` object based on whether they were an "offer" or "answer" client.

 Server Listening: 
- The server listens on port 3000 and logs a message indicating that the signaling server is running.

 Summary:   
The `signalingServer.js` file plays a vital role in managing signaling for WebRTC connections by facilitating the exchange of SDP offers and answers. It handles client connections, maintains state, and ensures reliable message delivery, enabling seamless peer-to-peer communication.


## 8. UI Handler:



 Purpose:   
The `uihandler.cpp` file implements the `UIHandler` class, which manages user interface interactions and connects user actions to the underlying WebRTC functionality. It handles the initiation of calls, offers, answers, and manages audio input and output streams.

![uihandler](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles/uihandlerCPP1.png)


 Constructor: 
-  UIHandler(QObject *parent):   
  Initializes the `UIHandler` object, sets up WebRTC and signaling server handlers, and connects to the signaling server at the specified URL.The `uihandler.cpp` file plays a crucial role in managing the user interface for the voice call application. It establishes connections, handles SDP offers and answers, and manages audio streams, providing a seamless experience for initiating and answering calls through WebRTC.

 Methods: 

1.  offer():   
   Initiates an offer to establish a WebRTC connection:
   - Creates an instance of `AudioInput`.
   - Initializes the WebRTC configuration for the "EOT-x" peer as the offerer.
   - Connects signaling server's `sdpAnswerReceived` signal to the `setOfferSdp` slot.
   - Connects WebRTC's `sdpGenerated` signal to send the SDP offer via the signaling server.
   - Connects WebRTC's `trackIsOpen` signal to start the audio input.
   - Connects `newDataAvailable` signal from `AudioInput` to send audio data to the WebRTC instance.
   - Adds the peer "answerer" and generates an SDP offer.

2.  answer():   
   Prepares to answer a WebRTC call:
   - Creates an instance of `AudioOutput`.
   - Initializes the WebRTC configuration for the "EOT-x" peer as the answerer.
   - Connects WebRTC's `sdpGenerated` signal to send the SDP answer via the signaling server.
   - Connects signaling server's `sdpOfferReceived` signal to the `setAnswerSdp` slot.
   - Connects WebRTC's `trackIsOpen` signal to start the audio output.
   - Connects `packetReceived` signal from WebRTC to add audio data to the `AudioOutput`.
   - Emits an answer socket event to the signaling server.
   - Adds the peer "offerer".

3.  setAnswerSdp(const QString &sdp):   

![answersdp](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles/uihandlerCPP2.png)

   Sets the remote description for the "offerer" peer with the received SDP answer.

5.  setOfferSdp(const QString &sdp):   
   Sets the remote description for the "answerer" peer with the received SDP offer.

6.  sendData(const QByteArray &data):   
   Sends audio data to the "answerer" peer using the WebRTC instance.








