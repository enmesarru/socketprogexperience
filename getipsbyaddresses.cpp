#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <memory>
#include <cstring>
#include <array>

struct addrinfodeleter {
  void operator()(addrinfo *ai) { if (ai) freeaddrinfo(ai); }
};

using s_addrinfo = std::unique_ptr<addrinfo, addrinfodeleter>;

int main(int argc, char *argv[]) {
    int status;
    addrinfo hints;
    addrinfo *res;
    std::array<char, INET6_ADDRSTRLEN> ipstr;

    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(argv[1], nullptr, &hints, &res);
    s_addrinfo result_pointer(res);

    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    std::cout << "IP Addresses for -> " << argv[1] << '\n';

    while (res != nullptr)
    {
        if(res->ai_family == AF_INET) {
            in_addr& ina = reinterpret_cast<sockaddr_in*>(res->ai_addr)->sin_addr;
            inet_ntop(res->ai_family, &ina.s_addr, ipstr.data(), sizeof(ipstr));
            std::cout << "Family: IPV4, Ip Addresses => " << ipstr.data() << '\n';
        }
        else if(res->ai_family == AF_INET6)
        {
            in6_addr& ina6 = reinterpret_cast<sockaddr_in6*>(res->ai_addr)->sin6_addr;
            inet_ntop(res->ai_family, &ina6, ipstr.data(), sizeof(ipstr));
            std::cout << "Family: IPV6, Ip Addresses => " << ipstr.data() << '\n';
        }
        res = res->ai_next;
    }
    
}
