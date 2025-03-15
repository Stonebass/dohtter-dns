#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <boost/asio.hpp>
#include <vector>
#include <string>

using boost::asio::ip::udp;

void HandleRequest(const std::shared_ptr<boost::asio::io_service> &io_service,
                   udp::socket &socket, const udp::endpoint &clientEndpoint,
                   std::vector<uint8_t> requestBuffer, const std::string &dohUrl);

#endif // REQUEST_HANDLER_H
