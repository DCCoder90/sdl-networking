#include "NetHandler.h"

int WinMain(int argc, char* argv[]) {
    NetHandler* netHandler = new NetHandler();
    netHandler->Start();
    return 0;
}
