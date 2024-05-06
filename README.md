# IRC
An IRC server is the central piece of **I**nternet **R**elay **C**hat, acting like a meeting point for users who want to chat with each other. IRC server acts as a central hub, facilitating connections between clients and enabling communication across channels 
and networks Here's a breakdown of the key components:
- **Server Software:** This is the core program running on a computer, handling connections from IRC clients and managing communication between them. There are various open-source and proprietary IRC server software options available.
- **Clients:** These are the applications users run on their devices (phones, laptops) to connect to the IRC server and participate in chats. Some popular IRC clients include mIRC, HexChat, and irssi.
- **Networks:** An IRC network is a collection of interconnected servers that allows users on any server within the network to communicate with each other. This enables a wider reach compared to a single server.
- **Channels:** These are virtual chat rooms within the network where users can gather and discuss specific topics or interests. Users can join and leave channels at will.
- **Protocol (IRC):** This is the set of rules and commands that define how IRC servers and clients communicate with each other. It uses TCP for data transfer and optionally TLS for encryption.

  ## Server
The addrinfo structure, defined in **<sys/socket.h>**, plays a crucial role in network programming for specifying address information and creating sockets. It contains several fields, and three of them are particularly important for socket creation:

**ai_family:**  This field specifies the address family, which defines the addressing scheme used for the socket. Here are some common values:
  - AF_INET: This indicates the use of IPv4 addressing, the most common version of the Internet Protocol.
  - AF_INET6: This signifies the use of IPv6 addressing, the next generation of IP with a larger address space.
  - Other address families like AF_UNIX for local communication might exist depending on the system.

**ai_socktype:** This field defines the type of socket you want to create. The two most common types are:
  - SOCK_STREAM: This creates a stream socket, which provides a reliable, connection-oriented communication channel. Data is delivered in the order it was sent, and all bytes are guaranteed to be received. This is suitable for protocols like TCP that require reliable data transfer.
  - SOCK_DGRAM: This creates a datagram socket, which provides a connectionless, unreliable communication channel. Datagrams (packets) are sent independently without guaranteed delivery or order. This is suitable for protocols like UDP where speed and simplicity are more important than reliability.

**ai_protocol:** This field specifies the protocol to be used with the socket. In many cases, it's set to 0, allowing the system to choose the default protocol for the specified address family and socket type. However, for specific protocols with custom formats (like SCTP), you might need to set this to a specific protocol number.
