#include "RequestHandler.h"
#include "DoHClient.h"
#include <iostream>

void HandleRequest(const std::shared_ptr<boost::asio::io_service> &io_service, udp::socket &socket,
                   const udp::endpoint &clientEndpoint, std::vector<uint8_t> requestBuffer, const std::string &dohUrl)
{
    DnsOverHttpsClient dohClient(dohUrl);

    uint16_t dnsId = (requestBuffer[0] << 8) | requestBuffer[1];
    std::cout << "Processing DNS request with ID: " << dnsId << std::endl;

    std::string dohResponse = dohClient.ForwardToDoH(requestBuffer);
    if (!dohResponse.empty())
    {
        std::vector<uint8_t> responseBuffer(dohResponse.begin(), dohResponse.end());
        responseBuffer[0] = (dnsId >> 8) & 0xFF;
        responseBuffer[1] = dnsId & 0xFF;

        socket.async_send_to(boost::asio::buffer(responseBuffer), clientEndpoint,
                             [io_service](boost::system::error_code, std::size_t) {});
    }
    else
    {
        std::cerr << "Failed to get response from DoH server." << std::endl;
    }
}
