### CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(ECG_MITBIH_Sequential)

set(CMAKE_CXX_STANDARD 11)

# Add the executable; include all source files
add_executable(ecg_app src/main.cpp src/ecg_processor.cpp)
