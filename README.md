# Distributed Systems Project - University Project

The overall goal of the first project will be to realize a key-value pair storage service similar to that used by Amazon to support its web services. In this sense, the data structures used to store this information are a simple linked list and a hash table, given their high search efficiency.

----------------------------------
## Part 1

In distributed systems it is necessary to exchange information via the network. This information must be sent / received correctly and can represent any data structure. In this sense, it is necessary to develop serialization / deserialization mechanisms to be used for sending / receiving information:
* So in project 1 the objective is to implement data structures (the hash table structure with simple linked list) and various functions to handle the manipulation of the data, to be stored in the table, as well as to manage a local hash table that supports a subset of the services defined by the hash table.
* It should also be implemented the functions necessary to serialize (encode) and de-serialize (decode) data structures in byte arrays (to be transmitted on the network).

NOTE: For more information PDF can be found under fase1 with the name [`sd_proj1`](part1/sd_proj1.pdf)

----------------------------------
## Part 2

The goal is to provide applications that would use this table with a Remote Procedure Call (RPC) communication model, where multiple clients access the same shared hash table ona at a time.

* This part aims to implement the functions necessary to serialize (code) and de-serialize (decode) using the [*Google Protocol Buffers*](https://github.com/protobuf-c/protobuf-c).
* Realize the hash table on a server.
* Implement a client with an interface similar to what was made in Project 1 for the table (defined by the table.h file), that is, functions that interact with the table in memory. This implies:
    1. through the client, the user will invoke operations, which will be transformed into messages and sent over the network to the server;
    2. The server will interpret these messages, and: 
        * will perform the corresponding operations on the local table;
        * will then send the transformed response to the customer;
    3. The client will interpret the reply message, and proceed according to the result, and then be ready for the next operation.
* High server reliability is expected, so there can be no unchecked error conditions or inefficient memory management to prevent it from crashing, which would leave all clients without the shared table

NOTE: For more information PDF can be found under fase2 with the name [`sd_proj2`](part2/sd_proj2.pdf)

----------------------------------
