set(APP_NAME lua-tests)

set(SAMPLE_SRC
    Classes/AppDelegate.cpp
    Classes/lua_assetsmanager_test_sample.cpp
    Classes/lua_test_bindings.cpp
)

if(LINUX)
    set(SAMPLE_SRC ${SAMPLE_SRC} proj.linux/main.cpp)
elseif(WINDOWS)
    list(APPEND SAMPLE_SRC proj.win32/main.cpp)
elseif(MACOSX)
    set(SAMPLE_SRC ${SAMPLE_SRC}
        proj.ios_mac/mac/main.cpp
        proj.ios_mac/mac/LuaObjectCBridgeTest.h
        proj.ios_mac/mac/LuaObjectCBridgeTest.mm
        )
endif()

include_directories(
  Classes
  ../../../cocos/scripting/lua-bindings/manual
  ../../../cocos/scripting/lua-bindings/auto
  ../../../external/lua/lua
  ../../../external/lua/tolua
)

cocos_add_executable(${APP_NAME} ${SAMPLE_SRC} RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

target_link_libraries(${APP_NAME} luacocos2d)

cocos_add_resources(${APP_NAME} ../res ../src)
file(GLOB COCOS_SCRIPTS ${CMAKE_SOURCE_DIR}/cocos/scripting/lua-bindings/script/*)
cocos_add_resources(${APP_NAME} ${COCOS_SCRIPTS} PREFIX /src/cocos/)
file(GLOB CPP_TESTS_RES ${CMAKE_SOURCE_DIR}/tests/cpp-tests/Resources/*)
cocos_add_resources(${APP_NAME} ${CPP_TESTS_RES} PREFIX /res)
cocos_add_resources(${APP_NAME} ${CMAKE_SOURCE_DIR}/external/lua/luasocket/script)
