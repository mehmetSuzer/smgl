
cmake_minimum_required(VERSION 3.0.0)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(smgl VERSION 0.1.0)

set(LIB_DIR lib)
set(DATA_DIR data)
file(COPY ${CMAKE_SOURCE_DIR}/${DATA_DIR} DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(${LIB_DIR}/stb)
include_directories(${LIB_DIR}/stb)

add_subdirectory(${LIB_DIR}/color)
include_directories(${LIB_DIR}/color)

add_subdirectory(${LIB_DIR}/vector3d)
include_directories(${LIB_DIR}/vector3d)

add_subdirectory(${LIB_DIR}/matrix3x3)
include_directories(${LIB_DIR}/matrix3x3)

add_subdirectory(${LIB_DIR}/camera)
include_directories(${LIB_DIR}/camera)

add_subdirectory(${LIB_DIR}/shape)
include_directories(${LIB_DIR}/shape)

add_subdirectory(${LIB_DIR}/sphere)
include_directories(${LIB_DIR}/sphere)

add_subdirectory(${LIB_DIR}/triangle)
include_directories(${LIB_DIR}/triangle)

add_subdirectory(${LIB_DIR}/bezier)
include_directories(${LIB_DIR}/bezier)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    bezier
    triangle
    sphere
    camera
    color
    vector3d
    matrix3x3
    stb
)
