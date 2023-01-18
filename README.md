# Publish-Subsribe-MQ

This project consists of broker and client sources.
Communication between these relies on message queues.
Broker handles message transfer and delivery.
Client publishes messages on a given topic and subscribe given topics.
Multiple clients can subscribe the same topic and publish and receive messages form this topic.

## Authors
Project was created for subject Computer Networks #2.

Szymon Kaczmarek 148056

Łukasz Kania 148077

# Broker functionalities

- read and parse config file
- receive messages on a given topic
- insert message to a topic
- register new clients
- every client has unique id (given or randomly generated)
- ping client after *keepalive* time and no messages were delivered to check if it is still connected
- send and receive signals from clients
- messages are not ordered
- every message is delivered at most once

# Client functionalities

- read and parse config file (gets broker ip and port from there)
- connect to a broker
- subscribe multiple topics
- publish message on given topic
- send and receive signals from broker

# Configuration

Config can be adjusted in ``config.conf`` file.
Inside given values can be configured:
- ADDRESS xx.xx.xx.xx - broker ip address
- PORT xxxx - broker port
- MAX_CLIENTS x - maximum amount of clients that can wait for connection accept at the same time
- VERBOSE (true/false) - if set to true, broker will give additional information in the terminal when running

# Signals

- CONN - connect:
    * sent from client means that it is ready to receive signals
- CONNACK - connection acknowledgement:
    * sent from broker with generated client id in response
- PING:
    * sent from broker as a check wether the client is still connected
    * sent from client in response
- SUB - subscribe:
    * sent from client with topic name to which it wants to subscribe
- SUBACK - subscribtion acknowledgement:
    * sent from broker with success code, topic name and topic messages in response
- PUB - publish:
    * sent from client with topic name and message content to publish on topic
- PUBACK - publish acknowledgement:
    * sent from broker with generated message id
- NEWMES - new message:
    * sent from broker with topic name, message id and message content to all clients subscribed to given topic
- UNSUB - unsubscribe:
    * sent from client with topic name from which it wants to be unsubscribed
- UNSUBACK - unsubscribe acknowledgement:
    * sent from broker with success code and topic name in response
- DISCONN - disconnect:
    * sent from client on close
    * sent from broker on close