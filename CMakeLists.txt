cmake_minimum_required(VERSION 3.8)
# Versioning stuff
if(NOT DEFINED MNN_VERSION_MAJOR)
  set(MNN_VERSION_MAJOR 0)
endif()
if(NOT DEFINED MNN_VERSION_MINOR)
  set(MNN_VERSION_MINOR 2)
endif()
if(NOT DEFINED MNN_VERSION_PATCH)
  set(MNN_VERSION_PATCH 1)
endif()
if(NOT DEFINED MNN_VERSION_BUILD)
  set(MNN_VERSION_BUILD 5)
endif()
if(NOT DEFINED MNN_VERSION_SUFFIX)
  set(MNN_VERSION_SUFFIX git)
endif()
if(NOT PACKAGE_VERSION)
  set(PACKAGE_VERSION
    "${MNN_VERSION_MAJOR}.${MNN_VERSION_MINOR}.${MNN_VERSION_PATCH}.${MNN_VERSION_BUILD}${MNN_VERSION_SUFFIX}")
endif()
add_compile_definitions(
  MNN_VERSION="${PACKAGE_VERSION}"
  MNN_VERSION_MAJOR=${MNN_VERSION_MAJOR}
  MNN_VERSION_MINOR=${MNN_VERSION_MINOR}
  MNN_VERSION_PATCH=${MNN_VERSION_PATCH}
)

# CMP0048 is related to letting CMake managing the package version for us

cmake_policy(SET CMP0048 NEW)
project(MNN VERSION ${MNN_VERSION_MAJOR}.${MNN_VERSION_MINOR}.${MNN_VERSION_PATCH}.${MNN_VERSION_BUILD} LANGUAGES C CXX ASM)
# complier options
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Required for OpenCL/OpenGL/Vulkan CodeGen
include(FindPythonInterp REQUIRED)
# build options
option(MNN_USE_SYSTEM_LIB "For opencl and vulkan, use system lib or use dlopen" OFF)
option(MNN_BUILD_HARD "Build -mfloat-abi=hard or not" OFF)
option(MNN_BUILD_SHARED_LIBS "MNN build shared or static lib" ON)
option(MNN_FORBID_MULTI_THREAD "Disable Multi Thread" OFF)
option(MNN_OPENMP "Use OpenMP's thread pool implementation. Does not work on iOS or Mac OS" OFF)
option(MNN_USE_THREAD_POOL "Use MNN's own thread pool implementation" ON)
option(MNN_BUILD_TRAIN "Build MNN's training framework" OFF)
option(MNN_BUILD_DEMO "Build demo/exec or not" OFF)
option(MNN_BUILD_TOOLS "Build tools/cpp or not" ON)
option(MNN_BUILD_QUANTOOLS "Build Quantized Tools or not" OFF)
option(MNN_EVALUATION "Build Evaluation Tools or not" OFF)
option(MNN_BUILD_CONVERTER "Build Converter" OFF)
option(MNN_SUPPORT_TFLITE_QUAN "Enable MNN's tflite quantized op" ON)
option(MNN_DEBUG_MEMORY "MNN Debug Memory Access" OFF)
option(MNN_DEBUG_TENSOR_SIZE "Enable Tensor Size" OFF)
option(MNN_GPU_TRACE "Enable MNN Gpu Debug" OFF)
option(MNN_PORTABLE_BUILD "Link the static version of third party libraries where possible to improve the portability of built executables" OFF)
option(MNN_SEP_BUILD "Build MNN Backends and expression seperately. Only works with MNN_BUILD_SHARED_LIBS=ON" ON)
option(NATIVE_LIBRARY_OUTPUT "Native Library Path" OFF)
option(NATIVE_INCLUDE_OUTPUT "Native Include Path" OFF)
option(MNN_AAPL_FMWK "Build MNN.framework instead of traditional .a/.dylib" OFF)
option(MNN_WITH_PLUGIN "Build with plugin op support." OFF)
option(MNN_BUILD_MINI "Build MNN-MINI that just supports fixed shape models." OFF)
option(MNN_USE_SSE "Use SSE optimization for x86 if possiable" ON)
option(MNN_BUILD_CODEGEN "Build with codegen" OFF)
option(MNN_ENABLE_COVERAGE "Build with coverage enable" OFF)

