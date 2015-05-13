# DarkNet
Simple multithreaded networking library for win32. Provides basic functionality for broadcasting and recieving messages.

Multithreading property can be controlled while initailising the library.

It uses callbacks instead of polling to get network data. Simply provide a function pointer when initializing the library and that 
function will be called each time the client recieves any data from network.

You can create message formats yourself.
