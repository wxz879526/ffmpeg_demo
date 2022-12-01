cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(FFmpegDemo VERSION 0.0.1 LANGUAGES CXX)

option(USE_WERROR "Error on compile warning" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# we only support Debug and RelWithDebInfo configuration 
set(CMAKE_CONFIGURATION_TYPES Debug RelWithDebInfo CACHE STRING INTERNAL FORCE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(MUIModule)
include(FileUtils)

# Set compiler options
if(MSVC)
  set(STUDIO_COMPILE_OPTIONS
    /wd4251
    #/wd4244
    /experimental:external
    /external:anglebrackets 
    /external:W0 #https://docs.microsoft.com/en-us/cpp/build/reference/external-external-headers-diagnostics?view=msvc-170
    /W4
    "$<$<CONFIG:RELWITHDEBINFO>:/Od>"
    "$<$<COMPILE_LANGUAGE:CXX>:/MP>"
  )
  if (USE_WERROR)
    list(APPEND STUDIO_COMPILE_OPTIONS "/WX")
  endif()
else()
  set(STUDIO_COMPILE_OPTIONS
    "$<$<CONFIG:RELEASE>:-O2>"
    -Wuninitialized
    -pedantic-errors
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wshadow
  )
  if (USE_WERROR)
    list(APPEND STUDIO_COMPILE_OPTIONS "-Werror")
  endif()
endif()

set(STUDIO_DEFINITIONS -DQT_DEPRECATED_WARNINGS)
if (WIN32)
  list(APPEND STUDIO_DEFINITIONS -DUNICODE -D_UNICODE)
endif()

# Link Qt 5>
# 设置本机的QT安装路径
#set(QTDIR "D:\\Qt\\Qt5.14.0\\5.14.0\\msvc2017_64")
#set(CMAKE_PREFIX_PATH ${QTDIR})

set(QT_LIBRARIES
  Core
  Gui
  Network
  Widgets
  OpenGL
  LinguistTools
  Concurrent
  Sql
  Svg
  WebChannel
  WebEngine
  WebEngineCore
  WebEngineWidgets
  Xml
)

find_package(Qt5 5.14 REQUIRED
  COMPONENTS
    ${QT_LIBRARIES}
)

add_subdirectory(1.simple)
add_subdirectory(2.decode_use)