set(APP_NAME cpp-empty-test)

if(ANDROID)
  set(PLATFORM_SRC proj.android/jni/hellocpp/main.cpp)
elseif(WINDOWS)
  set(PLATFORM_SRC proj.win32/main.cpp)
elseif(IOS)
  set(PLATFORM_SRC
    proj.ios/main.m
    proj.ios/AppController.mm
    proj.ios/RootViewController.mm
  )
elseif(MACOSX OR APPLE)
  set(PLATFORM_SRC proj.mac/main.cpp)
elseif(LINUX)
  set(PLATFORM_SRC proj.linux/main.cpp)
else()
  message( FATAL_ERROR "Unsupported platform, CMake will exit" )

endif()

set(SAMPLE_SRC
  ${PLATFORM_SRC}
  Classes/AppDelegate.cpp
  Classes/HelloWorldScene.cpp
)

cocos_add_executable(${APP_NAME}
    SOURCES ${SAMPLE_SRC}
    BUNDLE_RESOURCES Resources
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

