cmake_minimum_required(VERSION 3.10.2)

project("face_verifier")

# 1. 设置 NCNN 路径 (这需要在你的 Android 项目中替换为你实际下载的 ncnn-android-vulkan 路径)
# 此处以环境变量或项目根目录下的 ncnn-android-sdk 为例
set(ncnn_DIR ${CMAKE_SOURCE_DIR}/ncnn-android-vulkan/${ANDROID_ABI}/lib/cmake/ncnn)
find_package(ncnn REQUIRED)

# 2. 查找 Android 原生系统库
find_library(log-lib log)
find_library(jnigraphics-lib jnigraphics)

# 3. 关联你的 SDK JNI 源码
add_library(face_verifier SHARED
            face_verifier.cpp)

# 4. 链接 NCNN 与 Android 原生依赖
target_link_libraries(face_verifier
                      ncnn
                      ${log-lib}
                      ${jnigraphics-lib})
