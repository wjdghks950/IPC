# Message Passing between Threads in C (POSIX)


## Prerequisites

Installation of gcc compiler.

## Builds

$ gcc -o a.out msg_passIPC.c -pthread

The "-pthread" option allows the compilation of POSIX pthread functions

## Execution

$ ./a.out [send_key] [receive_key]

You must specify the send_key and receive_key. This will be the keys for the 
send and receive message queues.

*Make sure to use a unique sequence of numbers on a shared system.
