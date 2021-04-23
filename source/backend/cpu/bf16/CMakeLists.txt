
file(GLOB MNN_BF16_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*")
file(GLOB MNN_BF16_SRCS_ASM "${CMAKE_CURRENT_SOURCE_DIR}/asm/*")

add_library(MNN_BF16 ${MNN_BF16_SRCS})

target_link_libraries(MNN_BF16 PUBLIC MNNCore)

if(MNN_USE_SSE)
    if(MNN_SSE_USE_FP16_INSTEAD)
        target_compile_definitions(MNN_BF16 PRIVATE MNN_SSE_USE_FP16_INSTEAD)
        target_compile_options(MNN_BF16 PRIVATE -mf16c)
    endif()
endif()

install(TARGETS MNN_BF16
    EXPORT MNNConfig
    RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib
)
