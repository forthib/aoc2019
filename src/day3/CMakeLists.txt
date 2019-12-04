add_library (Intersection intersection.cpp intersection.h)
add_executable (Day3 main.cpp)
target_link_libraries (Day3 Intersection)