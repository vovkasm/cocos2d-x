set(APP_NAME cpp-empty-test)

if(ANDROID)
  set(PLATFORM_SRC proj.android/jni/hellocpp/main.cpp)
  set(RES_PREFIX "/Resources")
elseif(WINDOWS)
  set(PLATFORM_SRC proj.win32/main.cpp)
  set(RES_PREFIX "")
elseif(IOS)
  set(PLATFORM_SRC
    proj.ios/main.m
    proj.ios/AppController.mm
    proj.ios/RootViewController.mm
  )

elseif(MACOSX OR APPLE)
  set(PLATFORM_SRC proj.mac/main.cpp)

  file(GLOB_RECURSE RES_FILES Resources/*)
  cocos_mark_resources(FILES ${RES_FILES} BASEDIR Resources)
  list(APPEND PLATFORM_SRC ${RES_FILES})

elseif(LINUX)
  set(PLATFORM_SRC proj.linux/main.cpp)
  set(RES_PREFIX "/Resources")
else()
  message( FATAL_ERROR "Unsupported platform, CMake will exit" )

endif()

set(SAMPLE_SRC
  ${PLATFORM_SRC}
  Classes/AppDelegate.cpp
  Classes/HelloWorldScene.cpp
)

# add the executable
add_executable(${APP_NAME} WIN32 MACOSX_BUNDLE ${SAMPLE_SRC})

target_link_libraries(${APP_NAME} cocos2d)

if(MSVC)
  set_target_properties(${APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${APP_NAME}")

  #get our dlls
  string(REPLACE ";" "$<SEMICOLON>" DLL_DIRS_ARG "${DLL_DIRS}")
  add_custom_command(
    TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -DAPP_DIR=$<TARGET_FILE:${APP_NAME}> -DDLL_DIRS:PATH=${DLL_DIRS_ARG} -P ${Cocos2d-X_SOURCE_DIR}/cmake/bundle_fixup.cmake
    COMMENT "Copying dependant dlls"
    )

  add_custom_command(
    TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Resources $<TARGET_FILE_DIR:${APP_NAME}>/${RES_PREFIX}
    COMMENT "Copying resources"
    )
elseif(MACOSX OR APPLE)
  set_target_properties(${APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

else()

  set(APP_BIN_DIR "${CMAKE_BINARY_DIR}/bin/${APP_NAME}")

  set_target_properties(${APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY  "${APP_BIN_DIR}")

  add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Resources $<TARGET_FILE_DIR:${APP_NAME}>${RES_PREFIX}
    )

endif()

