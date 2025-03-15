#include "DoHClient.h"
#include <iostream>
#include <stdexcept>

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

DnsOverHttpsClient::DnsOverHttpsClient(const std::string &dohUrl) : dohUrl_(dohUrl), curl_(curl_easy_init())
{
    if (!curl_)
    {
        throw std::runtime_error("Failed to initialize CURL");
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    headers_ = curl_slist_append(headers_, "Content-Type: application/dns-message");
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_);
    curl_easy_setopt(curl_, CURLOPT_URL, dohUrl.c_str());
    curl_easy_setopt(curl_, CURLOPT_POST, 1L);
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
}

DnsOverHttpsClient::~DnsOverHttpsClient()
{
    curl_easy_cleanup(curl_);
    curl_slist_free_all(headers_);
    curl_global_cleanup();
}

std::string DnsOverHttpsClient::ForwardToDoH(const std::vector<uint8_t> &dnsQuery)
{
    std::string response;
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, dnsQuery.data());
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, dnsQuery.size());
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl_);
    if (res != CURLE_OK)
    {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
    }
    return response;
}
