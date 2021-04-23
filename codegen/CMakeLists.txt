option(MNN_CODEGEN_LLVM "Build llvm backend for codegen." OFF)
option(MNN_CODEGEN_C "Build C source backend for codegen." OFF)
option(MNN_CODEGEN_OPENCL "Build OpenCL source backend for codegen." OFF)
option(MNN_CODEGEN_JIT "Build jit for codegen." OFF)

file(GLOB CODEGEN_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.*")
file(GLOB CPU_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/cpu/*.*")
file(GLOB JIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/jit/*.*")

add_library(MNNCodegen
    ${CODEGEN_HEADER}
    ${JIT_SRCS}
)

set_target_properties(MNNCodegen
PROPERTIES
    CXX_STANDARD 14
)

target_include_directories(MNNCodegen
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_compile_definitions(MNNCodegen
PUBLIC
    MNN_BUILD_CODEGEN
)

target_link_libraries(MNNCodegen
PUBLIC
    MNNCore
)

if(MNN_CODEGEN_OPENCL)
    target_compile_definitions(MNNCodegen PUBLIC MNN_CODEGEN_OPENCL)
    file(GLOB OPENCL_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/opencl/*.*")
    target_sources(MNNCodegen PRIVATE ${OPENCL_SRCS})
endif()

if(MNN_CODEGEN_C)
    target_compile_definitions(MNNCodegen PUBLIC MNN_CODEGEN_CPU MNN_CODEGEN_C)
    file(GLOB C_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/cpu/c/*.*")
    target_sources(MNNCodegen PRIVATE ${CPU_SRCS} ${C_SRCS})
endif()

if(MNN_CODEGEN_LLVM)
    target_compile_definitions(MNNCodegen PUBLIC MNN_CODEGEN_CPU MNN_CODEGEN_LLVM)
    file(GLOB LLVM_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/cpu/llvm/*.*")
    target_sources(MNNCodegen PRIVATE ${CPU_SRCS} ${LLVM_SRCS})
    # add llvm libs
    find_package(LLVM REQUIRED CONFIG)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    target_include_directories(MNN PUBLIC ${LLVM_INCLUDE_DIRS})
    target_compile_definitions(MNN PUBLIC ${LLVM_DEFINITIONS})

    llvm_map_components_to_libnames(llvm_libs core bitwriter OrcJIT Support nativecodegen native CodeGen)
    target_link_libraries(MNN PUBLIC ${llvm_libs})
endif()

install(TARGETS MNNCodegen
    EXPORT MNNConfig
    RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib
)
