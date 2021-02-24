cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(Simplex)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:/QtMSVC15/5.15.2/msvc2015_64")
find_package(Qt5Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(Simplex main.cpp SimplexMethod.cpp SimplexMethod.h LP.h LP.cpp Common.h LPInput.cpp LPInput.h lpinput.ui)
target_link_libraries(Simplex Qt5::Widgets)