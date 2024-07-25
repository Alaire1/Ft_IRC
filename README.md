# IRC

![""](HexChat.png)
An IRC server is the central piece of **I**nternet **R**elay **C**hat, acting like a meeting point for users who want to chat with each other. IRC server acts as a central hub, facilitating connections between clients and enabling communication across channels 
and networks Here's a breakdown of the key components:
- **Server Software:** This is the core program running on a computer, handling connections from IRC clients and managing communication between them. There are various open-source and proprietary IRC server software options available.
- **Clients:** These are the applications users run on their devices (phones, laptops) to connect to the IRC server and participate in chats. Some popular IRC clients include mIRC, HexChat, and irssi.
- **Networks:** An IRC network is a collection of interconnected servers that allows users on any server within the network to communicate with each other. This enables a wider reach compared to a single server.
- **Channels:** These are virtual chat rooms within the network where users can gather and discuss specific topics or interests. Users can join and leave channels at will.
- **Protocol (IRC):** This is the set of rules and commands that define how IRC servers and clients communicate with each other. It uses TCP for data transfer and optionally TLS for encryption.

## Channel
An IRC channel is a virtual room where multiple users can chat simultaneously. It's like a public chat room with a specific topic or interest. Users can join and leave channels as they please, and messages sent in a channel are visible to everyone in that channel. 
### Channel commands:
Our IRC server has to cover commands JOIN, INVITE, KICK, MODE, TOPIC, PRIVMSG. Those can be used by registered users that have to provide correct PASS, and unique NICK nad USER credentials.
The client is sending command that is then being parsed by the server and then server is sending a response, in a following format

```
[prefix] command [parameters] [trailing parameters]
:ourSuperServer 001 client1 :Welcome to ft_irc server! - example
```

 **JOIN** - join and create channel if it doesn't exist
  - Has only one parameter `channel name` 
   - Channel name has to start with a `#` cannot contain bell signs, spaces and a commas, alse cannot be longer than 50 characters </br>
   - `JOIN #channelName` - sent from client 
   -  If the channel doesn't exist it is also creating it's instance and the person becomes an operator of that channel.
     
**TOPIC** - Change or view the channel topic 
  
 -  Has two parameters `#channelName` and  `topic` that is a description of the channel content. Can have several words, starts with `:`
 -  `TOPIC  #channelName :This is channel topic`
   
**INVITE** - Invite a client to a channel
  
  - Has two parameters `nickname` and  `#channelNane`
  - `INVITE client2 #channelName`

**KICK** -  Eject a client from the channel
  - Has three parameters `#channelName`, `nickname` and `message`. If message is not provided, the default one is being taken as a parameter.
  - In order to to use you have to be an operator of a channel, normal users cannot kick anyone. Operators can kick other operators, but cannot kick themselves.
  - `KICK #channelName client2 :you are being kicked`

**MODE** - Change the channel’s mode or see active modes
Modes are to be used by the operators of the channel, only displaying current active modes can be done by any member of a channel.
Mode has several options to choose: 

**no mode**
  - it's taking only one parameter `#channelName`
  - `MODE #channelName` - example
  - displaying current active channel modes `#channel Modes on #channel are +ti`
  - only one that can be used by any member of channel
    
**i**: Set/remove Invite-only channel
  - takes two parameters `#channelName` and either `+i` to turn invite only mode or `-i` to turn it off.
  - `MODE #channelName +o` - example
  - after being used clients cannot just join channel they have to be invited first
    
**t**: Set/remove the restrictions of the TOPIC command to channel
  - it's taking `#channelName` and `+t` or `-t`
  - `MODE #channelName -t` - example
  - if it's on the `TOPIC` command can only be used by an operator of this channel

**k**: Set/remove the channel key (password)
  - if setting on takes three params `#channelName`, `+k` and `password` (password has to be one word)
  - `MODE #channelName +k <password>` - example
  - if this mode is on everyone who would like to join the channel has to use correct password while joining `JOIN ##channel <pass>`
  - when channel key removed we only need `#channelName` and `-k`
  - `MODE #channelName -k`

**o**: Give/take channel operator privilege
  - takes three params `#channelName`, `+o` or `-o` and `nickname` of person we want to have or not to have operator rights anymore
  - `MODE #channelName -o client2` - example
**l**: Set/remove the user limit to channel
    - option `+l` needs third param `limit of users`, after reaching the limit, the `JOIN` command throws an error
    - `MODE #channelName +l 10` - example
    - `-l`:`MODE #channelName -l` -example
      
**PRIVMSG** - send a message to the other client or to the channel
  - takes three params `#channelName` or `nick` and `message`
  - `PRIVMSG #channel :this is my message` - this version is being send to all the clients on this channel
  - `PRIVMSG client2 :message text` - this one is being sent just to one client that we have targetted 
    
**QUIT** - Disconnets from the server
  - takes one param which is leaving message, if none give will take the default one
  - `QUIT :Leaving` - example
  - deletes client from the list of the server clients
  



