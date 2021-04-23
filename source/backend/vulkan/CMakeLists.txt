FILE(GLOB_RECURSE MNN_Vulkan_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*)

if(MNN_USE_SYSTEM_LIB)
    find_package(Vulkan REQUIRED)
endif()
if(MNN_SEP_BUILD)
    add_library(MNN_Vulkan SHARED ${MNN_Vulkan_SRC})
    target_link_libraries(MNN_Vulkan PUBLIC ${Vulkan_LIBRARIES} PRIVATE MNN)
    if(MNN_USE_SYSTEM_LIB)
        target_link_libraries(MNN_Vulkan ${Vulkan_LIBRARY})
    elseif(NOT WIN32)
        target_link_libraries(MNN_Vulkan ${CMAKE_DL_LIBS})
        target_compile_definitions(MNN_Vulkan PRIVATE MNN_USE_LIB_WRAPPER)
    endif()
else()
    add_library(MNN_Vulkan ${MNN_Vulkan_SRC})
    if(NOT WIN32 AND NOT MNN_USE_SYSTEM_LIB)
        target_compile_definitions(MNN_Vulkan PRIVATE MNN_USE_LIB_WRAPPER)
    endif()
endif()

target_include_directories(MNN_Vulkan
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/component
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/execution
    ${CMAKE_CURRENT_SOURCE_DIR}/backend
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
)
target_link_libraries(MNN_Vulkan
PRIVATE
    MNNCore
)

if(CMAKE_SYSTEM_NAME MATCHES "^Android")
  target_compile_definitions(MNN_Vulkan PUBLIC VK_USE_PLATFORM_ANDROID_KHR)
endif()
install(TARGETS MNN_Vulkan
        EXPORT MNNConfig
        RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib
)
