# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kouryuu/opencv_lib

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kouryuu/opencv_lib

# Include any dependencies generated for this target.
include CMakeFiles/opencv_lib.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/opencv_lib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/opencv_lib.dir/flags.make

CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o: CMakeFiles/opencv_lib.dir/flags.make
CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o: src/opencv_lib.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kouryuu/opencv_lib/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o -c /home/kouryuu/opencv_lib/src/opencv_lib.cpp

CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kouryuu/opencv_lib/src/opencv_lib.cpp > CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.i

CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kouryuu/opencv_lib/src/opencv_lib.cpp -o CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.s

CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o.requires:

.PHONY : CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o.requires

CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o.provides: CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o.requires
	$(MAKE) -f CMakeFiles/opencv_lib.dir/build.make CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o.provides.build
.PHONY : CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o.provides

CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o.provides.build: CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o


CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o: CMakeFiles/opencv_lib.dir/flags.make
CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o: src/opencv_bio.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kouryuu/opencv_lib/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o -c /home/kouryuu/opencv_lib/src/opencv_bio.cpp

CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kouryuu/opencv_lib/src/opencv_bio.cpp > CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.i

CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kouryuu/opencv_lib/src/opencv_bio.cpp -o CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.s

CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o.requires:

.PHONY : CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o.requires

CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o.provides: CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o.requires
	$(MAKE) -f CMakeFiles/opencv_lib.dir/build.make CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o.provides.build
.PHONY : CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o.provides

CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o.provides.build: CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o


# Object files for target opencv_lib
opencv_lib_OBJECTS = \
"CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o" \
"CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o"

# External object files for target opencv_lib
opencv_lib_EXTERNAL_OBJECTS =

lib/libopencv_lib.so: CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o
lib/libopencv_lib.so: CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o
lib/libopencv_lib.so: CMakeFiles/opencv_lib.dir/build.make
lib/libopencv_lib.so: /usr/local/lib/libopencv_highgui.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/libopencv_videoio.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/libopencv_face.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/liborbweaver.so
lib/libopencv_lib.so: /usr/local/lib/libopencv_imgcodecs.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/libopencv_photo.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/libopencv_cudaimgproc.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/libopencv_cudafilters.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/libopencv_cudaarithm.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/libopencv_objdetect.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/libopencv_dnn.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/libopencv_calib3d.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/libopencv_features2d.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/libopencv_imgproc.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/libopencv_flann.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/libopencv_core.so.4.6.0
lib/libopencv_lib.so: /usr/local/lib/libopencv_cudev.so.4.6.0
lib/libopencv_lib.so: CMakeFiles/opencv_lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kouryuu/opencv_lib/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX shared library lib/libopencv_lib.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/opencv_lib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/opencv_lib.dir/build: lib/libopencv_lib.so

.PHONY : CMakeFiles/opencv_lib.dir/build

CMakeFiles/opencv_lib.dir/requires: CMakeFiles/opencv_lib.dir/src/opencv_lib.cpp.o.requires
CMakeFiles/opencv_lib.dir/requires: CMakeFiles/opencv_lib.dir/src/opencv_bio.cpp.o.requires

.PHONY : CMakeFiles/opencv_lib.dir/requires

CMakeFiles/opencv_lib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/opencv_lib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/opencv_lib.dir/clean

CMakeFiles/opencv_lib.dir/depend:
	cd /home/kouryuu/opencv_lib && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kouryuu/opencv_lib /home/kouryuu/opencv_lib /home/kouryuu/opencv_lib /home/kouryuu/opencv_lib /home/kouryuu/opencv_lib/CMakeFiles/opencv_lib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/opencv_lib.dir/depend

