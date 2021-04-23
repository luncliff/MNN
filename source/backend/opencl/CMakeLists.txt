#add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/execution/cl/opencl_program.cc"
#    COMMAND ${PYTHON_EXECUTABLE}
#    "${CMAKE_CURRENT_SOURCE_DIR}/execution/cl/opencl_codegen.py"
#    "${CMAKE_CURRENT_SOURCE_DIR}/execution/cl/"
#    "${CMAKE_CURRENT_SOURCE_DIR}/execution/cl/opencl_program.cc"
#    COMMENT "OpenCL Code Generation"
#)
#add_custom_target (MNN_CLCodeGen DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/execution/cl/opencl_program.cc")
file(GLOB_RECURSE MNN_OpenCL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*)

option(MNN_OPENCL_SIZE_CUT "Disable MNN OpenCL Buffer Opt" OFF)
option(MNN_OPENCL_PROFILE "Enable MNN OpenCL Kernel Profile" OFF)

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    add_compile_definitions(MNN_USE_LIB_WRAPPER)
    add_compile_definitions(CL_HPP_TARGET_OPENCL_VERSION=110)
else()
    if(MNN_USE_SYSTEM_LIB)
        find_package(OpenCL REQUIRED)
    else()
        add_compile_definitions(MNN_USE_LIB_WRAPPER)
        add_compile_definitions(CL_HPP_TARGET_OPENCL_VERSION=110)
    endif()
endif()
if(MNN_SEP_BUILD)
    add_library(MNN_CL SHARED ${MNN_OpenCL_SRC} "${CMAKE_CURRENT_SOURCE_DIR}/execution/cl/opencl_program.cc")
    target_link_libraries(MNN_CL PRIVATE MNN PUBLIC ${OpenCL_LIBRARIES})
else()
    add_library(MNN_CL ${MNN_OpenCL_SRC} "${CMAKE_CURRENT_SOURCE_DIR}/execution/cl/opencl_program.cc")
endif()

target_include_directories(MNN_CL
PUBLIC
    ${OpenCL_INCLUDE_DIRS}
PRIVATE
    schema/current
)
target_link_libraries(MNN_CL
PUBLIC
    MNNCore ${OpenCL_LIBRARIES}
)

if(MNN_OPENCL_PROFILE)
    target_compile_definitions(MNN_CL PRIVATE ENABLE_OPENCL_TIME_PROFILER)
endif()
if(MNN_OPENCL_SIZE_CUT)
    target_compile_definitions(MNN_CL PUBLIC MNN_OPENCL_BUFFER_CLOSED)
endif()

install(TARGETS MNN_CL
        EXPORT MNNConfig
        RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib
)
