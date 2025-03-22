cmake_minimum_required(VERSION 3.10)
project(ECG_MITBIH_Sequential)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find and link OpenMP
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message(STATUS "Found OpenMP")
endif()

add_executable(ecg_app src/main.cpp src/ecg_processor.cpp)

if(OpenMP_CXX_FOUND)
    target_link_libraries(ecg_app PUBLIC OpenMP::OpenMP_CXX)
endif()

# Link with experimental filesystem library
target_link_libraries(ecg_app PUBLIC stdc++fs)