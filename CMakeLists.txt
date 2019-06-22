cmake_minimum_required(VERSION 3.13)
project(durex C)

set(CMAKE_C_STANDARD 11)

add_executable(durex srcs/main.c incs/durex.h srcs/server/server.c srcs/server/listen_client.c srcs/server/client.c)

include_directories(incs)