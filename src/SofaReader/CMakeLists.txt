add_library(SofaReader SofaReader.hpp HRIRSofaReader.cpp DRIRSofaReader.cpp)

target_include_directories(SofaReader PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/libsofa/src
)

target_link_libraries(SofaReader ${PROJECT_SOURCE_DIR}/lib/libcurl.a)
target_link_libraries(SofaReader ${PROJECT_SOURCE_DIR}/lib/libhdf5.a)
target_link_libraries(SofaReader ${PROJECT_SOURCE_DIR}/lib/libhdf5_hl.a)
target_link_libraries(SofaReader ${PROJECT_SOURCE_DIR}/lib/libnetcdf.a)
target_link_libraries(SofaReader ${PROJECT_SOURCE_DIR}/lib/libnetcdf_c++4.a)
target_link_libraries(SofaReader ${PROJECT_SOURCE_DIR}/lib/libz.a)
target_link_libraries(SofaReader ${PROJECT_SOURCE_DIR}/lib/libsofa.a)