if(NOT MNN_BUILD_SHARED_LIBS AND MNN_SEP_BUILD)
  message(WARNING "Close MNN_SEP_BUILD for static library")
  SET(MNN_SEP_BUILD OFF CACHE BOOL "<docstring>" FORCE)
endif()
if(APPLE AND MNN_AAPL_FMWK AND MNN_SEP_BUILD)
  message(WARNING "MNN_SEP_BUILD AND MNN_AAPL_FMWK can't coexist. Turning off MNN_SEP_BUILD")
  SET(MNN_SEP_BUILD OFF CACHE BOOL "<docstring>" FORCE)
endif()
if(WIN32)
  if(MNN_SEP_BUILD)
    message(WARNING "MNN_SEP_BUILD IS TROUBLESOME ON Windows. Forcing OFF...")
    SET(MNN_SEP_BUILD OFF CACHE BOOL "<docstring>" FORCE)
  endif()
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)

  if(MSVC)
    add_compile_options(
      /wd4267 /wd4018 /wd4251 /wd4996 /wd4244 /wd4146 /wd4129 /wd4305 /wd4275 /wd4819
    )
  endif()
endif()

# for coverage test
if(MNN_ENABLE_COVERAGE)
  add_compile_options(
    -fprofile-arcs -ftest-coverage
  )
endif()

include(${PROJECT_SOURCE_DIR}/cmake/macros.cmake)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT MNN_BUILD_SHARED_LIBS AND NOT (MSVC OR WIN32))
  SET(MNN_SEP_BUILD OFF CACHE BOOL "<docstring>" FORCE)
  if(MNN_BUILD_CONVERTER)
    SET(MNN_PORTABLE_BUILD ON CACHE BOOL "<docstring>" FORCE)
  endif()
endif()


if(MNN_FORBID_MULTI_THREAD)
    add_compile_definitions(MNN_FORBIT_MULTI_THREADS)
endif()
if(MNN_SUPPORT_TFLITE_QUAN)
    add_compile_definitions(MNN_SUPPORT_TFLITE_QUAN)
endif()

# debug options
if(MNN_DEBUG_MEMORY)
    add_compile_definitions(MNN_DEBUG_MEMORY)
endif()
if(MNN_DEBUG_TENSOR_SIZE)
    add_compile_definitions(MNN_DEBUG_TENSOR_SIZE)
endif()
if(MNN_GPU_TRACE)
    add_compile_definitions(MNN_GPU_FORCE_FINISH)
endif()

# backend options
option(MNN_METAL "Enable Metal" OFF)
option(MNN_OPENCL "Enable OpenCL" OFF)
option(MNN_OPENGL "Enable OpenGL" OFF)
option(MNN_VULKAN "Enable Vulkan" OFF)
option(MNN_ARM82 "Enable ARM82" OFF)
option(MNN_ONEDNN "Enable oneDNN" OFF)
option(MNN_AVX512 "Enable AVX512" OFF)
option(MNN_CUDA "Enable CUDA" OFF)
option(MNN_TENSORRT "Enable TensorRT" OFF)

if(MNN_USE_THREAD_POOL)
    message(STATUS "Use Threadpool, forbid openmp")
    set(MNN_OPENMP OFF)
    add_compile_definitions(MNN_USE_THREAD_POOL)
endif()

# When build Android based on arm32 by MTL, force turn off MNN_ARM82
if(CMAKE_SYSTEM_NAME MATCHES "^Android" AND CMAKE_SYSTEM_PROCESSOR MATCHES "^armv7")
    message(STATUS "force turn off MNN_ARM82 when build for Android based on arm32 by MTL")
    SET(MNN_ARM82 OFF CACHE BOOL "Enable ARM82" FORCE)
endif()

