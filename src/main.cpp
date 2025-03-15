#include "ConfigManager.h"
#include "DnsServer.h"
#include <iostream>

int main()
{
    std::string upstreamDnsServer;
    uint16_t port;

    try
    {
        auto config = LoadConfig();
        upstreamDnsServer = config.first;
        port = config.second;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    RunServer(upstreamDnsServer, port);
    return 0;
}
