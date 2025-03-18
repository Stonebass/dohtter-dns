#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <curl/curl.h>

using boost::asio::ip::udp;

unsigned short local_port = 5054;                             // Default port
std::string doh_url = "https://cloudflare-dns.com/dns-query"; // Default DoH URL

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

bool load_config()
{
    std::ifstream config_file;

    // Check for ./config first
    config_file.open("./config");
    if (!config_file.is_open())
    {
        // Fallback to /etc/dohtter_dns/config
        config_file.open("/etc/dohtter_dns/config");
    }

    if (!config_file.is_open())
    {
        std::cerr << "No configuration file found. Using default values." << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(config_file, line))
    {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value))
        {
            if (key == "port")
            {
                local_port = static_cast<unsigned short>(std::stoi(value));
            }
            else if (key == "doh_url")
            {
                doh_url = value;
            }
        }
    }
    config_file.close();
    return true;
}

std::string forward_to_doh(const std::vector<unsigned char> &dns_request)
{
    CURL *curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl)
    {
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/dns-message");

        curl_easy_setopt(curl, CURLOPT_URL, doh_url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dns_request.data());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, dns_request.size());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    return response;
}

int main()
{
    load_config(); // Load configuration from file

    try
    {
        boost::asio::io_context io_context;
        udp::socket socket(io_context, udp::endpoint(udp::v4(), local_port));
        std::cout << "Listening on port " << local_port << " for DNS requests..." << std::endl;

        while (true)
        {
            std::vector<unsigned char> data(512);
            udp::endpoint sender_endpoint;
            size_t length = socket.receive_from(boost::asio::buffer(data), sender_endpoint);

            data.resize(length); // Resize to actual length of the received packet
            std::cout << "Received DNS request from " << sender_endpoint << std::endl;

            std::string doh_response = forward_to_doh(data);
            if (!doh_response.empty())
            {
                socket.send_to(boost::asio::buffer(doh_response), sender_endpoint);
                std::cout << "Response sent back to client." << std::endl;
            }
            else
            {
                std::cerr << "Failed to get a response from the DoH server." << std::endl;
            }
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
