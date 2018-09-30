cmake_minimum_required(VERSION 2.8)

project(LuvaLampServer CXX)

######################################
### External Packages

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC true)

######################################
### Include directories

set(${PROJECT_NAME}_DIRS
	includes
)

######################################
### Headers

set(${PROJECT_NAME}_HDR
	includes/Server.h
)

######################################
## Source

set(${PROJECT_NAME}_SRC
	src/main.cpp
	src/Server.cpp
)

######################################
## Executable

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_HDR} ${${PROJECT_NAME}_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_DIRS})
target_link_libraries(${PROJECT_NAME} Qt5::Network Qt5::Widgets)
