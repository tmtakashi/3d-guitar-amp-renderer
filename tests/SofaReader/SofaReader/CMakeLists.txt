add_executable(SofaReaderTest SofaReaderTest.cpp)
target_include_directories(SofaReaderTest PUBLIC
    ${PROJECT_SOURCE_DIR}/src/SofaReader
)
target_link_libraries(SofaReaderTest PUBLIC 
                    gtest_main
                    SofaReader
)
add_test(NAME SofaReader_test COMMAND SofaReaderTest)