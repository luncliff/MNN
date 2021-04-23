FILE(GLOB MNN_OpenGL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
option(MNN_OPENGL_REGEN "Regenerate OpenGL Shaders." OFF)

if(MNN_SEP_BUILD)
    add_library(MNN_GL SHARED ${MNN_OpenGL_SRC} "${CMAKE_CURRENT_SOURCE_DIR}/AllShader.cpp")
    target_link_libraries(MNN_GL MNN GLESv3 EGL)
else()
    add_library(MNN_GL ${MNN_OpenGL_SRC} "${CMAKE_CURRENT_SOURCE_DIR}/AllShader.cpp")
endif()
target_include_directories(MNN_GL PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/)
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