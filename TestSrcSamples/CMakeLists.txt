cmake_minimum_required(VERSION 3.20)

project(TestSrcSamples LANGUAGES CXX)

# -------------
# C++ standard
# -------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------
# Load RTL first (defines rtl_enable)
# ------------------------------------

add_subdirectory(ReflectionTemplateLib)

# ------------------
# User source files
# ------------------

file(GLOB USER_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# ------------------
# Executable target
# ------------------

add_executable(TestSrcSamples
    main.cpp
    ${USER_SRCS}
)

# -----------------------------------
# Include directories (user headers)
# -----------------------------------

target_include_directories(TestSrcSamples PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ----------------------------------------------
# Enable RTL (runtime + registration injection)
# ----------------------------------------------

rtl_enable(TestSrcSamples)

# ---------------------------
# Visual Studio organization
# ---------------------------

source_group("Source Files" FILES
    main.cpp
    ${USER_SRCS}
)
