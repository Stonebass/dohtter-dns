# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Target executable
TARGET = build/dohtter_dns

# Source and header files
SRC = src/main.cpp src/ConfigManager.cpp src/DoHClient.cpp src/RequestHandler.cpp src/DnsServer.cpp
HEADERS = src/ConfigManager.h src/DoHClient.h src/RequestHandler.h src/DnsServer.h

# Object files in the build directory
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))

# Libraries
LIBS = -lcurl -lboost_system

# Installation directories
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
CONFIGDIR = /etc/dohtter_dns

# Create the build directory if it doesn't exist
$(shell mkdir -p build)

# Build rules
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LIBS)

build/%.o: src/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Install rules
.PHONY: install
install: $(TARGET)
	mkdir -p $(BINDIR)
	cp $(TARGET) $(BINDIR)
	mkdir -p $(CONFIGDIR)
	echo "https://dns.google/dns-query\n5053" > $(CONFIGDIR)/config
	cp dohtter_dns.service /etc/systemd/system/
	systemctl daemon-reload
	systemctl enable dohtter_dns.service
	systemctl start dohtter_dns.service

# Clean up
.PHONY: clean
clean:
	rm -rf build

.PHONY: uninstall
uninstall:
	rm -f $(BINDIR)/dohtter_dns
	rm -rf $(CONFIGDIR)
	systemctl stop dohtter_dns.service
	systemctl disable dohtter_dns.service
	rm -f /etc/systemd/system/dohtter_dns.service
	systemctl daemon-reload
