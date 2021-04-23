
file(GLOB MNN_TRT_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/backend/* ${CMAKE_CURRENT_SOURCE_DIR}/execution/*)
file(GLOB CUDA_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/execution/plugin/*)

find_package(CUDA REQUIRED)

include(${PROJECT_SOURCE_DIR}/source/backend/cuda/SelectCudaComputeArch.cmake)
CUDA_SELECT_NVCC_ARCH_FLAGS(CUDA_ARCH_FLAGS ${CUDA_ARCHS})
set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} ${CUDA_ARCH_FLAGS}")
# Fix for some combinations of CUDA and GCC (e.g. under Ubuntu 16.04).
set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -D_FORCE_INLINES -Wno-deprecated-gpu-targets -w ")
message(STATUS "Enabling CUDA support (version: ${CUDA_VERSION_STRING},"
                " archs: ${CUDA_ARCH_FLAGS_readable})")
message(STATUS "Using CUDA_NVCC_FLAGS: ${CUDA_NVCC_FLAGS}")

cuda_add_library(MNN_TRT ${MNN_TRT_SRCS} ${CUDA_SRCS})

set_target_properties(MNN_TRT
PROPERTIES
    CXX_STANDARD 14
    CUDA_STANDARD 14
)

target_include_directories(MNN_TRT
PUBLIC
    ${CUDA_INCLUDE_DIRS}
PRIVATE
    backend
    execution
    execution/plugin
)
target_link_libraries(MNN_TRT MNNCore ${CUDA_CUDA_LIBRARY})

option(MNN_TRT_DYNAMIC "use dlopen to find trt" OFF)

if(MNN_TRT_DYNAMIC)
    target_compile_definitions(MNN_TRT PUBLIC MNN_TRT_DYNAMIC)
else()
    target_link_libraries(MNN_TRT nvinfer)
endif()
install(TARGETS MNN_TRT
        EXPORT MNNConfig
        RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib
)
