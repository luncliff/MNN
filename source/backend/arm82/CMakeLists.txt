
file(GLOB MNN_ARM82_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/compute/*.cpp")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^armv7" OR ARCHS MATCHES "^armv7(;armv7s)?")
    file(GLOB MNN_ARM82_SRCS_ASM "${CMAKE_CURRENT_SOURCE_DIR}/asm/arm32/*")
    add_library(MNN_Arm82 ${MNN_ARM82_SRCS} ${MNN_ARM82_SRCS_ASM})
    target_compile_options(MNN_Arm82
    PRIVATE
        -march=armv8.2-a+fp16 -mfpu=neon-fp-armv8 -mfloat-abi=softfp
    )
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64" OR ARCHS STREQUAL "arm64")
    file(GLOB MNN_ARM82_SRCS_ASM "${CMAKE_CURRENT_SOURCE_DIR}/asm/arm64/*")
    add_library(MNN_Arm82 ${MNN_ARM82_SRCS} ${MNN_ARM82_SRCS_ASM})
    target_compile_options(MNN_Arm82
    PRIVATE
        -march=armv8.2-a+fp16
    )
else()
    # Building fat binary requires multiple seperate builds and lipo-by-hand under CMake's design
    message(FATAL_ERROR "${CMAKE_SYSTEM_PROCESSOR} ${ARCHS}")
endif()

target_include_directories(MNN_Arm82
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/compute/
    ${CMAKE_CURRENT_SOURCE_DIR}/asm/
)
target_link_libraries(MNN_Arm82 PRIVATE MNNCore MNNCPU)
if(ANDROID)
    target_link_libraries(MNN_Arm82 PUBLIC log)
endif()

install(TARGETS MNN_Arm82
    EXPORT MNNConfig
    RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib
)
