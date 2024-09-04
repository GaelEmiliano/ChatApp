# ChatApp

## Client-Server Chat Application in C++

This project is an implementation of a client-server architecture for a chat system in C++, using sockets
for communication between multiple clients and a central server. The server handles concurrent connections
using threads, allowing multiple clients to communicate with each other by sending messages through the server.
The communication between clients and the server is done using JSON format, and users can join different 
**chat-rooms** to communicate with other users in the same room.

## Project Description

This application provides continuous communication between multiple clients (potentially on different computers)
through a central server. The server is **concurrent**, meaning it accepts multiple connections simultaneously
and delegates each connection to a separate thread. This allows the server to always be available to accept new
connections while individual threads handle communication with each client.
