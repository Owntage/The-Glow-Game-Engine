cmake_minimum_required(VERSION 2.8)
project(android-sfml-game)

file(GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/../source/*.cpp)

add_executable(game ${SOURCE_FILES})

add_definitions(-std=c++11 -g)

set(SFML_ROOT ${PROJECT_SOURCE_DIR}/../)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(SFML REQUIRED system window graphics network audio)


if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	include_directories(${PROJECT_SOURCE_DIR}/include)
		
	target_link_libraries(game ${SFML_LIBRARIES})
endif()




