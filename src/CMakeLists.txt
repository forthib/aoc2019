include_directories("C:\\TFO\\CIVA_DK\\CIVA_DK_64_dev_003\\outilsExternes\\Boost\\boost_1_64_0a")
include_directories("C:\\TFO\\CIVA_DK\\CIVA_DK_64_dev_003\\outilsExternes\\tbb_2018_1\\include")
link_directories("C:\\TFO\\CIVA_DK\\CIVA_DK_64_dev_003\\outilsExternes\\tbb_2018_1\\lib\\intel64")

add_library (IO io.cpp io.h)
add_library (Fuel fuel.cpp fuel.h)
add_library (OpCode opcode.cpp opcode.h)
add_library (Intersection intersection.cpp intersection.h)
add_library (Password password.cpp password.h)
add_library (Orbits orbits.cpp orbits.h)
add_library (Image image.cpp image.h)

add_executable (Day1 day1.cpp)
target_link_libraries (Day1 Fuel IO)

add_executable (Day2 day2.cpp)
target_link_libraries (Day2 OpCode IO)

add_executable (Day3 day3.cpp)
target_link_libraries (Day3 Intersection IO)

add_executable (Day4 day4.cpp)
target_link_libraries (Day4 Password)

add_executable (Day5 day5.cpp)
target_link_libraries (Day5 OpCode IO)

add_executable (Day6 day6.cpp)
target_link_libraries (Day6 Orbits IO)

add_executable (Day7 day7.cpp)
target_link_libraries (Day7 OpCode IO)

add_executable (Day8 day8.cpp)
target_link_libraries (Day8 Image IO)