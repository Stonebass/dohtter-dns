#ifndef DOH_CLIENT_H
#define DOH_CLIENT_H

#include <string>
#include <vector>
#include <curl/curl.h>

class DnsOverHttpsClient
{
public:
    DnsOverHttpsClient(const std::string &dohUrl);
    ~DnsOverHttpsClient();

    std::string ForwardToDoH(const std::vector<uint8_t> &dnsQuery);

private:
    std::string dohUrl_;
    CURL *curl_;
    struct curl_slist *headers_ = nullptr;
};

#endif // DOH_CLIENT_H
