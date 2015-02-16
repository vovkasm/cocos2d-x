include(CMakeParseArguments)

macro(pre_build TARGET_NAME)
  add_custom_target( ${TARGET_NAME}_PRE_BUILD ALL )

  add_custom_command(
    TARGET ${TARGET_NAME}_PRE_BUILD
    ${ARGN}
    PRE_BUILD
    COMMENT "${TARGET_NAME}_PRE_BUILD ..."
    )

  add_custom_target(${TARGET_NAME}_CORE_PRE_BUILD)
  add_dependencies(${TARGET_NAME}_PRE_BUILD ${TARGET_NAME}_CORE_PRE_BUILD)
  add_dependencies(${TARGET_NAME} ${TARGET_NAME}_PRE_BUILD)
endmacro()

function(cocos_add_resources target)
    set(oneValueArgs PREFIX)
    cmake_parse_arguments(opt "" "${oneValueArgs}" "" ${ARGN})
    if(NOT opt_PREFIX)
        set(opt_PREFIX /)
    endif()
    if(NOT (opt_PREFIX MATCHES "^/"))
        set(opt_PREFIX "/${opt_PREFIX}")
    endif()

    if(MACOSX OR APPLE)
        set(PLATFORM_RES_PREFIX "\${APP_DIR}/Contents/Resources")
    elseif(WIN32)
        set(PLATFORM_RES_PREFIX "\${APP_DIR}")
    else()
        set(PLATFORM_RES_PREFIX "\${APP_DIR}/Resources")
    endif()

    foreach(f ${opt_UNPARSED_ARGUMENTS})
        get_filename_component(f_abs "${f}" ABSOLUTE)

        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${target}_make_bundle.cmake "file(COPY \"${f_abs}\" DESTINATION \"${PLATFORM_RES_PREFIX}${opt_PREFIX}\" REGEX \"\\\\.git\" EXCLUDE)\n")
    endforeach()
endfunction()

