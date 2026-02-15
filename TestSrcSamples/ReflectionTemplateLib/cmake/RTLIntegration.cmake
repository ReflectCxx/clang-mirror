# ----------------------------------------
# Attach generated registration to target
# ----------------------------------------

function(rtl_attach_registration TARGET CLANG_MR_DIR)

    if(NOT EXISTS "${CLANG_MR_DIR}")
        message(FATAL_ERROR
            "RTL ERROR: Registration directory not found: ${CLANG_MR_DIR}\n"
            "Make sure clang-mirror generated files exist."
        )
    endif()

    if(NOT EXISTS "${CLANG_MR_DIR}/reg_src")
        message(FATAL_ERROR
            "RTL ERROR: Registration source directory not found: ${CLANG_MR_DIR}/reg_src"
        )
    endif()

    file(GLOB RTL_REG_SRCS
        "${CLANG_MR_DIR}/reg_src/*.cpp"
    )

    if(NOT RTL_REG_SRCS)
        message(FATAL_ERROR
            "RTL ERROR: No registration source files found in ${CLANG_MR_DIR}/reg_src"
        )
    endif()

    file(GLOB RTL_REG_HDRS
        "${CLANG_MR_DIR}/*.h"
    )

    target_sources(${TARGET} PRIVATE
        ${RTL_REG_SRCS}
        ${RTL_REG_HDRS}
    )

    target_include_directories(${TARGET} PRIVATE
        "${CLANG_MR_DIR}"
    )

    set_source_files_properties(${RTL_REG_SRCS}
        PROPERTIES GENERATED TRUE
    )

    source_group("RTLRegistration" FILES
        ${RTL_REG_SRCS}
        ${RTL_REG_HDRS}
    )

    message(STATUS "RTL: Attached registration from ${CLANG_MR_DIR}")

endfunction()

# -------------------
# Public entry point 
# -------------------

function(rtl_enable TARGET)

    # Link RTL runtime
    target_link_libraries(${TARGET}
        PRIVATE RTL::ReflectionTemplateLib
    )

    # Get the directory where the target was defined
    get_target_property(TARGET_SOURCE_DIR ${TARGET} SOURCE_DIR)

    if(NOT TARGET_SOURCE_DIR)
        message(FATAL_ERROR
            "RTL ERROR: Could not determine source directory for target ${TARGET}"
        )
    endif()

    # Construct mirror path relative to target
    set(MIRROR_DIR "${TARGET_SOURCE_DIR}/clang-mr")

    rtl_attach_registration(${TARGET} "${MIRROR_DIR}")

endfunction()