# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = C:\Users\Joshua\AppData\Local\JetBrains\Toolbox\apps\CLion\ch-0\211.7142.21\bin\cmake\win\bin\cmake.exe

# The command to remove a file.
RM = C:\Users\Joshua\AppData\Local\JetBrains\Toolbox\apps\CLion\ch-0\211.7142.21\bin\cmake\win\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "D:\Technion\Operating Systems Wet Homework\OS-Wet-HW2"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "D:\Technion\Operating Systems Wet Homework\OS-Wet-HW2\cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/OS_Wet_HW2.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/OS_Wet_HW2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/OS_Wet_HW2.dir/flags.make

CMakeFiles/OS_Wet_HW2.dir/bank.cpp.obj: CMakeFiles/OS_Wet_HW2.dir/flags.make
CMakeFiles/OS_Wet_HW2.dir/bank.cpp.obj: ../bank.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="D:\Technion\Operating Systems Wet Homework\OS-Wet-HW2\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/OS_Wet_HW2.dir/bank.cpp.obj"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\OS_Wet_HW2.dir\bank.cpp.obj -c "D:\Technion\Operating Systems Wet Homework\OS-Wet-HW2\bank.cpp"

CMakeFiles/OS_Wet_HW2.dir/bank.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/OS_Wet_HW2.dir/bank.cpp.i"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "D:\Technion\Operating Systems Wet Homework\OS-Wet-HW2\bank.cpp" > CMakeFiles\OS_Wet_HW2.dir\bank.cpp.i

CMakeFiles/OS_Wet_HW2.dir/bank.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/OS_Wet_HW2.dir/bank.cpp.s"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "D:\Technion\Operating Systems Wet Homework\OS-Wet-HW2\bank.cpp" -o CMakeFiles\OS_Wet_HW2.dir\bank.cpp.s

# Object files for target OS_Wet_HW2
OS_Wet_HW2_OBJECTS = \
"CMakeFiles/OS_Wet_HW2.dir/bank.cpp.obj"

# External object files for target OS_Wet_HW2
OS_Wet_HW2_EXTERNAL_OBJECTS =

OS_Wet_HW2.exe: CMakeFiles/OS_Wet_HW2.dir/bank.cpp.obj
OS_Wet_HW2.exe: CMakeFiles/OS_Wet_HW2.dir/build.make
OS_Wet_HW2.exe: CMakeFiles/OS_Wet_HW2.dir/linklibs.rsp
OS_Wet_HW2.exe: CMakeFiles/OS_Wet_HW2.dir/objects1.rsp
OS_Wet_HW2.exe: CMakeFiles/OS_Wet_HW2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="D:\Technion\Operating Systems Wet Homework\OS-Wet-HW2\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable OS_Wet_HW2.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\OS_Wet_HW2.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/OS_Wet_HW2.dir/build: OS_Wet_HW2.exe

.PHONY : CMakeFiles/OS_Wet_HW2.dir/build

CMakeFiles/OS_Wet_HW2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\OS_Wet_HW2.dir\cmake_clean.cmake
.PHONY : CMakeFiles/OS_Wet_HW2.dir/clean

CMakeFiles/OS_Wet_HW2.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "D:\Technion\Operating Systems Wet Homework\OS-Wet-HW2" "D:\Technion\Operating Systems Wet Homework\OS-Wet-HW2" "D:\Technion\Operating Systems Wet Homework\OS-Wet-HW2\cmake-build-debug" "D:\Technion\Operating Systems Wet Homework\OS-Wet-HW2\cmake-build-debug" "D:\Technion\Operating Systems Wet Homework\OS-Wet-HW2\cmake-build-debug\CMakeFiles\OS_Wet_HW2.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/OS_Wet_HW2.dir/depend

