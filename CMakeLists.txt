cmake_minimum_required(VERSION 3.10)
#Setting compiller flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-O0 -g)
set(PROJECT_NAME pong)
project(${PROJECT_NAME})

#Define static lib
list(APPEND STATIC_LIBS m)

if(MINGW)
  message(STATUS "Build on MINGW")
  set(LIBS_PATH c:/mingw_dev_lib/lib)
  set(LIBS__INCLUDE_PATH c:/mingw_dev_lib/include/SDL2)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(STATUS "Build on LINUX")
  set(LIBS_PATH /usr/local/lib)
  set(LIBS__INCLUDE_PATH /usr/local/include/SDL2 /usr/include/SDL2)
endif()
#Find libs
find_library(SDL2main SDL2main NAME SDL2main HINTS ${LIBS_PATH})
find_library(SDL2 SDL2 NAME SDL2 HINTS ${LIBS_PATH})
find_library(SDL2_image SDL2_image NAME SDL2_image HINTS ${LIBS_PATH})
find_library(SDL2_ttf SDL2_ttf NAME SDL2_ttf HINTS ${LIBS_PATH})

#Gather files
file(GLOB SRCS ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/src/*.cpp)

#Build
add_executable(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/res)
target_include_directories(${PROJECT_NAME} PUBLIC ${LIBS__INCLUDE_PATH})
target_link_libraries(${PROJECT_NAME} PUBLIC "$<$<PLATFORM_ID:Windows>:mingw32 ${SDL2main}>" ${STATIC_LIBS} ${SDL2} ${SDL2_image} ${SDL2_ttf})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/res/ $<TARGET_FILE_DIR:${PROJECT_NAME}>)
