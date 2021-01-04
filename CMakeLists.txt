cmake_minimum_required(VERSION 3.10)

# set the project name
project(LoadCellAmp)
find_package(Catch2 REQUIRED)

enable_testing()

add_subdirectory(tests)
# add_subdirectory(src)


# add the executable
# add_executable(LoadCellAmp test/testmain.cpp)
