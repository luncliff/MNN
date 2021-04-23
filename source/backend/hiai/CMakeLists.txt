file(GLOB_RECURSE MNN_NPU_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
if(MNN_SEP_BUILD)
        add_library(MNN_NPU SHARED ${MNN_NPU_SRCS})
        add_library(hiai SHARED IMPORTED )
        set_target_properties(hiai PROPERTIES
                IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/${ANDROID_ABI}/libhiai.so")

        add_library(hiai_ir SHARED IMPORTED )
        set_target_properties(hiai_ir PROPERTIES
                IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/${ANDROID_ABI}/libhiai_ir.so")

        add_library(hiai_ir_build SHARED IMPORTED )
        set_target_properties(hiai_ir_build PROPERTIES
                IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/${ANDROID_ABI}/libhiai_ir_build.so")

        target_link_libraries(MNN_NPU PRIVATE
                hiai hiai_ir hiai_ir_build
                MNN
        )
else()
        add_library(MNN_NPU  ${MNN_NPU_SRCS})
endif()
target_include_directories(MNN_NPU PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/backend/
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/include/
)

install(TARGETS MNN_NPU
        EXPORT MNNConfig
        RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib
)
