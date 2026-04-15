#include <jni.h>
#include <string>
#include <vector>
#include <android/asset_manager_jni.h>
#include <android/bitmap.h>
#include <android/log.h>

// NCNN 核心头文件
#include "net.h"

#define TAG "FaceVerifierJNI"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

// NCNN 实例
static ncnn::Net g_face_net;
static ncnn::Net g_liveness_net;

extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_ncnnsdk_FaceVerifier_init(JNIEnv *env, jobject thiz, jobject assetManager) {
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    
    // NCNN 优化选项配置
    ncnn::Option opt;
    opt.lightmode = true;
    opt.num_threads = 4;
    // 如果编译的 NCNN 包含 rknpu 支持，这里相关调用会在底层自动分发到 rknpu 环境
    // opt.use_vulkan_compute = false; 

    g_face_net.opt = opt;
    g_liveness_net.opt = opt;

    // TODO: 替换为实际的模型文件名。请将 param 和 bin 放入 Android main/assets 目录下
    if (g_face_net.load_param(mgr, "face_recognition.param") != 0 ||
        g_face_net.load_model(mgr, "face_recognition.bin") != 0) {
        LOGE("Failed to load face recognition model");
        return JNI_FALSE;
    }

    if (g_liveness_net.load_param(mgr, "liveness_dual.param") != 0 ||
        g_liveness_net.load_model(mgr, "liveness_dual.bin") != 0) {
        LOGE("Failed to load liveness model");
        return JNI_FALSE;
    }

    LOGD("NCNN Models load success!");
    return JNI_TRUE;
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_example_ncnnsdk_FaceVerifier_verify(JNIEnv *env, jobject thiz, jobject rgb_bitmap, jobject ir_bitmap) {
    // 1. 获取 Bitmap 信息并转换为 ncnn::Mat
    AndroidBitmapInfo rgb_info;
    AndroidBitmap_getInfo(env, rgb_bitmap, &rgb_info);
    
    void* rgb_pixels;
    AndroidBitmap_lockPixels(env, rgb_bitmap, &rgb_pixels);
    
    // 将 Android Bitmap 转换成 NCNN 接受的输入格式 
    // ncnn::Mat in_rgb = ncnn::Mat::from_pixels((const unsigned char*)rgb_pixels, ncnn::Mat::PIXEL_RGBA2RGB, rgb_info.width, rgb_info.height);
    // (需要视您的模型要求对图片进行 resize 和归一化处理)
    
    AndroidBitmap_unlockPixels(env, rgb_bitmap);

    // 2. 双目活体检测前向推理过程 (伪代码)
    // ncnn::Extractor ex_live = g_liveness_net.create_extractor();
    // ex_live.input("in_rgb", in_rgb); 
    // ex_live.input("in_ir", in_ir);
    // ncnn::Mat out_live;
    // ex_live.extract("out", out_live);
    bool is_live = true;        // 替换为模型推理解析后的结果
    float liveness_score = 0.95f; 
    
    // 3. 人脸特征提取与 1:1 比对
    // ncnn::Extractor ex_face = g_face_net.create_extractor();
    // ex_face.input("data", in_rgb);
    // ncnn::Mat out_feature;
    // ex_face.extract("feature", out_feature);
    // 计算两张图片的余弦相似度...
    float similarity = 0.88f;   // 替换为实际人脸相似度计算结果
    bool is_match = similarity > 0.80f;

    // 4. 将 C++ 结果封装进 Java VerifyResult 对象中
    jclass result_class = env->FindClass("com/example/ncnnsdk/FaceVerifier$VerifyResult");
    jmethodID constructor = env->GetMethodID(result_class, "<init>", "()V");
    jobject result_obj = env->NewObject(result_class, constructor);

    jfieldID field_isLive = env->GetFieldID(result_class, "isLive", "Z");
    jfieldID field_livenessScore = env->GetFieldID(result_class, "livenessScore", "F");
    jfieldID field_similarity = env->GetFieldID(result_class, "similarity", "F");
    jfieldID field_isMatch = env->GetFieldID(result_class, "isMatch", "Z");

    env->SetBooleanField(result_obj, field_isLive, is_live);
    env->SetFloatField(result_obj, field_livenessScore, liveness_score);
    env->SetFloatField(result_obj, field_similarity, similarity);
    env->SetBooleanField(result_obj, field_isMatch, is_match);

    return result_obj;
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ncnnsdk_FaceVerifier_release(JNIEnv *env, jobject thiz) {
    g_face_net.clear();
    g_liveness_net.clear();
    LOGD("NCNN Models released");
}
