cmake_minimum_required(VERSION 3.20)

project(RTLRegistration LANGUAGES CXX)

add_library(RTLRegistration STATIC)

target_compile_features(RTLRegistration PUBLIC cxx_std_20)

# Automatically pick up generated headers
file(GLOB RTL_REG_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# Automatically pick up generated sources
file(GLOB RTL_REG_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/reg_src/*.cpp
)

target_sources(RTLRegistration
    PRIVATE
        ${RTL_REG_SOURCES}

    PUBLIC
        ${RTL_REG_HEADERS}
)

target_include_directories(RTLRegistration
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(RTLRegistration
    PUBLIC RTL::ReflectionTemplateLib
)

add_library(RTL::Registration ALIAS RTLRegistration)

# IDE organization
source_group("Reflection Mirror" FILES ${RTL_REG_HEADERS})
source_group("Generated Files" FILES ${RTL_REG_SOURCES})
