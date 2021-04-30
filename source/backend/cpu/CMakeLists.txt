# CPU
option(MNN_SUPPORT_BF16 "Enable MNN's bf16 op" OFF)

FILE(GLOB MNN_CPU_SRC ${CMAKE_CURRENT_SOURCE_DIR}/* ${CMAKE_CURRENT_SOURCE_DIR}/compute/*)
add_library(MNNCPU STATIC ${MNN_CPU_SRC})

target_link_libraries(MNNCPU PRIVATE MNNCore)

if(MNN_SUPPORT_BF16)
    add_subdirectory(bf16)
    target_compile_definitions(MNN_BF16 PUBLIC MNN_SUPPORT_BF16)
    target_link_libraries(MNNCPU PUBLIC MNN_BF16)
    # MNNCPU and MNNARM32 need to know flag MNN_SUPPORT_BF16
endif()
option(MNN_SSE_USE_FP16_INSTEAD "Use fp16 instead of bf16 for x86op" OFF)

# X86_64 AVX/SSE
if(MNN_USE_SSE)
    add_subdirectory(x86_x64)
    if(TARGET MNNAVX512)
        target_link_libraries(MNNCPU PRIVATE MNNAVX512)
    endif()
    if(TARGET MNNSSE)
        target_link_libraries(MNNCPU PRIVATE MNNX8664 MNNAVX MNNAVXFMA MNNSSE)
    endif()
endif()

# AArch32/64 Assemblies
add_subdirectory(arm)
if(TARGET MNNARM32)
    target_link_libraries(MNNCPU PRIVATE MNNARM32)
endif()
if(TARGET MNNARM64)
    target_link_libraries(MNNCPU PRIVATE MNNARM64)
endif()

# ARM82 Assemblies
if(MNN_ARM82)
    target_compile_definitions(MNNCPU PRIVATE ENABLE_ARMV82)
endif()

install(TARGETS MNNCPU
    EXPORT MNNConfig
    RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib
)
