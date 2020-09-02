cmake_minimum_required(VERSION 3.0.0)
project(hoa-binaural VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()
include(CPack)

add_subdirectory(src/SofaReader)

add_executable(hoa-binaural src/main.cpp)
target_link_libraries(hoa-binaural PUBLIC SofaReader)
target_include_directories(hoa-binaural PUBLIC
    ${PROJECT_SOURCE_DIR}/src
)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
