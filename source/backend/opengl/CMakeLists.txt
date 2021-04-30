FILE(GLOB MNN_OpenGL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
option(MNN_OPENGL_REGEN "Regenerate OpenGL Shaders." OFF)

if(MNN_SEP_BUILD)
    add_library(MNN_GL SHARED ${MNN_OpenGL_SRC} "${CMAKE_CURRENT_SOURCE_DIR}/AllShader.cpp")
else()
    add_library(MNN_GL ${MNN_OpenGL_SRC} "${CMAKE_CURRENT_SOURCE_DIR}/AllShader.cpp")
endif()
target_include_directories(MNN_GL PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/)

target_link_libraries(MNN_GL PRIVATE MNNCore)
if(ANDROID)
    target_link_libraries(MNN_GL PUBLIC GLESv3 EGL)
endif()
if(WIN32)
    target_compile_definitions(MNN_GL
    PRIVATE
        NOMINMAX
    )
    find_package(unofficial-angle CONFIG REQUIRED)
    target_link_libraries(MNN_GL
    PUBLIC
        unofficial::angle::libEGL unofficial::angle::libGLESv2
    )
endif()
if(MNN_ARM82)
    target_link_libraries(MNN_GL PUBLIC MNN_Arm82)
    if(TARGET MNNARM32) # for MNNPack
        target_link_libraries(MNN_GL PRIVATE MNNARM32)
    elseif(TARGET MNNARM64)
        target_link_libraries(MNN_GL PRIVATE MNNARM64)
    endif()
else()
    target_link_libraries(MNN_GL PRIVATE MNNCPU)
endif()

if(MNN_OPENGL_REGEN)
  add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/AllShader.cpp"
    COMMAND ${PYTHON_EXECUTABLE}
    "${CMAKE_CURRENT_SOURCE_DIR}/makeshader.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/glsl/"
    "${CMAKE_SOURCE_DIR}/include/MNN/backend/opengl/AllShader.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/AllShader.cpp"
    COMMENT "OpenGL Code Generation"
  )
  add_custom_target (MNN_GLCodeGen DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/AllShader.cpp")
  add_dependencies(MNN_GL MNN_GLCodeGen)
endif()
install(TARGETS MNN_GL
        EXPORT MNNConfig
        RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib
)
