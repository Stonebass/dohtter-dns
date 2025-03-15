#include "DnsServer.h"
#include "RequestHandler.h"
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <thread>
#include <vector>
#include <iostream>

using boost::asio::ip::udp;

void RunServer(const std::string &upstreamDnsServer, uint16_t port)
{
    auto io_service = std::make_shared<boost::asio::io_service>();
    boost::asio::io_service::work work(*io_service);

    udp::socket socket(*io_service, udp::endpoint(udp::v4(), port));
    boost::asio::signal_set signals(*io_service, SIGINT, SIGTERM);

    signals.async_wait([&](const boost::system::error_code &, int)
                       {
        std::cout << "Shutting down server..." << std::endl;
        socket.close();
        io_service->stop(); });

    std::cout << "Listening for DNS requests on port " << port << "..." << std::endl;

    auto receiveBuffer = std::make_shared<std::vector<uint8_t>>(512);
    auto clientEndpoint = std::make_shared<udp::endpoint>();

    std::function<void()> receiveHandler = [&]()
    {
        socket.async_receive_from(boost::asio::buffer(*receiveBuffer), *clientEndpoint,
                                  [&, receiveBuffer, clientEndpoint](boost::system::error_code ec, std::size_t length)
                                  {
                                      if (!ec && length > 0)
                                      {
                                          receiveBuffer->resize(length);
                                          HandleRequest(io_service, socket, *clientEndpoint, *receiveBuffer, upstreamDnsServer);
                                          receiveHandler(); // Continue listening for requests
                                      }
                                  });
    };

    receiveHandler();

    std::vector<std::thread> threadPool;
    const size_t threadCount = std::thread::hardware_concurrency();
    for (size_t i = 0; i < threadCount; ++i)
    {
        threadPool.emplace_back([io_service]()
                                { io_service->run(); });
    }

    for (auto &thread : threadPool)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}
