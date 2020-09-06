add_executable(DRIRSofaReaderTest DRIRSofaReaderTest.cpp)
target_include_directories(DRIRSofaReaderTest PUBLIC
    ${PROJECT_SOURCE_DIR}/src/SofaReader
)
target_link_libraries(DRIRSofaReaderTest PUBLIC 
                    gtest_main
                    SofaReader
)
add_test(NAME DRIRSofaReader_test COMMAND DRIRSofaReaderTest)