# target options
option(MNN_BUILD_BENCHMARK "Build benchmark or not" OFF)
option(MNN_BUILD_TEST "Build tests or not" OFF)
option(MNN_USE_LOGCAT "Use Logcat intead of print for info" ON)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(MNN_HIDDEN FALSE)
else()
    set(MNN_HIDDEN TRUE)
endif()


message(STATUS ">>>>>>>>>>>>>")
message(STATUS "MNN BUILD INFO:")
message(STATUS "\tSystem: ${CMAKE_SYSTEM_NAME}")
message(STATUS "\tProcessor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "\tMetal: ${MNN_METAL}")
message(STATUS "\tOpenCL: ${MNN_OPENCL}")
message(STATUS "\tOpenGL: ${MNN_OPENGL}")
message(STATUS "\tVulkan: ${MNN_VULKAN}")
message(STATUS "\tARM82: ${MNN_ARM82}")
message(STATUS "\toneDNN: ${MNN_ONEDNN}")
message(STATUS "\tTensorRT: ${MNN_TENSORRT}")
message(STATUS "\tCUDA: ${MNN_CUDA}")
message(STATUS "\tOpenMP: ${MNN_OPENMP}")
message(STATUS "\tBF16: ${MNN_SUPPORT_BF16}")
message(STATUS "\tThreadPool: ${MNN_USE_THREAD_POOL}")
message(STATUS "\tHidden: ${MNN_HIDDEN}")
message(STATUS "\tBuild Path: ${CMAKE_CURRENT_BINARY_DIR}")

if(CMAKE_SYSTEM_NAME MATCHES "^Android" OR CMAKE_SYSTEM_NAME MATCHES "^Linux")
    set(CMAKE_POSITION_INDEPENDENT_CODE true)
endif()
if(CMAKE_SYSTEM_NAME MATCHES "^Android")
    add_compile_definitions(MNN_BUILD_FOR_ANDROID)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
        add_compile_options(-mfloat-abi=softfp -mfpu=neon)
    endif()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "^Linux")
    add_compile_definitions(__STRICT_ANSI__)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      # This is to workaround libgcc.a
      add_compile_options(-stdlib=libc++)
    endif()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^armv7")
      # please define in project/cross-compile/arm.toolchain.cmake
      add_compile_options(-mfpu=neon)
    endif()
    if(MNN_BUILD_HARD)
      # better define in project/cross-compile/arm.toolchain.cmake
      add_compile_options(-mfloat-abi=hard)
    endif()
endif()


if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(MNN_DEBUG DEBUG)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "^Android")
    if(MNN_USE_LOGCAT)
        add_compile_definitions(MNN_USE_LOGCAT)
    endif()
endif()

add_library(MNN INTERFACE)

if(MNN_BUILD_SHARED_LIBS)
  if(WIN32)
    add_compile_definitions(BUILDING_MNN_DLL USING_MNN_DLL)
    target_compile_definitions(MNN
    INTERFACE
      USING_MNN_DLL
    )
  endif()
endif()

find_package(Flatbuffers CONFIG REQUIRED)

