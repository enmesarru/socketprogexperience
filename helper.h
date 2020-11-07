#ifndef __HELPER_H_
#define __HELPER_H_
#include <memory>
#include <netdb.h>

namespace network_helper
{
    struct addrinfo_deleter
    {
        void operator()(addrinfo* ai) 
        {
            if(ai) freeaddrinfo(ai); 
        } 
    };

    using addrinfo_pointer = std::unique_ptr<addrinfo, addrinfo_deleter>;
    
} // namespace network_h

#endif