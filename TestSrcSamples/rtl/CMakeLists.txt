cmake_minimum_required(VERSION 3.20)

project(ReflectionTemplateLib LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# Public headers
set(LOCAL_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/rtl_access.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/rtl_builder.h"
)

target_sources(${PROJECT_NAME}
    PUBLIC
        ${LOCAL_HEADERS}
)

# Correct include dirs (distribution-safe)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rtl>
        $<INSTALL_INTERFACE:include>
)

# Add internal structure (injects sources into this target)
add_subdirectory(rtl)

# Provide professional namespace alias
add_library(RTL::ReflectionTemplateLib ALIAS ${PROJECT_NAME})
