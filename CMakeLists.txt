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

add_executable(hello hello.cc)
target_link_libraries(hello ${ODE_LIBRARIES} drawstuff)

add_executable(ds_demo ds_demo.cc)
target_link_libraries(ds_demo ${ODE_LIBRARIES} drawstuff)

add_executable(collision collision.cc)
target_link_libraries(collision ${ODE_LIBRARIES} drawstuff)

add_executable(joint joint.cc)
target_link_libraries(joint ${ODE_LIBRARIES} drawstuff)

add_executable(force force.cc)
target_link_libraries(force ${ODE_LIBRARIES} drawstuff)

add_executable(arm arm.cc)
target_link_libraries(arm ${ODE_LIBRARIES} drawstuff)

add_executable(key key.cc)
target_link_libraries(key ${ODE_LIBRARIES} drawstuff)