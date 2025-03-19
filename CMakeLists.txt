### CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(ECG_MITBIH_Sequential)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ecg_app src/main.cpp src/ecg_processor.cpp)
target_link_libraries(ecg_app stdc++fs)
