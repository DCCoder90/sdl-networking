cmake_minimum_required(VERSION 3.25)
project(networkingclient)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2_net CONFIG REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,SDL2_net::SDL2_net,SDL2_net::SDL2_net-static>)
