option(TF_CONVERT_ORIGIN "Fall Back to Origin Model Converter" OFF)
option(TFMODEL_OPTIMIZE "Enable tensorflow model optimizer" OFF)
option(MNN_BUILD_TORCHSCRIPT "Build Converter support TorchScript." OFF)

find_package(Protobuf CONFIG REQUIRED)

file(GLOB COMMON_SRC ${CMAKE_CURRENT_SOURCE_DIR}/source/common/*.cpp)

add_library(MNNConvertDeps STATIC ${COMMON_SRC} ${MNN_CONVERTER_BACKENDS_OBJECTS})

target_include_directories(MNNConvertDeps
PUBLIC
  ${PROJECT_SOURCE_DIR}/tools/
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
  ${CMAKE_CURRENT_SOURCE_DIR}/source/tflite/schema/
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(MNNConvertDeps
PUBLIC
  protobuf::libprotobuf
  MNNCore ${MNN_CONVERTER_TS_LIBS}
)

if(NOT MSVC)
  target_compile_options(MNNConvertDeps
  PUBLIC
    -fexceptions
  )
endif()

SET(MNN_CONVERTER_BACKENDS_TARGETS "")
SET(MNN_CONVERTER_BACKENDS_OBJECTS "")
SET(MNN_CONVERTER_TS_LIBS "")

# include(${CMAKE_CURRENT_SOURCE_DIR}/source/compression/CMakeLists.txt)
file(GLOB CALIBRATION_SRC ${CMAKE_CURRENT_SOURCE_DIR}/source/compression/*.cpp)
protobuf_generate_cpp(MNN_COMPRESSION_PROTO_SRCS MNN_COMPRESSION_PROTO_HDRS
  ${CMAKE_CURRENT_SOURCE_DIR}/source/compression/MNN_compression.proto
)

message(WARNING "? ${MNN_COMPRESSION_PROTO_HDRS} ${CMAKE_CURRENT_BINARY_DIR}")
add_library(MNNCompress STATIC
  ${CALIBRATION_SRC}
  ${MNN_COMPRESSION_PROTO_SRCS} ${MNN_COMPRESSION_PROTO_HDRS}
)

target_include_directories(MNNCompress
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/source/compression
  ${CMAKE_CURRENT_BINARY_DIR}/source/compression
  ${CMAKE_CURRENT_BINARY_DIR}
)

# todo
# include(${CMAKE_CURRENT_SOURCE_DIR}/source/tensorflow/CMakeLists.txt)
# include(${CMAKE_CURRENT_SOURCE_DIR}/source/onnx/CMakeLists.txt)
# include(${CMAKE_CURRENT_SOURCE_DIR}/source/caffe/CMakeLists.txt)
# include(${CMAKE_CURRENT_SOURCE_DIR}/source/MNN/CMakeLists.txt)
# include(${CMAKE_CURRENT_SOURCE_DIR}/source/optimizer/CMakeLists.txt)
# include(${CMAKE_CURRENT_SOURCE_DIR}/source/tflite/CMakeLists.txt)

if(MNN_BUILD_TORCHSCRIPT)
  target_compile_definitions(MNNConvertDeps PUBLIC MNN_BUILD_TORCHSCRIPT)
  # include(${CMAKE_CURRENT_SOURCE_DIR}/source/torchscript/CMakeLists.txt)
endif()


add_executable(MNNDump2Json
  ${CMAKE_CURRENT_SOURCE_DIR}/source/MNNDump2Json.cpp
)
target_link_libraries(MNNDump2Json PRIVATE MNNCore)

add_executable(MNNConvert
  ${CMAKE_CURRENT_SOURCE_DIR}/source/MNNConverter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/cli.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/config.cpp
)
target_link_libraries(MNNConvert PRIVATE MNNConvertDeps MNNCompress)

add_executable(TestConvertResult
  ${CMAKE_CURRENT_SOURCE_DIR}/source/TestConvertResult.cpp
)
target_link_libraries(TestConvertResult PRIVATE MNNConvertDeps)

add_executable(TestPassManager
  ${CMAKE_CURRENT_SOURCE_DIR}/source/TestPassManager.cpp
)
target_link_libraries(TestPassManager PRIVATE MNNConvertDeps)

install(TARGETS MNNDump2Json MNNConvert
    RUNTIME DESTINATION bin
)
