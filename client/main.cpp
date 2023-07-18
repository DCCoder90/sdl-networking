#include <iostream>
#include <SDL_net.h>
#include <thread>

void handleServer(TCPsocket serverSocket){
    // Receive a response from the server
    char buffer[512];
    while(true) {
        int bytesRead = SDLNet_TCP_Recv(serverSocket, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            std::cerr << "Error or connection closed by server" << std::endl;
            break;
        } else {
            buffer[bytesRead] = '\0'; // Null-terminate the received data
            std::cout << "Received response from server: " << buffer << std::endl;
        }
    }
}

int WinMain(int argc, char* argv[]) {
    // Initialize SDL_net
    if (SDLNet_Init() != 0) {
        std::cerr << "Failed to initialize SDL_net: " << SDLNet_GetError() << std::endl;
        return 1;
    }

    // Resolve the server host
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, "localhost", 1234) == -1) {
        std::cerr << "Failed to resolve host: " << SDLNet_GetError() << std::endl;
        return 1;
    }

    // Open a TCP connection to the server
    TCPsocket clientSocket = SDLNet_TCP_Open(&ip);
    if (!clientSocket) {
        std::cerr << "Failed to open socket: " << SDLNet_GetError() << std::endl;
        return 1;
    }



    // Create a new thread to handle the client connection
    std::thread clientThread(handleServer, clientSocket);
    clientThread.detach(); // Detach the thread and let it run independently

    while(true) {
        std::string str;
        std::cout << "\nEnter Message:";
        std::getline(std::cin, str);

        // Send a message to the server
        int bytesSent = SDLNet_TCP_Send(clientSocket, str.c_str(), str.length() + 1);
        if (bytesSent < str.length() + 1) {
            std::cerr << "Failed to send message: " << SDLNet_GetError() << std::endl;
        }
    }

    // Close the client socket
    SDLNet_TCP_Close(clientSocket);

    // Quit SDL_net
    SDLNet_Quit();

    return 0;
}