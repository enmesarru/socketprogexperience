#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include "helper.h"

int main(int argc, char const *argv[])
{
    constexpr char port[] = "3490";
    constexpr int backlog = 10;


    sockaddr_storage their_addr;
    socklen_t addr_size;
    addrinfo hints, *res;
    int sockfd, new_fd;

    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me

    int status = getaddrinfo(nullptr, port, &hints, &res);
    
    if(status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << '\n';
        return 1;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if(sockfd == -1) {
        std::cout << "socket not connected \n";
        return 2;
    }
    
    if(bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        std::cout << "bind err \n";
        return 3;
    }

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    if(listen(sockfd, 10) == -1){
        std::cout << "listen err \n";
        return 3;
    }

    while(true) {
        std::array<char, NI_MAXHOST> host_buff;
        std::array<char, NI_MAXSERV> server_buff;
        addr_size = sizeof(their_addr);
        new_fd = accept(sockfd, (sockaddr* )&their_addr, &addr_size);

        getnameinfo(
            (sockaddr*)&their_addr, addr_size, host_buff.data(),  
            sizeof(host_buff), server_buff.data(), sizeof(server_buff), 
            NI_NUMERICHOST | NI_NUMERICSERV
        );
        std::cout << server_buff.data() << host_buff.data() << '\n';
        if(new_fd == -1) {
            std::cout << "not accepted \n";
            return 4;
        }

        const std::string message = "Test message!";
        int bytes_sent = send(new_fd, host_buff.data(), host_buff.size(), 0);
        close(new_fd);
    }

}
