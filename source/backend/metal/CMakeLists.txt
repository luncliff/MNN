FILE(GLOB MNN_Metal_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.mm ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
FILE(GLOB MNN_Metal_KERNELS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.metal)
message(STATUS "Generating mnn.metallib at ${CMAKE_CURRENT_BINARY_DIR}/mnn.metallib")
if(DEFINED SDK_VERSION)
    #Defined by iOS toolchain
    SET(METAL_SDK_PLAT "iphoneos")
else()
    SET(METAL_SDK_PLAT "macosx")
endif()
message(STATUS "Compiling Metal Kernels with ${METAL_SDK_PLAT} SDK")
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/mnn.metallib
    COMMAND xcrun -sdk ${METAL_SDK_PLAT} metal "${MNN_Metal_KERNELS_SRC}" -o ${CMAKE_CURRENT_BINARY_DIR}/mnn.metallib
    COMMAND_EXPAND_LISTS
)
add_custom_target(MNNMetalLIB
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/mnn.metallib
    COMMENT "Generating mnn.metallib"
)
add_library(MNNMetal ${MNN_Metal_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/MetalOPRegister.mm)
target_link_libraries(MNNMetal PUBLIC MNNCore)

# This is just work around some CMake limitations and is really ugly
# list(APPEND MNN_OBJECTS_TO_LINK ${CMAKE_CURRENT_BINARY_DIR}/mnn.metallib)

file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/MetalOPRegister.mm)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/MetalOPRegister.mm
    COMMAND ${PYTHON_EXECUTABLE}
        "${CMAKE_CURRENT_SOURCE_DIR}/MetalCodeGen.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/"
        "${CMAKE_CURRENT_SOURCE_DIR}/MetalOPRegister.mm"
    COMMENT "Metal Code Generation"
)
add_custom_target(MNNMetalCodeGen
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/MetalOPRegister.mm
    COMMENT "Registering MetalOps"
)
add_dependencies(MNNMetal MNNMetalCodeGen)
add_dependencies(MNNMetal MNNMetalLIB)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/mnn.metallib
PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mnn.metallib
        DESTINATION lib
)
install(TARGETS MNNMetal
        EXPORT MNNConfig
        RUNTIME LIBRARY DESTINATION lib ARCHIVE DESTINATION lib
)
