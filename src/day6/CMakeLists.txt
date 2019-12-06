add_library (Orbits orbits.cpp orbits.h)
add_executable (Day6 main.cpp)
target_include_directories(Orbits PRIVATE "C:\\TFO\\CIVA_DK\\CIVA_DK_64_dev_003\\outilsExternes\\Boost\\boost_1_64_0a")
target_link_libraries (Day6 Orbits)