# see https://github.com/microsoft/vcpkg/blob/master/ports/cudnn/FindCUDNN.cmake
set(CUDA_MIN_VERSION "9.0")
find_package(CUDA ${CUDA_MIN_VERSION} REQUIRED)
find_package(CUDNN REQUIRED)
message(STATUS "Using CUDNN: ${CUDNN_LIBRARIES}")

if(WIN32)
    find_library(CUBLAS_LIB NAMES cublas PATHS $ENV{CUDA_PATH}/lib/x64)
else()
    find_library(CUBLAS_LIB cublas)
endif()
if(NOT CUBLAS_LIB)
    message(FATAL_ERROR "cuBLAS not found")
else()
    message(STATUS "Using cuBLAS: ${CUBLAS_LIB}")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/SelectCudaComputeArch.cmake)
CUDA_SELECT_NVCC_ARCH_FLAGS(CUDA_ARCH_FLAGS ${CUDA_ARCHS})
set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} ${CUDA_ARCH_FLAGS}")
# Fix for some combinations of CUDA and GCC (e.g. under Ubuntu 16.04).
set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -D_FORCE_INLINES -Wno-deprecated-gpu-targets -w ")
message(STATUS "Enabling CUDA support (version: ${CUDA_VERSION_STRING},"
                " archs: ${CUDA_ARCH_FLAGS_readable})")
message(STATUS "Using CUDA_NVCC_FLAGS: ${CUDA_NVCC_FLAGS}")

file(GLOB_RECURSE MNN_CUDA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/core/* ${CMAKE_CURRENT_SOURCE_DIR}/execution/*)

cuda_add_library(MNN_CUDA ${MNN_CUDA_SRC} Register.cpp)

set_target_properties(MNN_CUDA
PROPERTIES
    CXX_STANDARD 14
    CUDA_STANDARD 14
)

target_include_directories(MNN_CUDA
PUBLIC
    ${CUDA_INCLUDE_DIRS}
PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(MNN_CUDA
    MNNCore MNNCPU
    CuDNN::CuDNN ${CUBLAS_LIB}
)

install(TARGETS MNN_CUDA
    EXPORT MNNConfig
    RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib
)
