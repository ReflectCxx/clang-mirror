cmake_minimum_required(VERSION 3.20)

project(TestSrcSamples LANGUAGES CXX)

# --------------------------------------------------
# C++ standard
# --------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------
# Add RTL library
# --------------------------------------------------

add_subdirectory(rtl)

# --------------------------------------------------
# User source files
# --------------------------------------------------

file(GLOB USER_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# --------------------------------------------------
# Generated mirror source files
# --------------------------------------------------

file(GLOB CLANG_MR_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/clang-mr/reg_src/*.cpp
)

# Mark generated sources explicitly
set_source_files_properties(${CLANG_MR_SRCS}
    PROPERTIES GENERATED TRUE
)

# --------------------------------------------------
# Executable target
# --------------------------------------------------

add_executable(TestSrcSamples
    main.cpp
    ${USER_SRCS}
    ${CLANG_MR_SRCS}
)

# --------------------------------------------------
# Include directories
# --------------------------------------------------

target_include_directories(TestSrcSamples PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/clang-mr
)

# --------------------------------------------------
# Link RTL library (clean, namespaced)
# --------------------------------------------------

target_link_libraries(TestSrcSamples PRIVATE
    RTL::ReflectionTemplateLib
)

# --------------------------------------------------
# Visual Studio organization
# --------------------------------------------------

source_group("Source Files" FILES
    main.cpp
    ${USER_SRCS}
)

source_group("Generated Files" FILES
    ${CLANG_MR_SRCS}
)
