add_library (OpCode opcode.cpp opcode.h)
add_executable (Day2 main.cpp)
target_link_libraries (Day2 OpCode)