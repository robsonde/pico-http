# Raspberry Pi Pico 2 W - HTTP example

This project is a simple example of an HTTP client for the Pi Pico 2 W.
It is not expected that this would be directly used in a larger project, but it is intended to give clear code that can be copied or modified to meet the need of other projects.

## What does this project do?

Overall this project does very little, it connects wifi, then requests a single file over http and returns it to a given buffer.

## What problem am I solving?

Many of the examples of `httpc_get_file_dns` are either limited in functionality, overlly complex, or broken.
- Many examples have a memory leak.
- Many exaples don't return data to a buffer, they just print it as it comes in.
- Some examples are integrated into a large project and make the HTTP code hard to see and understand.
- The lwip documentation is very dense and technical, lacking a copy and paste example.

## How do I use it?
Assuming you have done a git clone of the repo, and assuming a linux command line environment.
Update example.c to have your correct wifi network name and password.
```console
foo@bar:~/pico-http$ mkdir build
foo@bar:~/pico-http$ cd build
foo@bar:~/pico-http/build$ cmake ..
<ASSORTED OUTPUT OF CMAKE>
foo@bar:~/pico-http/build$ make
<ASSORTED OUTPUT OF MAKE>
```
This will give you a file of example.uf2 which you can copy to a Pi Pico 2 W.

## What's wrong with other examples?
**Only one packet.**
Most examples are missing a line in the `recv_fn` that is needed to loop over each packet, this is done with:
```c
altcp_recved(conn, p->tot_len);
```
**Memory leaks.**
Several examples are missing a line in the `recv_fn` which leads to a memory leak.  The pointer `p` needs to be freed.
```c
pbuf_free(p);
```
**Lack of a buffer.**
Most examples simply use printf() to output the data as it arrives, not copying the data to a buffer.
```c
int Length = pbuf_copy_partial(p, HttpBuff+Http_Recv, p->tot_len, 0);
Http_Recv = Http_Recv + Length;
``` 

## Contributions welcome

I am new to both C code and Pi Pico hardware, I expect to have errors and mistakes, I welcome comments and feedback from people. (AI not welcome) 

This whole project is provided as is and free to the world, no copyright or license.


