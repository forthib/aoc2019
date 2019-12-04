add_library (Password password.cpp password.h)
add_executable (Day4 main.cpp)
target_link_libraries (Day4 Password)