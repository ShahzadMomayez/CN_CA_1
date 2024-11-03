# ComputerNetworks
# Distributed voice call using WebRTC
#### Shahzad Momayez 810100272, Amirali Shahriari 810100173

### 1. Introduction:
This project aims to develop a real-time communication platform using WebRTC, enhanced by the libdatachannel library for data transmission and the Opus codec for superior audio quality.


### List any prerequisites or dependencies:
• Qt framework 
• WebRTC (with libdatachannel) 
• Opus
• Coturn

### what is WebRTC?
WebRTC (Web Real-Time Communication) is a powerful, open-source project that enables real-time communication directly within web browsers and mobile applications. Its magic lies in facilitating peer-to-peer audio, video, and data sharing without the need for plugins or external software.

Imagine you're using a web app for a video call or multiplayer game, and WebRTC seamlessly handles the live communication between your device and your friend's. It's used in video conferencing apps, live streaming platforms, collaborative work tools, and even gaming applications.

The WebRTC API includes media capture, encoding and decoding audio and video, transportation layer, and session management.

![My Local Image](https://path-to-your-image.com/webRTC-1.png)


### Media Capture
The first step is to get access to the camera and microphone of the user's device. We detect the type of devices available, get user permission to access these devices and manage the stream.

### Encoding and Decoding Audio and Video:
It is not an easy task to send a stream of audio and video data over the Internet. This is where encoding and decoding are used. This is the process of splitting up video frames and audio waves into smaller chunks and compressing them. This algorithm is called codec. There is an enormous amount of different codecs, which are maintained by different companies with different business goals. There are also many codecs inside WebRTC like Opus.

