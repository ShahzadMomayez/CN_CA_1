## What is Coturn?
Coturn is an open-source implementation of a TURN (Traversal Using Relays around NAT) and STUN (Session Traversal Utilities for NAT) server. It's widely used in WebRTC applications to facilitate communication between peers when direct connections are blocked by NATs (Network Address Translators) or firewalls1. Essentially, Coturn helps relay traffic between peers, ensuring smooth and secure communication.

## How Coturn Works
#### TURN Server: Coturn acts as a TURN server, relaying traffic between peers when a direct connection isn't possible. This is especially useful in scenarios where peers are behind NATs or firewalls1.

#### STUN Server: Coturn also functions as a STUN server, helping peers discover their public IP addresses and the type of NAT they are behind.

####  Relay Mechanism: When a direct connection between peers isn't possible, Coturn relays the traffic through its server, ensuring that the communication can still take place.



## Coturn
#### Advantages:

Reliable NAT traversal: Coturn is a popular open-source TURN server that helps in NAT traversal, allowing devices behind NAT to communicate with each other.

High performance: It is designed to handle a large number of concurrent connections efficiently.

Open-source: Being open-source, it is free to use and modify, with a large community for support.

Cross-platform: It can be deployed on various operating systems, including Linux, Windows, and macOS.

#### Disadvantages:

Complex setup: Setting up Coturn can be complex, especially for beginners.

Resource-intensive: It may require significant computational resources to handle a large number of connections.

Maintenance: As an open-source project, it may lack the dedicated support and regular updates that commercial solutions offer.

## Signaling Server

![schema](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles2/Signalingserver.png)

####  Advantages:

Facilitates communication: Signaling servers are crucial for establishing and managing communication sessions, such as voice and video calls.

Scalability: They can handle a large number of concurrent sessions and users.

Flexibility: Supports various communication protocols like SIP and WebRTC.

Intermediary role: Helps in negotiating communication parameters and ensuring smooth connections.

####  Disadvantages:

Latency: Introducing an intermediary can add latency to the communication process.

Complexity: Implementing and managing signaling servers can be complex and require technical expertise.

Security: Ensuring the security of signaling servers is crucial, as they handle sensitive information.

## Qt
####  Advantages:

Cross-platform: Qt is a powerful framework that supports development for multiple platforms, including Windows, Linux, macOS, Android, and iOS.

Rich set of tools: Provides a comprehensive set of tools for GUI development, networking, and more.

Community support: A large and active community provides extensive resources and support.

Versatility: Supports multiple programming languages like C++, Python, and JavaScript.

####  Disadvantages:

Learning curve: Qt has a steep learning curve, especially for beginners.

Resource usage: Can be resource-intensive, especially for complex applications.

Licensing: While Qt offers flexible licensing options, understanding and complying with the licensing terms can be challenging.

## STUN (Session Traversal Utilities for NAT)

![schema](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles2/STUN.png)
### What is a STUN Server?

Simple Explanation

STUN servers allow a client device to discover its own Public IP address.

In situations where a client device is behind a NAT device usually a router, (devices behind a NAT have private IP address and all their data is routed through a single Public IP address by the NAT device usually a router)

Client devices do not know what their public IP address is, they only know their private IP address

These devices ping the STUN server and the STUN server replies back with the IP address and port number with which it is being pinged.

This is relayed back to the NAT device first which then forwards it to the client

the client device can then share the public IP and port with another device on the internet in order to establish P2P communication

#### Role of the STUN Server

    Discovery of Public IP and port A STUN server which is located on the internet, helps the client devices discover what their Public IP address and PORT number is.

This is achieved when the client sends a request to the STUN server and when then replies back with the devices Public IP and PORT number.

    Type of NAT Determination: STUN can be used to determine, client are behind what knd of NAT.

This kind of information can used to establish effective communication strategies behind different kinds of NAT

    Connection Establishment : One the external IP address and PORT number of the clients are known, the client then shares this information with its peer clinet and they can establish direct communication with each other using WebRTC or any other protocol.

this can be useful in VoIP, video calling, online gaming, and other real-time communications.


####  Advantages:

NAT traversal: STUN helps devices behind NAT to discover their public IP address and port, facilitating communication.

Simple setup: Relatively easy to set up and integrate into applications.

Open standard: Based on an open standard, making it widely supported and interoperable.

####  Disadvantages:

Limited functionality: STUN alone may not be sufficient for all NAT traversal scenarios, especially in complex network environments.

Dependence on public servers: Relies on STUN servers, which may not always be available or reliable.

## TURN (Traversal Using Relays around NAT)
![schema](https://github.com/ShahzadMomayez/CN_CA_1/blob/master/ReadmeFiles2/STUN vs. TURN.png)
#### Advantages:

Reliable NAT traversal: TURN is used when STUN fails, providing a relay server to facilitate communication.

High availability: Can be used as a fallback mechanism to ensure connectivity.

Supports various protocols: Works with multiple communication protocols, including WebRTC.

#### Disadvantages:

Resource-intensive: Requires significant computational resources to handle relaying traffic.

Latency: Introducing a relay server can add latency to the communication process.

Complexity:
