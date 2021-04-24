option(MNN_TRAIN_DEBUG "Enable MNN Train Grad Debug" OFF)
option(MNN_BUILD_TRAIN_MINI "Don't build dataset and models" OFF)
option(MNN_USE_OPENCV "Use opencv" OFF)

if(MNN_SEP_BUILD)
    add_library(MNNTrain SHARED ${MNN_TRAIN_SRCS})
else()
    add_library(MNNTrain ${MNN_TRAIN_SRCS})
endif()
target_link_libraries(MNNTrain PUBLIC MNNCore MNNCPU MNN_Express)

target_include_directories(MNNTrain
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source/grad>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source/optimizer>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source/transformer>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source/data>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source/nn>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source/models>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source/datasets>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/tools>
)

file(GLOB GRAD ${CMAKE_CURRENT_SOURCE_DIR}/source/grad/*)
file(GLOB TRANSFORMER ${CMAKE_CURRENT_SOURCE_DIR}/source/transformer/*)
file(GLOB OPTIMIZER ${CMAKE_CURRENT_SOURCE_DIR}/source/optimizer/*)
file(GLOB DATALOADER ${CMAKE_CURRENT_SOURCE_DIR}/source/data/*)
file(GLOB MODELS ${CMAKE_CURRENT_SOURCE_DIR}/source/models/*)
file(GLOB NNFILES ${CMAKE_CURRENT_SOURCE_DIR}/source/nn/*)
file(GLOB DATASETS ${CMAKE_CURRENT_SOURCE_DIR}/source/datasets/*)

# MNNTrain
if(MNN_TRAIN_DEBUG)
    target_compile_definitions(MNNTrain PRIVATE MNN_TRAIN_DEBUG)
endif()

target_sources(MNNTrain
PRIVATE
    ${GRAD} ${BASIC_INCLUDE} ${OPTIMIZER} ${DATALOADER} ${TRANSFORMER} ${NNFILES}
)
if(NOT MNN_BUILD_TRAIN_MINI)
    target_sources(MNNTrain PRIVATE ${MODELS} ${DATASETS})
endif()

# executables
set(MNN_TRAIN_TOOLS "")
add_executable(transformer.out ${CMAKE_CURRENT_SOURCE_DIR}/source/exec/transformerExecution.cpp)
add_executable(train.out ${CMAKE_CURRENT_SOURCE_DIR}/source/exec/train.cpp ${SCHEMA} ${BASIC_INCLUDE})
add_executable(rawDataTransform.out ${CMAKE_CURRENT_SOURCE_DIR}/source/exec/rawDataTransform.cpp ${SCHEMA} ${BASIC_INCLUDE})
add_executable(dataTransformer.out ${CMAKE_CURRENT_SOURCE_DIR}/source/exec/dataTransformer.cpp ${SCHEMA} ${BASIC_INCLUDE})
file(GLOB DEMOSOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source/demo/*)

add_executable(runTrainDemo.out ${DEMOSOURCE} ${BASIC_INCLUDE})
if(MNN_BUILD_TRAIN_MINI)
    target_sources(runTrainDemo.out PRIVATE ${DATASETS} ${MODELS})
endif()
target_include_directories(runTrainDemo.out PRIVATE ${STB_INCLUDE_DIRS})

list(APPEND MNN_TRAIN_TOOLS transformer.out)
list(APPEND MNN_TRAIN_TOOLS train.out)
list(APPEND MNN_TRAIN_TOOLS rawDataTransform.out)
list(APPEND MNN_TRAIN_TOOLS dataTransformer.out)
list(APPEND MNN_TRAIN_TOOLS runTrainDemo.out)
foreach(TARGET ${MNN_TRAIN_TOOLS})
    target_link_libraries(${TARGET} PRIVATE MNNTrain)
    if(MSVC)
        target_compile_definitions(${TARGET} PRIVATE _CRT_SECURE_NO_WARNINGS)
        if(NOT MNN_BUILD_SHARED_LIBS)
            foreach (DEPEND ${MNN_DEPS})
                target_link_options(${TARGET} PRIVATE /WHOLEARCHIVE:$<TARGET_FILE:${DEPEND}>)
            endforeach ()
            if(MNN_SEP_BUILD)
                target_link_options(${TARGET} PRIVATE /WHOLEARCHIVE:$<TARGET_FILE:MNNTrain>)
            endif()
        endif()
    endif()
endforeach()

if(MNN_USE_OPENCV)
    find_package(OpenCV CONFIG REQUIRED)
    target_include_directories(runTrainDemo.out PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_compile_definitions(runTrainDemo.out PRIVATE MNN_USE_OPENCV)
    target_link_libraries(runTrainDemo.out PRIVATE ${OpenCV_LIBS})
endif()
