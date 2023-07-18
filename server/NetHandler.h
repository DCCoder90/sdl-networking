#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <SDL_net.h>
#include <string>
#include <sstream>
#include <numeric>

class NetHandler {
public:
    void Start();
    NetHandler();
    ~NetHandler();
private:
    TCPsocket serverSocket;
    IPaddress ip;
    std::vector<TCPsocket> clientSockets;
    static void handleClient(TCPsocket clientSocket);
    static bool stillRunning;
};
