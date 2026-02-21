cmake_minimum_required(VERSION 3.20)

project(ReflectionTemplateLib LANGUAGES CXX)

# ---------------------------
# Create RTL runtime library
# ---------------------------

add_library(ReflectionTemplateLib STATIC)

target_compile_features(ReflectionTemplateLib PUBLIC cxx_std_20)

# ---------------
# Public headers
# ---------------

set(RTL_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl_access.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl_builder.h
)

target_sources(ReflectionTemplateLib PRIVATE
    ${RTL_PUBLIC_HEADERS}
)

# --------------------
# Include directories
# --------------------

target_include_directories(ReflectionTemplateLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rtl>
        $<INSTALL_INTERFACE:include>
)

# --------------------------------
# Add internal RTL implementation
# --------------------------------
add_subdirectory(rtl)

# ----------------
# Namespace alias
# ----------------

add_library(RTL::ReflectionTemplateLib ALIAS ReflectionTemplateLib)

# -------------------------
# Load integration helpers
# -------------------------

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/RTLIntegration.cmake)

# -----------------------
# Visual Studio grouping
# -----------------------

source_group("Header Files" FILES ${RTL_PUBLIC_HEADERS})