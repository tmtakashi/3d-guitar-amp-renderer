add_executable(HRIRSofaReaderTest HRIRSofaReaderTest.cpp)
target_include_directories(HRIRSofaReaderTest PUBLIC
    ${PROJECT_SOURCE_DIR}/src/SofaReader
)
target_link_libraries(HRIRSofaReaderTest PUBLIC 
                    gtest_main
                    SofaReader
)
add_test(NAME HRIRSofaReader_test COMMAND HRIRSofaReaderTest)