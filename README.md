# Publish-Subsribe-MQ

This project consists of two parts: broker and client library.
Communication between client & broker relies on message queues.
Broker handles message transfer and delivery.
Client publishes messages on a given topic and subscribe given topics.
Multiple clients can subscribe the same topic and receive the same message.
Multiple clients can publish on the same topic.

## Authors
Project was created for subject Computer Networks part 2.

Szymon Kaczmarek 148056
Łukasz Kania 148077

# Broker functionalities

- read and parse ``broker.conf`` config file
- receive messages on a given topic
- insert message to a topic queue
- register new clients
- every client has unique id (given or randomly generated)
- ping client after *keepalive* time and no messages were delivered to check if it is still connected
- send signals to clients as acknowledgements - callbacks
- messages are not ordered
- every message is delivered at most once

## Broker configuration

Broker can be adjusted in **config.h** file.
Inside, listener port and ip can be configured.

To Be Announced...

# Client functionalities

- optional user given id
- connect to a broker
- subscribe multiple topics
- publish message on given topic
- callbacks:
    * on_connect
    * on_subscribe
    * on_publish
    * on_message
    * on_unsubscribe
    * on_disconnect
- *loop_forever* - client runs on main process
- *loop_start* - client runs in thread, begin callbacks processing
- *loop_stop* - stop callbacks processing
- *keepalive* parameter - time after broker pings client if there weren't any messages

