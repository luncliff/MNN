file(GLOB_RECURSE MNN_EXPR_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.*")

option(MNN_EXPR_ENABLE_PROFILER "Support profile Expr's op cost" OFF)
option(MNN_EXPR_SHAPE_EAGER "Force compute Expr's shape directly cost" OFF)

add_library(MNN_Express ${MNN_EXPR_SRCS})

target_include_directories(MNN_Express
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(MNN_Express PUBLIC MNNCore)

if(MNN_EXPR_ENABLE_PROFILER)
    target_compile_definitions(MNN_Express PRIVATE MNN_EXPR_ENABLE_PROFILER)
endif()
if(MNN_EXPR_SHAPE_EAGER)
    target_compile_definitions(MNN_Express PUBLIC MNN_EXPR_SHAPE_EAGER)
endif()

install(TARGETS MNN_Express
        EXPORT MNNConfig
        RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib
)
