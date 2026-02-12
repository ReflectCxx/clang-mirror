cmake_minimum_required(VERSION 3.21)

project(clang-mirror LANGUAGES CXX)

# --------------------------------------------------
# Language setup
# --------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CXX_EXE_NAME clang-mirror)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# --------------------------------------------------
# LLVM / Clang location (user supplied)
# --------------------------------------------------
set(LLVM_ROOT "" CACHE PATH "Root directory of extracted clang+llvm bundle")

if (NOT LLVM_ROOT)
    message(FATAL_ERROR
        "LLVM_ROOT not set.\n"
        "Configure with:\n"
        "  -DLLVM_ROOT=<path-to-clang+llvm-root>")
endif()

set(LLVM_DIR  "${LLVM_ROOT}/lib/cmake/llvm")
set(Clang_DIR "${LLVM_ROOT}/lib/cmake/clang")

if (NOT EXISTS "${LLVM_DIR}/LLVMConfig.cmake")
    message(FATAL_ERROR "LLVMConfig.cmake not found at ${LLVM_DIR}")
endif()

if (NOT EXISTS "${Clang_DIR}/ClangConfig.cmake")
    message(FATAL_ERROR "ClangConfig.cmake not found at ${Clang_DIR}")
endif()

find_package(LLVM CONFIG REQUIRED PATHS "${LLVM_DIR}" NO_DEFAULT_PATH)
find_package(Clang CONFIG REQUIRED PATHS "${Clang_DIR}" NO_DEFAULT_PATH)

message(STATUS "Found LLVM: ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM includes: ${LLVM_INCLUDE_DIRS}")
message(STATUS "Clang includes: ${CLANG_INCLUDE_DIRS}")

# --------------------------------------------------
# Target
# --------------------------------------------------
add_executable(${CXX_EXE_NAME})

add_subdirectory(inc)
add_subdirectory(src)

target_include_directories(${CXX_EXE_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${LLVM_INCLUDE_DIRS}
        ${CLANG_INCLUDE_DIRS}
)

# --------------------------------------------------
# LLVM components
# --------------------------------------------------
llvm_map_components_to_libnames(LLVM_LIBS
    support
)

# --------------------------------------------------
# Link clang tooling stack
# --------------------------------------------------
target_link_libraries(${CXX_EXE_NAME}
    PRIVATE
        clangTooling
        clangBasic
        clangAST
        clangFrontend
        clangSerialization
        clangDriver
        clangParse
        clangSema
        clangAnalysis
        clangEdit
        clangLex
        clangRewrite
        clangRewriteFrontend
        clangTidy
        ${LLVM_LIBS}
)

# --------------------------------------------------
# MSVC tuning + DIA SDK fix
# --------------------------------------------------
if (MSVC)

    target_compile_definitions(${CXX_EXE_NAME} PRIVATE
        _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
    )

    # Treat LLVM/Clang headers as external
    target_compile_options(${CXX_EXE_NAME} PRIVATE
        /external:I"${LLVM_INCLUDE_DIRS}"
        /external:I"${CLANG_INCLUDE_DIRS}"
        /external:W0
    )

    # ----------------------------------------
    # DIA SDK (needed for LLVM PDB support)
    # Fixes: diaguids.lib linker error in CI
    # ----------------------------------------
    set(DIA_PATHS
        "C:/Program Files (x86)/Microsoft Visual Studio/2022/Enterprise/DIA SDK/lib/amd64"
        "C:/Program Files (x86)/Microsoft Visual Studio/2022/Professional/DIA SDK/lib/amd64"
        "C:/Program Files (x86)/Microsoft Visual Studio/2022/Community/DIA SDK/lib/amd64"
    )

    foreach(p ${DIA_PATHS})
        if (EXISTS "${p}/diaguids.lib")
            target_link_directories(${CXX_EXE_NAME} PRIVATE "${p}")
            message(STATUS "Using DIA SDK lib path: ${p}")
            break()
        endif()
    endforeach()

endif()
