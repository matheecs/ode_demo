cmake_minimum_required(VERSION 3.12)
project(sphere)

set(CMAKE_CXX_STANDARD 17)

find_package(ODE REQUIRED)
include_directories(${ODE_INCLUDE_DIRS} include)
include_directories(/usr/local/opt/ncurses/include)

link_directories(/Users/zhangjixiang/Code/ode-0.16.2/build)
link_directories(/usr/local/opt/ncurses/lib)

add_executable(uvc core.cpp biped.cpp)
target_link_libraries(uvc ${ODE_LIBRARIES} drawstuff ncurses)
