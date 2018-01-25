# CSE 5462 Lab2

# Description
A file transfer application using TCP STREAM oriented sockets.

[Protocol]

Client will send following info to the server

* file size: 4 bytes
* file name: 20 bytes
* data

After getting the file, server will send a 4-byte response to client indicating how many bytes were actually sent

# Usage
To compile

> $ make

To run server

> $ ./server [port]

To send files at client side

> $ ./client [server ip] [sever port] [filepath to be sent]
