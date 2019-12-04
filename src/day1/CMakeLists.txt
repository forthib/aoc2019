add_library (Fuel fuel.cpp fuel.h)
add_executable (Day1 main.cpp)
target_link_libraries (Day1 Fuel)