project(labDB)
cmake_minimum_required(VERSION 3.9.0)

#add_definitions( "-w" )

file(GLOB CS "*.c")

add_executable(${PROJECT_NAME} ${CS})