cmake_minimum_required(VERSION 3.10)

# set the project name
project(LoadCellAmp)
find_package(Catch2 REQUIRED)

enable_testing()

add_subdirectory(src/common)
# 
if("${CMAKE_CXX_COMPILER}" MATCHES "xtensa")
  set(CMAKE_TARGET_PLATFORM "espidf")
else()
  set(CMAKE_TARGET_PLATFORM "local_tests")
endif()

# 
add_subdirectory(src/arch/${CMAKE_TARGET_PLATFORM})

# add_subdirectory(src)


# add the executable
# add_executable(LoadCellAmp test/testmain.cpp)
