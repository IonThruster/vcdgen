add_executable(vcdgen_tester test.cpp)

target_link_libraries(vcdgen_tester
                    PRIVATE vcdgen)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/golden/test_001_golden.vcd ${CMAKE_CURRENT_BINARY_DIR}/golden/test_001_golden.vcd COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/golden/test_002_golden.vcd ${CMAKE_CURRENT_BINARY_DIR}/golden/test_002_golden.vcd COPYONLY)
