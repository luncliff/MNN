if(NOT DEFINED ARCHS)
  set(ARCHS ${CMAKE_SYSTEM_PROCESSOR})
endif()
FILE(GLOB MNN_AArch32_SRC ${CMAKE_CURRENT_SOURCE_DIR}/arm32/*.[sS])
FILE(GLOB MNN_AArch64_SRC ${CMAKE_CURRENT_SOURCE_DIR}/arm64/*.[sS])

FILE(GLOB MNN_NEON_SRC ${CMAKE_CURRENT_SOURCE_DIR}/CommonOptFunctionNeon.cpp)
if(MNN_SUPPORT_BF16)
    FILE(GLOB MNN_NEON_SRC ${MNN_NEON_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/CommonNeonBF16.cpp)
else()
    LIST(FILTER MNN_AArch32_SRC EXCLUDE REGEX ".*BF16.*")
    LIST(FILTER MNN_AArch64_SRC EXCLUDE REGEX ".*BF16.*")
endif()

# remove the armv82 extension assemblies file
if(NOT MNN_ARM82)
    list(REMOVE_ITEM MNN_AArch64_SRC ${CMAKE_CURRENT_SOURCE_DIR}/arm64/MNNGemmInt8AddBiasScale_ARMV82_Unit.S)
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^armv7" OR ARCHS MATCHES "^armv7(;armv7s)?")
    message(STATUS "Enabling AArch32 Assemblies")
    add_library(MNNARM32 ${MNN_AArch32_SRC} ${MNN_NEON_SRC})
    target_include_directories(MNNARM32 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/)
    target_compile_definitions(MNNARM32 PUBLIC MNN_USE_NEON)

    if(MNN_SUPPORT_BF16)
        target_link_libraries(MNNARM32 PUBLIC MNN_BF16)
    endif()

    install(TARGETS MNNARM32 
        RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib
    )
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64" OR ARCHS STREQUAL "arm64")
    message(STATUS "Enabling AArch64 Assemblies")
    add_library(MNNARM64 ${MNN_AArch64_SRC} ${MNN_NEON_SRC})
    target_include_directories(MNNARM64 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/)
    target_compile_definitions(MNNARM64 PUBLIC MNN_USE_NEON)

    if(MNN_ARM82)
        message(STATUS "Enable INT8 SDOT")
        target_compile_options(MNNARM64 PRIVATE -march=armv8.2-a+dotprod)
        target_compile_definitions(MNNARM64 PUBLIC ENABLE_ARMV82)
    endif()
    install(TARGETS MNNARM64
        EXPORT MNNConfig
        RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib
    )
else()
    # Building fat binary requires multiple seperate builds and lipo-by-hand under CMake's design
endif()
