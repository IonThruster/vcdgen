add_executable(vcdgen_tester test.cpp)

target_link_libraries(vcdgen_tester
                    PRIVATE vcdgen)
