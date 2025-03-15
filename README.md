# Dohtter DNS

A DNS-over-HTTPS (DoH) forwarder. It's intended use is mainly to provide DoH for Pi-hole setups.

---

## Requirements

Install the following dependencies on your system:
- **C++ Compiler** (C++17 compatible, e.g., `g++` or `clang`).
- **Boost Libraries**.
- **cURL Library**.

```sudo apt-get install g++ libboost-all-dev libcurl4-openssl-dev```

## Build and install

Clone the repository

```git clone git@github.com:Stonebass/dohtter-dns.git```

Run 
```make```
and then 
```sudo make install```

## Configuration

Make copies the sample config which starts the service on port 5053 and uses google as the upstream DoH server. To change that, edit /etc/dohtter_dns/config and then restart the service.

## License

This project is licensed under the MIT License. See [LICENSE](./LICENSE) for details.

### Third-Party Licenses
This application uses the following third-party libraries, which are licensed separately:
- Boost C++ Libraries, licensed under the Boost Software License 1.0 ([LICENSE_BOOST.txt](./LICENSE_BOOST.txt)).
- cURL, licensed under the cURL license ([LICENSE_CURL.txt](./LICENSE_CURL.txt)).
