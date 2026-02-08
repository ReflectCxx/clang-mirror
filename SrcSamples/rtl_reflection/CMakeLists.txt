cmake_minimum_required(VERSION 3.16)

add_subdirectory(rtl)

file(GLOB RTL_REFLECTION_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/reg_src/*.cpp
)

add_library(rtl_reflection STATIC
    ${RTL_REFLECTION_SRCS}
)


target_include_directories(rtl_reflection PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)


target_link_libraries(rtl_reflection PUBLIC rtl)


target_compile_features(rtl_reflection PUBLIC cxx_std_20)
set_target_properties(rtl_reflection PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
