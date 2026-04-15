package com.example.ncnnsdk;

import android.content.res.AssetManager;
import android.graphics.Bitmap;

public class FaceVerifier {

    // 加载底层 C++ 库
    static {
        System.loadLibrary("face_verifier");
    }

    /**
     * 验证结果数据类
     */
    public static class VerifyResult {
        public boolean isLive;       // 是否为真人活体
        public float livenessScore;  // 活体分数
        public float similarity;     // 1:1 比对相似度
        public boolean isMatch;      // 是否为同一人
    }

    /**
     * 初始化 NCNN 和人脸、活体模型 (利用 RK3566 的能力)
     * @param mgr Android 的 AssetManager，用于读取模型文件
     * @return 是否初始化成功
     */
    public native boolean init(AssetManager mgr);

    /**
     * 双目 1:1 活体核验
     * @param rgbImage  RGB 摄像头采集的图（用于面部比对和常规活体检测）
     * @param irImage   IR（红外）或深度摄像头采集的图（用于增强活体防伪检测）
     * @return 核验结果返回对象
     */
    public native VerifyResult verify(Bitmap rgbImage, Bitmap irImage);

    /**
     * 释放 NCNN 网络资源
     */
    public native void release();
}
