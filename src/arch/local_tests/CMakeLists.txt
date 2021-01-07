add_executable(tests testmain.cpp loadCellAmpTests.cpp)
target_include_directories(tests PUBLIC ./)
target_link_libraries(tests PUBLIC loadCellAmpCommon PRIVATE Catch2::Catch2WithMain)
