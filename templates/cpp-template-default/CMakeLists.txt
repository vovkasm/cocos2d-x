#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/
cmake_minimum_required(VERSION 2.8)

set(APP_NAME HelloCpp)
project(${APP_NAME})

set(BUILD_CPP_TESTS OFF CACHE BOOL "Build cocos2d-x tests")
set(BUILD_LUA_LIBS OFF CACHE BOOL "Build cocos2d-x with lua scripting")

set(COCOS2D_ROOT ${CMAKE_SOURCE_DIR}/cocos2d CACHE PATH "Path to cocos2d-x sources")

add_subdirectory(${COCOS2D_ROOT})

project(${APP_NAME})

set(CMAKE_MODULE_PATH "${COCOS2D_ROOT}/cmake/Modules" ${CMAKE_MODULE_PATH})
include(CocosBuildHelpers)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(PLATFORM_SPECIFIC_SRC)
set(PLATFORM_SPECIFIC_HEADERS)

if(IOS)
  set(PLATFORM_SPECIFIC_SRC
    proj.ios_mac/ios/main.m
    proj.ios_mac/ios/RootViewController.mm
    proj.ios_mac/ios/AppController.mm
  )
  set(PLATFORM_SPECIFIC_HEADERS
    proj.ios_mac/ios/RootViewController.h
    proj.ios_mac/ios/AppController.h
  )
elseif(MACOSX OR APPLE)
  set(PLATFORM_SPECIFIC_SRC proj.ios_mac/mac/main.cpp)
elseif(LINUX)
  set(PLATFORM_SPECIFIC_SRC proj.linux/main.cpp)
elseif(WIN32)
  set(PLATFORM_SPECIFIC_SRC proj.win32/main.cpp)
  set(PLATFORM_SPECIFIC_HEADERS
    proj.win32/main.h
    proj.win32/resource.h
  )
endif()

set(GAME_SRC
  Classes/AppDelegate.cpp
  Classes/HelloWorldScene.cpp
  ${PLATFORM_SPECIFIC_SRC}
)

set(GAME_HEADERS
  Classes/AppDelegate.h
  Classes/HelloWorldScene.h
  ${PLATFORM_SPECIFIC_HEADERS}
)

cocos_add_executable(${APP_NAME}
  SOURCES ${GAME_SRC} ${GAME_HEADERS}
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_include_directories(${APP_NAME} PUBLIC Classes $<TARGET_PROPERTY:cocos2d,INCLUDE_DIRECTORIES>)
target_compile_definitions(${APP_NAME} PUBLIC $<TARGET_PROPERTY:cocos2d,COMPILE_DEFINITIONS>)
target_compile_options(${APP_NAME} PUBLIC $<TARGET_PROPERTY:cocos2d,COMPILE_OPTIONS>)

file(GLOB GAME_RES "Resources/*")
cocos_add_resources(${APP_NAME} ${GAME_RES})

