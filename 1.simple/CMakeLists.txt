#irender execute
project(5.texture)

set(PP_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE PP_SOURCES_ALL ${PP_SOURCES_DIR}/*.h ${PP_SOURCES_DIR}/*.cpp ${PP_SOURCES_DIR}/*.qrc)
set(PP_SOURCES ${PP_SOURCES_ALL})

add_executable(${PROJECT_NAME} ${PP_SOURCES})
#add_dependencies(${PROJECT_NAME} shared irender)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h)
# update translation file and generate the rcc resource file
#updateDependentResIfNec(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/../studioapplication/Application/Windows/$<CONFIG>)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(${PROJECT_NAME}
    PROPERTIES
   #ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
   #LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
   RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
   RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
   RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/bin"
   #RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/../studioapplication/Application/Windows/RelWithDebInfo"
)

if (WIN32)
  # Set Windows application icon
  #target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/PP.rc)

  # remove console Windows
  set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
  source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PP_SOURCES})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/ffmpeg-4.2.1-win32-dev/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/sdl2/include)

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/ffmpeg-4.2.1-win32-dev/lib)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/sdl2/lib)

target_compile_definitions(${PROJECT_NAME} PRIVATE -DUNICODE -D_UNICODE)

  #set_target_properties(${PROJECT_NAME} PROPERTIES IMPORTED_LOCATION_DEBUG ${lib_debug})
  #set_target_properties(${PROJECT_NAME} PROPERTIES IMPORTED_LOCATION_RELEASE ${lib_release})

#set(Glog debug ${CMAKE_SOURCE_DIR}/third_party/glog/lib/glogd.lib optimized ${CMAKE_SOURCE_DIR}/third_party/glog/lib/glog.lib)

set (EXTRA_LIBS 
	#${Glog} 
	Crypt32.lib
    Version.lib
    Shlwapi.lib
	avformat.lib
	avcodec.lib
	avdevice.lib
	avfilter.lib
	avutil.lib
	swresample.lib
	swscale.lib
	Qt5::Core
    Qt5::Network
    Qt5::Sql
    Qt5::Svg
    Qt5::WebChannel
    Qt5::WebEngine
    Qt5::WebEngineCore
    Qt5::WebEngineWidgets
    Qt5::Widgets
    Qt5::Xml)
target_link_libraries(${PROJECT_NAME} ${EXTRA_LIBS})
target_compile_options(${PROJECT_NAME} PRIVATE ${STUDIO_COMPILE_OPTIONS})
