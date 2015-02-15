set(APP_NAME lua-empty-test)

set(SAMPLE_SRC Classes/AppDelegate.cpp)

if(LINUX)
    list(APPEND SAMPLE_SRC proj.linux/main.cpp)
elseif(WINDOWS)
    list(APPEND SAMPLE_SRC proj.win32/main.cpp)
elseif(MACOSX OR APPLE)
    list(APPEND SAMPLE_SRC proj.mac/main.cpp)
endif()

include_directories(
  Classes
  ../../../cocos/scripting/lua-bindings/manual
  ../../../external/lua/lua
  ../../../external/lua/tolua
)

cocos_add_executable(${APP_NAME} ${SAMPLE_SRC} RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

target_link_libraries(${APP_NAME} luacocos2d)

cocos_add_resources(${APP_NAME} ../res ../src)
file(GLOB COCOS_SCRIPTS ${CMAKE_SOURCE_DIR}/cocos/scripting/lua-bindings/script/*)
cocos_add_resources(${APP_NAME} ${COCOS_SCRIPTS} PREFIX /src/cocos/)
