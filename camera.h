#pragma once
#include <esp_camera.h>
#include "nvsoper.h"

////////////////////////////////////////////////////////////////////////////////////////////
// OV2640 摄像头引脚配置
////////////////////////////////////////////////////////////////////////////////////////////
#define CAM_PIN_PWDN -1  // 电源控制引脚（未使用）
#define CAM_PIN_RESET -1 // 复位引脚（使用软件复位）
#define CAM_PIN_XCLK 15  // XCLK时钟信号引脚
#define CAM_PIN_SIOD 4   // I2C数据引脚
#define CAM_PIN_SIOC 5   // I2C时钟引脚

// 数据引脚配置
#define CAM_PIN_D7 16
#define CAM_PIN_D6 17
#define CAM_PIN_D5 18
#define CAM_PIN_D4 12
#define CAM_PIN_D3 10
#define CAM_PIN_D2 8
#define CAM_PIN_D1 9
#define CAM_PIN_D0 11

// 控制信号引脚
#define CAM_PIN_VSYNC 6  // 垂直同步信号
#define CAM_PIN_HREF 7   // 水平参考信号
#define CAM_PIN_PCLK 13  // 像素时钟

// NVS存储键值
#define VNS_CAMERA_KEY_CJQ "cjq"  // JPEG质量键
#define VNS_CAMERA_KEY_CPF "cpf"  // 像素格式键
#define VNS_CAMERA_KEY_CFS "cfs"  // 帧尺寸键

// 默认相机配置
int cameraJpegQuality = 20;  // JPEG质量（0-63，数值越小质量越高）
int cameraPixFormat = PIXFORMAT_JPEG;  // 像素格式
int cameraFrameSize = FRAMESIZE_VGA;   // 帧尺寸

/**
 * 从NVS获取相机配置
 */
void getCameraConfig() {
    // 临时变量用于存储从NVS读取的值
    int cameraJpegQuality_t;
    int cameraPixFormat_t;
    int cameraFrameSize_t;
    
    // 从NVS读取配置，如果存在则更新默认值
    if (nvsGetNumber(VNS_CAMERA_KEY_CJQ, &cameraJpegQuality_t)) {
        cameraJpegQuality = cameraJpegQuality_t;
    }
    if (nvsGetNumber(VNS_CAMERA_KEY_CPF, &cameraPixFormat_t)) {
        cameraPixFormat = cameraPixFormat_t;
    }
    if (nvsGetNumber(VNS_CAMERA_KEY_CFS, &cameraFrameSize_t)) {
        cameraFrameSize = cameraFrameSize_t;
    }
}

// 相机配置结构体
camera_config_t camera_config;

/**
 * 初始化并打开相机
 * @return 成功返回true，失败返回false
 */
bool openCamera() {
    // 获取相机配置
    getCameraConfig();

    // 配置相机引脚
    camera_config.pin_d0 = CAM_PIN_D0;
    camera_config.pin_d1 = CAM_PIN_D1;
    camera_config.pin_d2 = CAM_PIN_D2;
    camera_config.pin_d3 = CAM_PIN_D3;
    camera_config.pin_d4 = CAM_PIN_D4;
    camera_config.pin_d5 = CAM_PIN_D5;
    camera_config.pin_d6 = CAM_PIN_D6;
    camera_config.pin_d7 = CAM_PIN_D7;
    camera_config.pin_xclk = CAM_PIN_XCLK;
    camera_config.pin_pclk = CAM_PIN_PCLK;
    camera_config.pin_vsync = CAM_PIN_VSYNC;
    camera_config.pin_href = CAM_PIN_HREF;
    camera_config.pin_sccb_sda = CAM_PIN_SIOD;
    camera_config.pin_sccb_scl = CAM_PIN_SIOC;

    // 配置相机参数
    camera_config.xclk_freq_hz = 20000000;  // 20MHz时钟频率
    camera_config.pixel_format = (pixformat_t)cameraPixFormat;  // 像素格式
    camera_config.frame_size = (framesize_t)cameraFrameSize;    // 帧尺寸
    camera_config.jpeg_quality = cameraJpegQuality;             // JPEG质量
    camera_config.fb_count = 2;  // 帧缓冲区数量

    // 初始化相机
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGI("Camera", "camera init failed");
        return false;
    }
    return true;
}

/**
 * 关闭相机
 * @return 成功返回true，失败返回false
 */
bool closeCamera() {
    return esp_camera_deinit();
}

/**
 * 获取一帧图像
 * @return 成功返回true，失败返回false
 */
bool getFrame() {
    camera_fb_t *fb = NULL;
    fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGI("Camera", "Camera capture failed");
        return false;
    }
    
    // 打印图像信息
    ESP_LOGI("Camera", "width=%d height=%d size=%dbyte", fb->width, fb->height, fb->len);
    
    // 释放帧缓冲区
    esp_camera_fb_return(fb);
    return true;
}