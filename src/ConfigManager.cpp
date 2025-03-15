#include "ConfigManager.h"
#include <fstream>
#include <stdexcept>

std::pair<std::string, uint16_t> LoadConfig()
{
    std::ifstream configFile;
    if (std::ifstream("./config").good())
    {
        configFile.open("./config");
    }
    else if (std::ifstream("/etc/dohtter_dns/config").good())
    {
        configFile.open("/etc/dohtter_dns/config");
    }
    else
    {
        throw std::runtime_error("No configuration file found.");
    }

    std::string upstreamDnsServer;
    uint16_t port;
    if (configFile.is_open())
    {
        configFile >> upstreamDnsServer >> port;
        configFile.close();
    }
    else
    {
        throw std::runtime_error("Failed to open configuration file.");
    }

    return {upstreamDnsServer, port};
}
