add_library(
    vcdgen
    vcdgen.h
)

set_target_properties(vcdgen PROPERTIES 
		LINKER_LANGUAGE CXX
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED YES
)

target_include_directories(vcdgen PUBLIC "${CURRENT_CMAKE_SOURCE_DIR}")