function(cocos_add_executable target)
    set(oneValueArgs RUNTIME_OUTPUT_DIRECTORY)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(opt "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT opt_RUNTIME_OUTPUT_DIRECTORY)
        set(opt_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    endif()

    add_executable(${target} WIN32 MACOSX_BUNDLE ${opt_SOURCES} ${opt_UNPARSED_ARGUMENTS})
    target_link_libraries(${target} cocos2d)

    # On macosx we create bundle by default, on other desktop systems we are emulating this behaviour by creating per-application directory
    if(MACOSX OR APPLE)
        set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${opt_RUNTIME_OUTPUT_DIRECTORY}")
    else()
        set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${opt_RUNTIME_OUTPUT_DIRECTORY}/${target}")
    endif()

    # Make Linux executable relocatable
    if(LINUX)
        set_target_properties(${target} PROPERTIES INSTALL_RPATH "\$ORIGIN/" BUILD_WITH_INSTALL_RPATH TRUE)
    endif()

    configure_file(${Cocos2d-X_SOURCE_DIR}/cmake/make_bundle.cmake.in ${target}_make_bundle.cmake @ONLY)

    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -DAPP_PATH=$<TARGET_FILE:${target}> -P ${target}_make_bundle.cmake
        COMMENT "Make application bundle"
        )

endfunction()

# cocos_find_package(pkg args...)
# works same as find_package, but do additional care to properly find
# prebuilt libs for cocos
macro(cocos_find_package pkg_name pkg_prefix)
  if(NOT USE_PREBUILT_LIBS OR NOT ${pkg_prefix}_FOUND)
    find_package(${pkg_name} ${ARGN})
  endif()
  if(NOT ${pkg_prefix}_INCLUDE_DIRS AND ${pkg_prefix}_INCLUDE_DIR)
    set(${pkg_prefix}_INCLUDE_DIRS ${${pkg_prefix}_INCLUDE_DIR})
  endif()
  if(NOT ${pkg_prefix}_LIBRARIES AND ${pkg_prefix}_LIBRARY)
    set(${pkg_prefix}_LIBRARIES ${${pkg_prefix}_LIBRARY})
  endif()

  message(STATUS "${pkg_name} include dirs: ${${pkg_prefix}_INCLUDE_DIRS}")
endmacro()

# cocos_use_pkg(pkg) function.
# This function applies standard package variables (after find_package(pkg) call) to current scope
# Recognized variables: <pkg>_INCLUDE_DIRS, <pkg>_LIBRARIES, <pkg>_LIBRARY_DIRS
# Also if BUILD_SHARED_LIBS variable off, it is try to use <pkg>_STATIC_* vars before
function(cocos_use_pkg target pkg)
  set(prefix ${pkg})
  
  set(_include_dirs)
  if(NOT _include_dirs)
    set(_include_dirs ${${prefix}_INCLUDE_DIRS})
  endif()
  if(NOT _include_dirs)
    # backward compat with old package-find scripts
    set(_include_dirs ${${prefix}_INCLUDE_DIR})
  endif()
  if(_include_dirs)
    include_directories(${_include_dirs})
    message(STATUS "${pkg} add to include_dirs: ${_include_dirs}")
  endif()
  
  set(_library_dirs)
  if(NOT _library_dirs)
    set(_library_dirs ${${prefix}_LIBRARY_DIRS})
  endif()
  if(_library_dirs)
    link_directories(${_library_dirs})
    message(STATUS "${pkg} add to link_dirs: ${_library_dirs}")
  endif()
  
  set(_libs)
  if(NOT _libs)
    set(_libs ${${prefix}_LIBRARIES})
  endif()
  if(NOT _libs)
    set(_libs ${${prefix}_LIBRARY})
  endif()
  if(_libs)
    target_link_libraries(${target} ${_libs})
    message(STATUS "${pkg} libs added to '${target}': ${_libs}")
  endif()
  
  set(_defs)
  if(NOT _defs)
    set(_defs ${${prefix}_DEFINITIONS})
  endif()
  if(_defs)
    add_definitions(${_defs})
    message(STATUS "${pkg} add definitions: ${_defs}")
  endif()
endfunction()

#cmake has some strange defaults, this should help us a lot
#Please use them everywhere

#WINDOWS 	= 	Windows Desktop
#WINRT 		= 	Windows RT
#WP8 	  	= 	Windows Phone 8
#ANDROID    =	Android
#IOS		=	iOS
#MACOSX		=	MacOS X
#LINUX      =   Linux

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  if(WINRT)
    set(SYSTEM_STRING "Windows RT")
  elseif(WP8)
    set(SYSTEM_STRING "Windows Phone 8")
  else()
    set(WINDOWS TRUE)
    set(SYSTEM_STRING "Windows Desktop")
  endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  if(ANDROID)
    set(SYSTEM_STRING "Android")
  else()
    set(LINUX TRUE)
    set(SYSTEM_STRING "Linux")
  endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  if(IOS)
    set(SYSTEM_STRING "IOS")
  else()
    set(MACOSX TRUE)
    set(APPLE TRUE)
    set(SYSTEM_STRING "Mac OSX")
  endif()
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(COMPILER_STRING ${CMAKE_CXX_COMPILER_ID})
  set(CLANG TRUE)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  if(MINGW)
    set(COMPILER_STRING "Mingw GCC")
  else()
    set(COMPILER_STRING "GCC")
  endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  set(COMPILER_STRING "${CMAKE_CXX_COMPILER_ID} C++")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(COMPILER_STRING "Visual Studio C++")
endif()

if(CMAKE_CROSSCOMPILING)
  set(BUILDING_STRING "It appears you are cross compiling for ${SYSTEM_STRING} with ${COMPILER_STRING}")
else()
  set(BUILDING_STRING "It appears you are builing natively for ${SYSTEM_STRING} with ${COMPILER_STRING}")
endif()
