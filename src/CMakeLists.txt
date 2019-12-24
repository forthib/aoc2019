include_directories("C:\\TFO\\CIVA_DK\\CIVA_DK_64_dev_003\\outilsExternes\\Boost\\boost_1_64_0a")
include_directories("C:\\TFO\\CIVA_DK\\CIVA_DK_64_dev_003\\outilsExternes\\tbb_2018_1\\include")
link_directories("C:\\TFO\\CIVA_DK\\CIVA_DK_64_dev_003\\outilsExternes\\tbb_2018_1\\lib\\intel64")

add_library (Test test.cpp test.h)
add_library (IO io.cpp io.h)
add_library (Fuel fuel.cpp fuel.h)
add_library (OpCode opcode.cpp opcode.h)
add_library (Intersection intersection.cpp intersection.h)
add_library (Password password.cpp password.h)
add_library (Orbits orbits.cpp orbits.h)
add_library (Image image.cpp image.h)
add_library (Asteroid asteroid.cpp asteroid.h)
target_link_libraries (Asteroid IO Image)
add_library (Robot robot.cpp robot.h)
add_library (Moon moon.cpp moon.h)
target_link_libraries (Moon IO)
add_library (Reaction reaction.cpp reaction.h)
target_link_libraries (Reaction IO)

add_executable (Day1 day1.cpp)
target_link_libraries (Day1 Fuel IO Test)

add_executable (Day2 day2.cpp)
target_link_libraries (Day2 OpCode IO Test)

add_executable (Day3 day3.cpp)
target_link_libraries (Day3 Intersection IO Test)

add_executable (Day4 day4.cpp)
target_link_libraries (Day4 Password Test)

add_executable (Day5 day5.cpp)
target_link_libraries (Day5 OpCode IO Test)

add_executable (Day6 day6.cpp)
target_link_libraries (Day6 Orbits IO Test)

add_executable (Day7 day7.cpp)
target_link_libraries (Day7 OpCode IO Test)

add_executable (Day8 day8.cpp)
target_link_libraries (Day8 Image IO Test)

add_executable (Day9 day9.cpp)
target_link_libraries (Day9 OpCode IO Test)

add_executable (Day10 day10.cpp)
target_link_libraries (Day10 Asteroid Test)

add_executable (Day11 day11.cpp)
target_link_libraries (Day11 Robot OpCode Image IO Test)

add_executable (Day12 day12.cpp)
target_link_libraries (Day12 Moon IO Test)

add_executable (Day13 day13.cpp)
target_link_libraries (Day13 OpCode Image IO Test)

add_executable (Day14 day14.cpp)
target_link_libraries (Day14 Reaction Test)

add_executable (Day15 day15.cpp)
target_link_libraries (Day15 Robot OpCode Image IO)

add_executable (Day16 day16.cpp)
target_link_libraries (Day16 IO Test)

add_executable (Day17 day17.cpp)
target_link_libraries (Day17 OpCode IO Image Test)

add_executable (Day18 day18.cpp)
target_link_libraries (Day18 IO Image Test)

add_executable (Day19 day19.cpp)
target_link_libraries (Day19 OpCode IO Image Test)

add_executable (Day20 day20.cpp)
target_link_libraries (Day20 IO Image Test)

add_executable (Day21 day21.cpp)
target_link_libraries (Day21 OpCode IO Image Test)

add_executable (Day22 day22.cpp)
target_link_libraries (Day22 IO Test)

add_executable (Day23 day23.cpp)
target_link_libraries (Day23 OpCode IO Image Test)

add_executable (Day24 day24.cpp)
target_link_libraries (Day24 IO Image Test)