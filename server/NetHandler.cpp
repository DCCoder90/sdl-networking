#include "NetHandler.h"

bool NetHandler::stillRunning = true;

NetHandler::NetHandler() {
    if (SDLNet_Init() != 0) {
        std::cerr << "Failed to initialize SDL_net: " << SDLNet_GetError() << std::endl;
    }

    if (SDLNet_ResolveHost(&ip, nullptr, 1234) == -1) {
        std::cerr << "Failed to resolve host: " << SDLNet_GetError() << std::endl;
    }

    serverSocket = SDLNet_TCP_Open(&ip);

    if (!serverSocket) {
        std::cerr << "Failed to open socket: " << SDLNet_GetError() << std::endl;
    }
}

void NetHandler::handleClient(TCPsocket clientSocket) {
    char buffer[512];

    while (true) {
        int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));

        if (bytesRead <= 0) {
            std::cerr << "Error or connection closed by client" << std::endl;
            break;
        }

        buffer[bytesRead] = '\0'; // Null-terminate the received data

        if(buffer[0] == '/'){
            std::string message(buffer);
            if(message == "/exit"){
                std::cout << "Client closed connection" << std::endl;
                break;
            }else if(message == "/shutdown"){
                std::cout << "Client triggered shutdown" << std::endl;
                stillRunning = false;
                break;
            }else{
                std::istringstream iss(message);
                std::vector<std::string> tokens;
                std::string token;
                while (std::getline(iss, token, ' ')) {
                    tokens.push_back(token);
                }

                std::string arguments = std::accumulate(
                        std::next(tokens.begin()), tokens.end(), std::string{},
                        [](const std::string& acc, const std::string& item) {
                            return acc.empty() ? item : acc + " " + item;
                        }
                );

                if(tokens[0] == "/echo"){
                    int bytesSent = SDLNet_TCP_Send(clientSocket, arguments.c_str(),
                                                    arguments.length() + 1);

                    if (bytesSent < arguments.length() + 1) {
                        std::cerr << "Failed to send response: " << SDLNet_GetError() << std::endl;
                        break;
                    }
                }
            }
        }else {
            std::cout << "Received message from client: " << buffer << std::endl;
        }
    }

    // Close the client socket
    SDLNet_TCP_Close(clientSocket);
}

void NetHandler::Start() {
    // Wait for incoming connections and handle them
    while (stillRunning) {
        TCPsocket clientSocket = SDLNet_TCP_Accept(serverSocket);

        if (!clientSocket) {
            continue; // No connection available yet, keep waiting
        }

        // Add the client socket to the vector
        clientSockets.push_back(clientSocket);

        // Create a new thread to handle the client connection
        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach(); // Detach the thread and let it run independently
    }
}


NetHandler::~NetHandler() {
    // Close all remaining open sockets before quitting
    try {
        for (int i = 0; i < clientSockets.size()-1; i++) {
            SDLNet_TCP_Close(clientSockets[i]);

        }
    }catch (const std::exception& ex) {
        std::cerr << "Exception occurred: " << ex.what() << std::endl;
    }

    SDLNet_TCP_Close(serverSocket);
    SDLNet_Quit();
}