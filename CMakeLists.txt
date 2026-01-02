cmake_minimum_required(VERSION 3.10)

project(rayout LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set(CMAKE_CXX_FLAGS_PROFILE "-O3 -g -pg -march=native")
set(CMAKE_EXE_LINKER_FLAGS_PROFILE "-pg")

add_executable(rayfloat src/main.cpp)
target_include_directories(rayfloat PRIVATE include)

if(OpenMP_CXX_FOUND)
    target_link_libraries(rayfloat PUBLIC OpenMP::OpenMP_CXX)
endif()

add_custom_target(analyze
    COMMAND ./rayfloat > /dev/null
    COMMAND gprof ./rayfloat gmon.out > analysis.txt
    DEPENDS rayfloat
    COMMENT "Running rayfloat and generating gprof analysis..."
)