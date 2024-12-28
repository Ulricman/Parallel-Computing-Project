cmake_minimum_required(VERSION 3.28.3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(Parallel-Computing-Project LANGUAGES CXX)

file(GLOB ProjectSources CONFIGURE_DEPENDS "src/*.cpp")

find_package(GTest REQUIRED)
enable_testing()
add_subdirectory(tests)

add_executable(
    main
    main.cpp ${ProjectSources}
)
target_include_directories(main PUBLIC ${CMAKE_SOURCE_DIR}/include)