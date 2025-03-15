#ifndef DNS_SERVER_H
#define DNS_SERVER_H

#include <string>

void RunServer(const std::string &upstreamDnsServer, uint16_t port);

#endif // DNS_SERVER_H
