add_library(vcdgen INTERFACE)

target_include_directories(vcdgen INTERFACE "${CURRENT_CMAKE_SOURCE_DIR}")

#add_library(
#    vcdgen SHARED
#    vcdgen.h
#)
#
#set_target_properties(vcdgen PROPERTIES 
#		LINKER_LANGUAGE CXX
#		CXX_STANDARD 11
#		CXX_STANDARD_REQUIRED YES
#         	LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
#)
#
#target_include_directories(vcdgen PUBLIC "${CURRENT_CMAKE_SOURCE_DIR}")
#
#message("PROJECT_BINARY_DIR is ${PROJECT_BINARY_DIR}")
