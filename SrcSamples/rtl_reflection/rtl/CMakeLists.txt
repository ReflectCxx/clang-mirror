# rtl/CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

# --------------------------------
# RTL library target
# --------------------------------

add_library(rtl STATIC)

target_compile_features(rtl PUBLIC cxx_std_20)

# --------------------------------
# Top-level RTL headers
# --------------------------------

set(RTL_LOCAL_HEADERS
    rtl_errors.h
    rtl_traits.h
    rtl_typeid.h
    rtl_constants.h
    rtl_forward_decls.h
    rtl_constructor.h

    rtl_function.h
    rtl_function_erased_return.h

    rtl_method.h
    rtl_method_const.h
    rtl_method_erased.h
    rtl_method_erased_return.h
    rtl_method_erased_target.h
)

target_sources(rtl PRIVATE ${RTL_LOCAL_HEADERS})

source_group("Header Files\\RTL" FILES ${RTL_LOCAL_HEADERS})

# --------------------------------
# Public include dirs
# --------------------------------

target_include_directories(rtl PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# --------------------------------
# Submodules
# --------------------------------

add_subdirectory(inc)
add_subdirectory(src)
add_subdirectory(builder)
add_subdirectory(detail)
add_subdirectory(dispatch)
