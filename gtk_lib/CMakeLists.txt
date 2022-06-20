SET(PROJECT_NAME opencv_lib)

PROJECT(${PROJECT_NAME} C CXX ASM)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
#set(CMAKE_VERBOSE_MAKEFILE ON)
enable_language(ASM)

#add_subdirectory(libglome)
#add_subdirectory(glome)
#add_subdirectory(torus)
#add_library(libglome)
#add_executable(torus)

set(OPENCV_LIB_TARGET_VERSION "4.0.0" CACHE STRING "OpenCV logical version")
set(OPENCV_LIB_TARGET_SOVERSION "4.0.0" CACHE STRING "OpenCV soname version")

set(BUILD_TYPE Release)
#set(BUILD_TYPE RelWithDebInfo)

set(EXECUTABLE_OUTPUT_PATH bin)
set(LIBRARY_OUTPUT_PATH lib)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# flags
message("Processor Type : ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message("OSX Architectures : ${CMAKE_OSX_ARCHITECTURES}")
message("OSX Deployment Target : ${CMAKE_OSX_DEPLOYMENT_TARGET}")
message("System Processor : ${CMAKE_SYSTEM_PROCESSOR}")
message("Lib Architecture : ${CMAKE_LIBRARY_ARCHITECTURE}")
message("Target Compiler : ${CMAKE_CXX_COMPILER_TARGET}")
message("ABI : ${CMAKE_CXX_COMPILER_ABI}")
message("C Compiler : ${CMAKE_C_COMPILER}")
message("C++ Compiler : ${CMAKE_CXX_COMPILER}")
message("Assembler : ${CMAKE_ASM_COMPILER}")
message("Lib Architecture : ${CMAKE_CXX_LIBRARY_ARCHITECTURE}")


if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin") 
    # OSX
    SET(OperatingSystem "MacOS")
    add_definitions("-DDARWIN")
    #pkg_search_module(jansson REQUIRED jansson>=1.0)
    #pkg_search_module(openssl REQUIRED openssl>=1.0)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
    SET(OperatingSystem "Linux")
    add_definitions("-DUNIX")
    #pkg_search_module(jansson REQUIRED jansson>=1.0)
    #pkg_search_module(openssl REQUIRED openssl>=1.0)
endif() 

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Windows specific code
    SET(OperatingSystem "Windows")
    add_definitions("-DWINDOWS")
endif()

find_package(OpenSSL REQUIRED) 

find_package(OpenCV QUIET COMPONENTS core highgui imgproc imgcodecs videoio)
if(NOT OpenCV_FOUND) # if not OpenCV 3.x, then imgcodecs are not found
  find_package(OpenCV REQUIRED COMPONENTS core highgui imgproc)
endif()
#include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})
#list(APPEND Caffe_LINKER_LIBS ${OpenCV_LIBS})
message(STATUS "OpenCV found (${OpenCV_CONFIG_PATH})")
add_definitions(-DUSE_OPENCV)


# ---[ gtk
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_search_module(GLIB REQUIRED glib-2.0 gobject-2.0 gmodule-2.0 )

# ---[ support for orbweaver interface
find_package(OrbWeaver CONFIG REQUIRED)
#find_package(Stack CONFIG REQUIRED)
if(OrbWeaver_FOUND)
  message("OrbWeaver found ${OrbWeaver_LIBRARIES}")
  add_definitions(-DHAVE_ORBWEAVER)
  ##list(APPEND Caffe_LINKER_LIBS ${Stack_LIBRARIES})
endif()


set(ASM_OPTIONS "-x assembler-with-cpp")
set(CMAKE_ASM_FLAGS "${CFLAGS} ${ASM_OPTIONS}")

set(CMAKE_CXX_FLAGS
   "${SSE_FLAGS} -O0 -g -march=native"
#   "${SSE_FLAGS} -O3 -g -std=c++0x -fno-omit-frame-pointer"
)
set(CMAKE_C_FLAGS
   "${SSE_FLAGS} -O0 -g -march=native"
#   "${SSE_FLAGS} -O3 -g -std=c++0x -fno-omit-frame-pointer"
)

if (MSVC)
     set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif (MSVC)

#global include directories
include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${GTK3_INCLUDE_DIRS} 
) 

# Sources files
set(gtklib_source_files 
  ${PROJECT_SOURCE_DIR}/src/gtk_lib.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_widget.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_window.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_container.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_button.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_label.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_input.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_combobox.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_progress_bar.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_switch.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_spinner.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_treeview.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_toolbar.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_image.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_frame.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_notebook.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_terminal.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_layout.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_table.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_stack.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_message_box.cpp
  ${PROJECT_SOURCE_DIR}/src/gtk_builder.cpp
)

# compile main library.
#include_directories(${OrbWeaver_INCLUDE_DIRS} ${GST_INCLUDE_DIRS}) 

if(${CMAKE_HOST_UNIX})
  message("UNIX System setup")
  set(ASM_SYS_FLAGS "-64")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-as-needed -fPIC -ldl -pthread")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--no-as-needed -fPIC -ldl -pthread")
  #set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed -ldl")
endif()

if(${CMAKE_HOST_WIN32})
  message("WIN32 System setup")
endif()

if(${CMAKE_HOST_APPLE})
  message("APPLE System setup")
  set(ASM_SYS_FLAGS "-arch x86_64")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-as-needed -fPIC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--no-as-needed -fPIC")
endif()


message("--- building gtk library.")
add_library(gtk_lib SHARED ${gtklib_source_files} )
target_link_libraries(gtk_lib ${GTK3_LIBRARIES} ${OrbWeaver_LIBRARIES}  )


