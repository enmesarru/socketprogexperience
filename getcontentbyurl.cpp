#include <iostream>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>
#include "helper.h"

int main(int argc, char *argv[]) {
    addrinfo hints;
    addrinfo* result = nullptr;
    addrinfo* p = nullptr;
    int _socket;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(argv[1], argv[2], &hints, &result);
    network_helper::addrinfo_pointer result_p(result);

    if(status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << '\n';
        return 1;
    }

    for(p = result; p != nullptr; p = p->ai_next) {
        _socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(_socket == -1) {
            std::cout << "socket not connected \n";
            continue;
        }

        if(connect(_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(_socket); // close it if no longer refers to any file
            continue;
        }
        break;
    }

    if(result == nullptr) {
        std::cerr << "failed to connect\n";
        return 1;
    }

    result_p.reset();
	pollfd fds[2];

	fds[0].fd = 0;
	fds[0].events = POLLIN;
	fds[1].fd = _socket;
	fds[1].events = POLLIN; /* set events poll for incoming data */
    int timeout = -1; // block until event occurs
	// Main loop
	for(;;) {
		if (poll(fds, 2, timeout) == -1) { // wait an event
			std::cout << "poll";
			exit(1);
		}
        for (int i = 0; i < 2; i++) {
			if (fds[i].revents & POLLIN) {
                std::array<char, 1024> buffer;
                int readbytes, writebytes;

                /*  if input event comes from client(terminal 
                    for example GET / ) then
                    output file descriptior will be socket,
                    if it is not then output fd will be terminal  
                */
                int outfd = fds[i].fd == 0 ? _socket: 1;

                if ((readbytes = read(fds[i].fd, buffer.data(), 1024)) == -1) {
					std::cerr << ("read");
					exit(2);
				}

                int tempbytes = readbytes;
                char* p = buffer.data();
                while(tempbytes > 0) {
					if ((writebytes = write(outfd, p, tempbytes)) == -1) {
						std::cerr << ("write");
						exit(2);
					}

                    p += writebytes;
                    tempbytes -= writebytes;
                }
            }
        }

    }

}