cmake_minimum_required(VERSION 3.16)
project(SrcSamples LANGUAGES CXX)

# -----------------------------
# User code target
# -----------------------------

file(GLOB USER_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(srcsamples_core STATIC
    ${USER_SRCS}
)

target_include_directories(srcsamples_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_compile_features(srcsamples_core PUBLIC cxx_std_20)

# -----------------------------
# User executable
# -----------------------------

add_executable(srcsamples_app
    main.cpp
)

target_link_libraries(srcsamples_app
    PRIVATE srcsamples_core
)

# -----------------------------
# Reflection (generated)
# -----------------------------

add_subdirectory(rtl_reflection)

# THIS LINE IS THE KEY FIX
target_link_libraries(rtl_reflection PUBLIC srcsamples_core)

target_link_libraries(srcsamples_app
    PRIVATE rtl_reflection
)
