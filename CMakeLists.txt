cmake_minimum_required(VERSION 3.16)
project(TicketSystem)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "-fmax-errors=10 -O2 -DONLINE_JUDGE -lm -std=c++17")
add_compile_options(-fmax-errors=10 -O2 -DONLINE_JUDGE -lm -std=c++17)

include_directories(src)

add_executable(TicketSystem src/BPlusTree.cpp)