# Core, CV, Math, Transform, Utils
FILE(GLOB MNN_Core_SRC ${CMAKE_CURRENT_LIST_DIR}/source/core/*)
FILE(GLOB MNN_CV_SRC ${CMAKE_CURRENT_LIST_DIR}/source/cv/*)
FILE(GLOB MNN_Math_SRC ${CMAKE_CURRENT_LIST_DIR}/source/math/*)
FILE(GLOB MNN_Transform_SRC ${CMAKE_CURRENT_LIST_DIR}/source/shape/* ${CMAKE_CURRENT_LIST_DIR}/source/geometry/*)
FILE(GLOB MNN_Utils_SRC ${CMAKE_CURRENT_LIST_DIR}/source/utils/*)

add_library(MNNCore STATIC
  ${MNN_Core_SRC} ${MNN_CV_SRC} ${MNN_Math_SRC} ${MNN_Transform_SRC} ${MNN_Utils_SRC}
)

target_include_directories(MNNCore
PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/schema/current>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/3rd_party/half>
)
target_link_libraries(MNNCore
PUBLIC
  flatbuffers::flatbuffers
)
if(ANDROID)
  target_link_libraries(MNNCore PUBLIC log)
endif()

if(MNN_BUILD_MINI)
    target_compile_definitions(MNNCore PRIVATE MNN_BUILD_MINI)
endif()
if(MNN_USE_SSE)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64)|(X86_64)|(x64)|(X64)|(amd64)|(AMD64)|(i686)")
        if(NOT MSVC)
            target_compile_options(MNNCore PRIVATE -msse4.1)
        endif()
    endif()
endif()

# target: MNNCPU, MNN_BF16 ...
add_subdirectory(source/backend/cpu)

# target: MNN_Arm82
if(MNN_ARM82)
    add_subdirectory(source/backend/arm82)
endif()

list(APPEND MNN_PUB_HDRS
  include/MNN/MNNDefine.h
  include/MNN/Interpreter.hpp
  include/MNN/HalideRuntime.h
  include/MNN/Tensor.hpp
  include/MNN/ErrorCode.hpp
  include/MNN/ImageProcess.hpp
  include/MNN/Matrix.h
  include/MNN/Rect.h
  include/MNN/MNNForwardType.h
  include/MNN/AutoTime.hpp
)
list(APPEND MNN_EXPR_PUB_HDRS
  include/MNN/expr/Expr.hpp
  include/MNN/expr/ExprCreator.hpp
  include/MNN/expr/MathOp.hpp
  include/MNN/expr/NeuralNetWorkOp.hpp
  include/MNN/expr/Optimizer.hpp
  include/MNN/expr/Executor.hpp
  include/MNN/expr/Module.hpp
  include/MNN/expr/NeuralNetWorkOp.hpp
)

if(NOT APPLE)
  if(MNN_OPENMP)
      find_package(Threads REQUIRED)
      find_package(OpenMP REQUIRED)
      target_link_libraries(MNN INTERFACE Threads::Threads OpenMP::OpenMP_CXX)
  endif()
endif()

if(NOT MSVC)
  add_compile_options(
      -fstrict-aliasing -ffunction-sections -fdata-sections -ffast-math -fno-rtti -fno-exceptions
  )
  if(MNN_HIDDEN)
    add_compile_options(
        -fvisibility-inlines-hidden -fvisibility=hidden
    )
  endif()
endif()

# Plugin
if(MNN_WITH_PLUGIN)
    add_compile_definitions(MNN_WITH_PLUGIN)
    add_subdirectory(source/plugin) # MNN_Plugin
endif()

# Metal
if(MNN_METAL AND APPLE)
    add_compile_definitions(MNN_METAL_ENABLED=1)
    add_subdirectory(source/backend/metal) # MNNMetal
endif()

# Vulkan
if(MNN_VULKAN)
    add_subdirectory(source/backend/vulkan) # MNN_Vulkan
endif()

# oneDNN
if(FALSE) # MNN_ONEDNN
    target_compile_definitions(MNNCPU PRIVATE MNN_USE_ONEDNN)
    add_dependencies(MNNCPU oneDNN)
    include(cmake/oneDNN.cmake)
    set(ONEDNN_DIR ${CMAKE_CURRENT_LIST_DIR}/3rd_party/oneDNN)
    add_library(ONEDNN_COMMON IMPORTED)
    file(GLOB_RECURSE OBJECT_FILES ${ONEDNN_DIR}/src/common/CMakeFiles/dnnl_common.dir/*.o)
    set_property(TARGET ONEDNN_COMMON PROPERTY IMPORTED_OBJECTS ${OBJECT_FILES})
    add_library(ONEDNN_CPU IMPORTED)
    file(GLOB_RECURSE OBJECT_FILES ${ONEDNN_DIR}/src/cpu/CMakeFiles/dnnl_cpu.dir/*.o)
    set_property(TARGET ONEDNN_CPU PROPERTY IMPORTED_OBJECTS ${OBJECT_FILES})
    add_library(ONEDNN_CPU_X64 IMPORTED)
    file(GLOB_RECURSE OBJECT_FILES ${ONEDNN_DIR}/src/cpu/x64/CMakeFiles/dnnl_cpu_x64.dir/*.o)
    set_property(TARGET ONEDNN_CPU_X64 PROPERTY IMPORTED_OBJECTS ${OBJECT_FILES})
    include_directories(${ONEDNN_DIR}/include)
    list(APPEND MNN_TARGETS ${ONEDNN_COMMON})
    list(APPEND MNN_TARGETS ${ONEDNN_CPU})
    list(APPEND MNN_TARGETS ${ONEDNN_CPU_X64})
    list(APPEND MNN_OBJECTS_TO_LINK $<TARGET_OBJECTS:ONEDNN_COMMON>)
    list(APPEND MNN_OBJECTS_TO_LINK $<TARGET_OBJECTS:ONEDNN_CPU>)
    list(APPEND MNN_OBJECTS_TO_LINK $<TARGET_OBJECTS:ONEDNN_CPU_X64>)
    add_dependencies(MNN ONEDNN_COMMON ONEDNN_CPU ONEDNN_CPU_X64)
endif()

# OpenCL
if(MNN_OPENCL)
  find_package(OpenCL REQUIRED)
  add_subdirectory(source/backend/opencl) # MNN_CL
  target_link_libraries(MNN INTERFACE MNN_CL)
endif()

# OpenGL
if(MNN_OPENGL)
  add_subdirectory(source/backend/opengl) # MNN_GL
  target_link_libraries(MNN INTERFACE MNN_GL)
endif()

# CUDA
if(MNN_CUDA)
  add_subdirectory(source/backend/cuda) # MNN_CUDA
  target_link_libraries(MNN INTERFACE MNN_CUDA)
endif()

# Express
add_subdirectory(express) # MNN_Express
target_link_libraries(MNN INTERFACE MNN_Express)

find_package(RapidJSON CONFIG REQUIRED)
# include_directories(
#                     # ${CMAKE_CURRENT_LIST_DIR}/tools/
#                     ${RAPIDJSON_INCLUDE_DIRS}
#                   )

# Train
if(MNN_BUILD_TRAIN)
  find_path(STB_INCLUDE_DIRS "stb.h")
  add_subdirectory(tools/train) # MNNTrain
  target_include_directories(MNNTrain PRIVATE ${STB_INCLUDE_DIRS})
  target_compile_definitions(MNNTrain PRIVATE STB_IMAGE_STATIC STB_IMAGE_IMPLEMENTATION)
endif()

#CodeGen
if(MNN_BUILD_CODEGEN) # requires llvm
    add_subdirectory(codegen)
    target_link_libraries(MNNCore PUBLIC MNNCodegen)
endif()

# NPU
if(FALSE) # MNN_NPU
    add_subdirectory(source/backend/hiai)
    if(MNN_SEP_BUILD)
        target_link_libraries(MNN INTERFACE MNN_NPU)
    else()
        # todo
        list(APPEND MNN_EXTRA_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/source/backend/hiai/3rdParty/${ANDROID_ABI}/libhiai.so)
        list(APPEND MNN_EXTRA_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/source/backend/hiai/3rdParty/${ANDROID_ABI}/libhiai_ir_build.so)
        list(APPEND MNN_EXTRA_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/source/backend/hiai/3rdParty/${ANDROID_ABI}/libhiai_ir.so)
        target_link_libraries(MNN INTERFACE ${MNN_EXTRA_DEPENDS})
    endif()
endif()

# TensorRT
if(MNN_TENSORRT)
  add_subdirectory(source/backend/tensorrt) # MNN_TRT
  target_link_libraries(MNN INTERFACE MNN_TRT)
endif()

target_sources(MNN PRIVATE ${MNN_PUB_HDRS} ${MNN_EXPR_PUB_HDRS})

set_target_properties(MNN
PROPERTIES
    PUBLIC_HEADER "${MNN_PUB_HDRS}"
)

if(MSVC)
  target_link_options(MNN PRIVATE "/IGNORE:4049,4217")
endif()

if(APPLE)
    if(MNN_AAPL_FMWK)
      set_target_properties(MNN PROPERTIES FRAMEWORK TRUE)
      set_target_properties(MNN PROPERTIES
          MACOSX_FRAMEWORK_IDENTIFIER com.alibaba.MNN
          MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${PACKAGE_VERSION}
          MACOSX_FRAMEWORK_BUNDLE_VERSION ${PACKAGE_VERSION}
          XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
      )
      set_target_properties(MNN PROPERTIES MACOSX_FRAMEWORK_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/project/ios/MNN/Info.plist)
    endif()
    if(MNN_METAL)
      find_library(FOUNDATION Foundation REQUIRED)
      target_link_libraries(MNN INTERFACE ${FOUNDATION})
      find_library(METAL Metal REQUIRED)
      target_link_libraries(MNN INTERFACE ${METAL})
      find_library(GRAPHIC CoreGraphics)
      target_link_libraries(MNN INTERFACE ${GRAPHIC})
    endif()
endif()
target_link_libraries(MNN INTERFACE MNNCore MNNCPU)

if(CMAKE_SYSTEM_NAME MATCHES "^Linux")
  # Using -pthread, needed by thread-safe implemention of glibc, is better than only using -lpthread
  # https://stackoverflow.com/questions/23250863/difference-between-pthread-and-lpthread-while-compiling
  target_link_libraries(MNN INTERFACE -pthread dl)
elseif(CMAKE_SYSTEM_NAME MATCHES "^Android")
  target_link_libraries(MNN INTERFACE log m)
else()
  # ...
endif()

# if(MNN_BUILD_CONVERTER)
#   add_subdirectory(tools/converter)
# endif()
# if(MNN_BUILD_DEMO)
#   add_subdirectory(demo/exec/CMakeLists.txt)
# endif()
# if(MNN_BUILD_TOOLS)
#   add_subdirectory(tools/cpp/CMakeLists.txt)
# endif()
# if(MNN_BUILD_TEST)
#   add_subdirectory(test/CMakeLists.txt)
# endif()
# if(MNN_BUILD_BENCHMARK)
#   add_subdirectory(benchmark/CMakeLists.txt)
# endif()
# if(MNN_BUILD_QUANTOOLS)
#   add_subdirectory(tools/quantization/CMakeLists.txt)
# endif()
# if(MNN_EVALUATION)
#   add_subdirectory(tools/evaluation/CMakeLists.txt)
# endif()

# Install
include(CMakePackageConfigHelpers)
set(VERSION_FILE_PATH ${CMAKE_BINARY_DIR}/cmake/MNNConfig-version.cmake)
write_basic_package_version_file(${VERSION_FILE_PATH}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion
)
install(FILES ${VERSION_FILE_PATH} 
        DESTINATION lib/cmake/MNN
)

install(FILES ${MNN_EXPR_PUB_HDRS} DESTINATION include/MNN/expr)
install(FILES ${MNN_EXTRA_DEPENDS} DESTINATION lib)

install(TARGETS MNN MNNCore
    EXPORT MNNConfig
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/MNN
    FRAMEWORK DESTINATION Library/Frameworks/
)
install(EXPORT MNNConfig
        NAMESPACE unofficial::
        DESTINATION lib/cmake/MNN
)

if(APPLE)
  FOREACH(HDR ${MNN_EXPR_PUB_HDRS})
    SET_SOURCE_FILES_PROPERTIES(${HDR} PROPERTIES MACOSX_PACKAGE_LOCATION Headers/expr/ )
  ENDFOREACH()
  FOREACH(HDR ${MNN_PUB_HDRS})
    SET_SOURCE_FILES_PROPERTIES(${HDR} PROPERTIES MACOSX_PACKAGE_LOCATION Headers/ )
  ENDFOREACH()
endif()
