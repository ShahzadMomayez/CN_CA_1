# ComputerNetworks
# Distributed voice call using WebRTC
#### Shahzad Momayez 810100272, Amirali Shahriari 810100173

## 1. Introduction:
This project aims to develop a real-time communication platform using WebRTC, enhanced by the libdatachannel library for data transmission and the Opus codec for superior audio quality.

## 2. List any prerequisites or dependencies:
• Qt framework 
• WebRTC (with libdatachannel) 
• Opus
• Coturn

## 3. what is WebRTC?
WebRTC (Web Real-Time Communication) is a powerful, open-source project that enables real-time communication directly within web browsers and mobile applications. Its magic lies in facilitating peer-to-peer audio, video, and data sharing without the need for plugins or external software.

Imagine you're using a web app for a video call or multiplayer game, and WebRTC seamlessly handles the live communication between your device and your friend's. It's used in video conferencing apps, live streaming platforms, collaborative work tools, and even gaming applications.

The WebRTC API includes media capture, encoding and decoding audio and video, transportation layer, and session management.

![My Local Image](https://path-to-your-image.com/webRTC-1.png)


## 4. Media Capture
The first step is to get access to the camera and microphone of the user's device. We detect the type of devices available, get user permission to access these devices and manage the stream.

## 5. Encoding and Decoding Audio and Video:
It is not an easy task to send a stream of audio and video data over the Internet. This is where encoding and decoding are used. This is the process of splitting up video frames and audio waves into smaller chunks and compressing them. This algorithm is called codec. There is an enormous amount of different codecs, which are maintained by different companies with different business goals. There are also many codecs inside WebRTC like Opus.

## 6. here is our audioinput.h code:
![My Local Image](https://path-to-your-image.com/audioinputH.png)

#### .cpp File Breakdown
###Constructor:

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
