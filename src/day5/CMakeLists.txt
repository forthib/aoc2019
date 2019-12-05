add_library (OpCode2 opcode2.cpp opcode2.h)
add_executable (Day5 main.cpp)
target_link_libraries (Day5 OpCode2)