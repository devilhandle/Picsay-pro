cmake_minimum_required(VERSION 3.22.1)
project(PicSayNativeArm64 LANGUAGES C CXX)

if(NOT ANDROID)
    message(FATAL_ERROR "Configure this project with the Android NDK toolchain.")
endif()

if(NOT ANDROID_ABI STREQUAL "arm64-v8a")
    message(FATAL_ERROR "This port target must be built for arm64-v8a; got: ${ANDROID_ABI}")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Native source is not present in the uploaded APK/repository. Do not substitute
# empty JNI stubs: PicSay's editing engine depends on the full implementation.
set(IMAGEOP_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/ImageOp/native_sources_required.cpp
)
set(GLIMAGEOP_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/GLImageOp/native_sources_required.cpp
)

foreach(source IN LISTS IMAGEOP_SOURCES GLIMAGEOP_SOURCES)
    if(NOT EXISTS "${source}")
        message(FATAL_ERROR
            "ARM64 port is not buildable yet: native implementation source is missing (${source}). "
            "Restore/reconstruct the real PicSay native source before building.")
    endif()
endforeach()

add_library(ImageOp SHARED ${IMAGEOP_SOURCES})
set_target_properties(ImageOp PROPERTIES OUTPUT_NAME "ImageOp")
target_link_libraries(ImageOp PRIVATE z jnigraphics log android)

add_library(GLImageOp SHARED ${GLIMAGEOP_SOURCES})
set_target_properties(GLImageOp PROPERTIES OUTPUT_NAME "GLImageOp")
target_link_libraries(GLImageOp PRIVATE ImageOp GLESv3 EGL log android)
