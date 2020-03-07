# Similar Amazon Storage Service (Client-Server)
(Distributed Systems Project - University Project)

The overall goal of the project will be to realize a key-value pair storage service similar to that used by Amazon to support its web services. In this sense, the data structures used to store this information are a simple linked list and a hash table, given their high search efficiency. Will support multiple clients and fault tolerance.

----------------------------------
## To Run The Project - Running PartX
* Go to the project part do you wanna run and run the command ```mkdir binary object lib``` to create the essential folders to build the project successfully 

* Then you can run ```make``` to build

* Finally you can run the binary:
    * Part1: You can run the default tests running ```make test```
    * Part2/3:
        * Server: ```table-server <port_number> <num_listsOfTable>```
        * Client: ```table-client <hostname_server>:<port_number>```
    * Part4: 
        * Server: ```table-server <port_number> <num_listsOfTable> <IPZookeeper>:<port_number_Zookeeper>```
        * Client: ```table-client <hostname_server>:<port_number>```

* Note: you can do ```make clean``` to clean the object and binary files (it cleans the lib's files too). More info in each README.md inside each part of the project.


----------------------------------
## Part 1

In distributed systems it is necessary to exchange information via the network. This information must be sent / received correctly and can represent any data structure. In this sense, it is necessary to develop serialization / deserialization mechanisms to be used for sending / receiving information:
* So in project 1 the objective is to implement data structures (the hash table structure with simple linked list) and various functions to handle the manipulation of the data, to be stored in the table, as well as to manage a local hash table that supports a subset of the services defined by the hash table.
* It should also be implemented the functions necessary to serialize (encode) and de-serialize (decode) data structures in byte arrays (to be transmitted on the network).

NOTE: For more information a PDF can be found under fase3 with the name [`sd_proj1`](part1/sd_proj1.pdf)

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

NOTE: For more information a PDF can be found under fase3 with the name [`sd_proj2`](part2/sd_proj2.pdf)

----------------------------------
## Part 3
This part aims to implement a concurrent system that accepts orders from multiple clients simultaneously through the use of I/O multiplexing and separates I/O treatment from data processing through the use of Threads:
* Adapt the server so that it supports requests from multiple clients connected simultaneously, which will be done through I/O multiplexing (using a system call: poll()).
* Adapt the server to provide asynchronous responses to customers' written requests. This implies:
	* Implement a verify operation, which takes the identifier of an operation as an argument and checks whether it has already been performed.
	* Implement a Task Queue (Producer-Consumer) where writing requests are kept until they are executed.
	*Adapt the server to have two threads: the main thread of the program, which is responsible for multiplexing new connections and customer requests, for responding to reading requests, and for inserting writing requests in the Task Queue; and a secondary thread, launched by the main thread, which removes the operations to be executed from the Task Queue and executes them.
	* Ensure thread synchronization when accessing the Task Table and Queue through the use of Locks and Conditional Variables

NOTE: For more information a PDF can be found under fase3 with the name [`sd_proj3`](part3/sd_proj3.pdf)

----------------------------------
## Part 4
This part aims to implement support fault tolerance through server state replication, following the Chain Replication model[1] and using the [*ZooKeeper*](https://zookeeper.apache.org/) coordination service:
* Implement server coordination in ZooKeeper to support the Chain Replication Model:
* Change server operation to:
	* Ask ZooKeeper what is the next server after this one in the replication chain.
	* After performing a write operation, send it to the next server in order to propagate replication.
	* Watch ZooKeeper in order to be notified of changes in the chain and connect to its new server, if it has changed.
* Change client operation to:
	* Ask ZooKeeper what servers are at the head and tail of the chain.
	* Send write operations to the server at the head of the chain.
	* Send read operations to the server at the tail of the chain.
	* Make watch on ZooKeeper in order to be notified of changes in the chain and connect to the new head and tail, if they have changed.

NOTE: For more information a PDF can be found under fase3 with the name [`sd_proj4`](part4/sd_proj4.pdf)

Bibliography:
[1] (R. V. Renesse and F. B. Schneider. Chain Replication for Supporting High Throughput and Availability. OSDI. Vol. 4. No. 91–104. 2004)