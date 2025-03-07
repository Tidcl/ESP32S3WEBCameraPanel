/**
 * @file network.h
 * @brief 网络配置及HTTP服务器实现
 * @details 包含WiFi连接、AP模式切换、HTTP服务器启动及处理函数
 */

#pragma once

#include <WebServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <esp_http_server.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Arduino.h>
#include <LittleFS.h>
#include <esp_task_wdt.h>
#include <esp_camera.h>
#include "nvsoper.h"
#include "deviceInfo.h"
#include "camera.h"

AsyncWebServer asyncServer(80);  // 异步服务器，处理常规HTTP请求
httpd_handle_t streamServer = NULL;  // 仅用于流媒体服务

// 常量定义区域
#define NVS_NET_AP_SSID "apssid"        // AP模式SSID的NVS存储键
#define NVS_NET_AP_PASSWORD "appassword" // AP模式密码的NVS存储键
#define NVS_NET_STA_SSID "stassid"      // STA模式SSID的NVS存储键
#define NVS_NET_STA_PASSWORD "stapassword" // STA模式密码的NVS存储键

// 网络配置默认值
static String g_ap_ssid = "esp32s3c";      // 默认AP SSID
static String g_ap_password = "12345678";  // 默认AP密码
static String g_sta_ssid = "Xiaomi_1F2D";  // 默认STA SSID
static String g_sta_password = "12345qwe"; // 默认STA密码

// IP地址配置
IPAddress APLocalIP(192,168,1,1);    // AP模式本地IP
IPAddress APGatewat(192,168,1,1);    // AP模式网关
IPAddress APSubNet(255,255,255,0);   // AP模式子网掩码

// 状态标志
bool g_ap_enable = false;  // AP模式是否启用
bool g_sta_enable = false; // STA模式是否启用

// HTTP服务器相关定义
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

// 函数声明
void register_handler(httpd_handle_t server_handle, const char *url, esp_err_t (*handler)(httpd_req_t *r));
bool switchToAP(String ssid, String password);
bool switchToSTA(String ssid, String password);
void getNetWorkConfig();
bool initNetWork();
void startServer();


/**
 * @brief 处理JPEG视频流请求
 * @param req HTTP请求对象
 * @return esp_err_t 执行结果
 */
esp_err_t jpg_stream_httpd_handler(httpd_req_t *req){
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len;
    uint8_t * _jpg_buf;
    char * part_buf[64];
    static int64_t last_frame = 0;
    if(!last_frame) {
        last_frame = esp_timer_get_time();
    }
    
    res = httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    if(res != ESP_OK){
        return res;
    }
    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if(res != ESP_OK){
        return res;
    }

    ESP_LOGD("HTTP VideoStream", "start.");
    while(true){
        fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE("Camera", "Camera capture failed");
            res = ESP_FAIL;
            break;
        }
        if(fb->format != PIXFORMAT_JPEG){
            bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
            if(!jpeg_converted){
                ESP_LOGE("Camera", "JPEG compression failed");
                esp_camera_fb_return(fb);
                res = ESP_FAIL;
            }
        } else {
            _jpg_buf_len = fb->len;
            _jpg_buf = fb->buf;
        }

        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if(res == ESP_OK){
            size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if(fb->format != PIXFORMAT_JPEG){
            free(_jpg_buf);
        }
        esp_camera_fb_return(fb);
        if(res != ESP_OK){
            break;
        }
    }

    ESP_LOGD("HTTP VideoStream", "finish.");
    last_frame = 0;
    return res;
}

// /**
//  * @brief 设置设备参数
//  * @param req HTTP请求对象
//  * @return esp_err_t 执行结果
//  * @details 可设置WiFi参数和相机参数
//  */
// esp_err_t set_Param(httpd_req_t *req){
//     // 获取查询字符串长度
//     size_t query_len = httpd_req_get_url_query_len(req);
//     if(query_len == 0){
//       return httpd_resp_send(req,  "bad params", HTTPD_RESP_USE_STRLEN);
//     }
//     // 动态分配内存存储查询字符串
//     char *query_str = (char*)malloc(query_len + 1); // +1 用于终止符
//     // 获取完整查询字符串
//     if (httpd_req_get_url_query_str(req, query_str, query_len + 1) == ESP_OK) {
//         ESP_LOGI("SET STA", "Query String: %s", query_str); // 输出：user=admin&action=start
//     }
//     char ssid[32] = {0};
//     // 获取单个参数值
//     if(httpd_query_key_value(query_str, "ssid", ssid, sizeof(ssid)) == ESP_OK) {
//       if(strlen(ssid) > 0)
//        g_sta_ssid = ssid;
//     }
//     char password[32] = {0};
//     if(httpd_query_key_value(query_str, "password", password, sizeof(password)) == ESP_OK) {
//       if(strlen(password) > 0)
//        g_sta_password = password;
//     }
//     char temp[32] = {0};
//     if(httpd_query_key_value(query_str, "quality", temp, sizeof(temp)) == ESP_OK) {
//        cameraJpegQuality = atoi(temp);
//     }
//     if(httpd_query_key_value(query_str, "pixformat", temp, sizeof(temp)) == ESP_OK) {
//        cameraPixFormat = atoi(temp);
//     }
//     if(httpd_query_key_value(query_str, "framesize", temp, sizeof(temp)) == ESP_OK) {
//        cameraFrameSize = atoi(temp);
//     }

//     ESP_LOGI("", "setParam %s %s %d %d %d", g_sta_ssid.c_str(), g_sta_password.c_str(),
//     cameraJpegQuality, cameraPixFormat, cameraFrameSize);

//     nvsSetNumber(VNS_CAMERA_KEY_CJQ, cameraJpegQuality);
//     nvsSetNumber(VNS_CAMERA_KEY_CPF, cameraPixFormat);
//     nvsSetNumber(VNS_CAMERA_KEY_CFS, cameraFrameSize);
//     nvsSetString(NVS_NET_STA_SSID, g_sta_ssid);
//     nvsSetString(NVS_NET_STA_PASSWORD, g_sta_password);

//     //设置返回类型
//     httpd_resp_set_type(req, "application/json");
//     httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
//     JSONVar doc;
//     doc["success"] = true;
//     doc["note"] = "pleaseRestart";
//     ESP_LOGI("", "json %s", JSON.stringify(doc).c_str());
//     httpd_resp_send(req, JSON.stringify(doc).c_str(), HTTPD_RESP_USE_STRLEN);

//     delay(1000);
//     ESP.restart();
// }


void startServer(void) {
    // 启动异步服务器
    // asyncServer.on("/all.min.css", 0b00000001, [](AsyncWebServerRequest *request){
    //     request->send(LittleFS, "/all.min.css", "text/css");
    // });

    // asyncServer.on("/bootstrap.min.css", 0b00000001, [](AsyncWebServerRequest *request){
    //     request->send(LittleFS, "/bootstrap.min.css", "text/css");
    // });

    // asyncServer.on("/bootstrap.bundle.min.js", 0b00000001, [](AsyncWebServerRequest *request){
    //     request->send(LittleFS, "/bootstrap.bundle.min.js", "text/javascript");
    // });
    asyncServer.serveStatic("/", LittleFS, "/").setDefaultFile("main.html").setCacheControl("max-age=3600");

    asyncServer.on("/webfonts/fa-solid-900.ttf", 0b00000001, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/fa-solid-900.ttf", "application/octet-stream");
    });

    asyncServer.on("/webfonts/fa-solid-900.woff2", 0b00000001, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/fa-solid-900.woff2", "application/octet-stream");
    });

    // asyncServer.on("/", 0b00000001, [](AsyncWebServerRequest *request){
    //     request->send(LittleFS, "/main.html", "text/html");
    // });

    asyncServer.on("/setParam", 0b00000001, [](AsyncWebServerRequest *request){
        // 获取参数值
        String ssid = request->getParam("ssid") ? request->getParam("ssid")->value() : "";
        String password = request->getParam("password") ? request->getParam("password")->value() : "";
        int quality = request->getParam("quality") ? request->getParam("quality")->value().toInt() : cameraJpegQuality;
        int pixformat = request->getParam("pixformat") ? request->getParam("pixformat")->value().toInt() : cameraPixFormat;
        int framesize = request->getParam("framesize") ? request->getParam("framesize")->value().toInt() : cameraFrameSize;

        // 更新全局变量
        if (ssid.length() > 0) g_sta_ssid = ssid;
        if (password.length() > 0) g_sta_password = password;
        cameraJpegQuality = quality;
        cameraPixFormat = pixformat;
        cameraFrameSize = framesize;

        // 保存到NVS
        nvsSetNumber(VNS_CAMERA_KEY_CJQ, cameraJpegQuality);
        nvsSetNumber(VNS_CAMERA_KEY_CPF, cameraPixFormat);
        nvsSetNumber(VNS_CAMERA_KEY_CFS, cameraFrameSize);
        nvsSetString(NVS_NET_STA_SSID, g_sta_ssid);
        nvsSetString(NVS_NET_STA_PASSWORD, g_sta_password);

        // 返回响应
        JSONVar doc;
        doc["success"] = true;
        doc["note"] = "pleaseRestart";
        request->send(200, "application/json", JSON.stringify(doc));

        // 重启设备
        delay(1000);
        ESP.restart();
    });

    asyncServer.on("/deviceInfo", HTTP_GET, [](AsyncWebServerRequest *request){
        DI::updateInfo();
        request->send(200, "application/json", DI::toJsonString());
    });

    asyncServer.begin();
    ESP_LOGI("HTTP", "Async server started on port 80");

    // 启动流媒体服务器
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 81;
    config.max_open_sockets = 2;  // 只需要处理流媒体连接
    config.stack_size = 4096;

    if (httpd_start(&streamServer, &config) == ESP_OK) {
        register_handler(streamServer, "/stream", jpg_stream_httpd_handler);
        ESP_LOGI("HTTP", "Stream server started on port 81");
    } else {
        ESP_LOGE("HTTP", "Failed to start stream server");
    }
}

//////////////////////////////////////////////////////////////////
//网络代码
//////////////////////////////////////////////////////////////////

/**
 * @brief 切换到AP模式
 * @param ssid AP的SSID
 * @param password AP的密码
 * @return bool 切换是否成功
 */
bool switchToAP(String ssid, String password){
  esp_task_wdt_reset();
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);

  WiFi.softAPConfig(APLocalIP, APGatewat, APSubNet);
  if(!WiFi.softAP(ssid)){
    g_ap_enable = false;
    return false;
  }
  ESP_LOGI("Network", "AP IP Address: %s", WiFi.softAPIP().toString().c_str());
  g_ap_enable = true;
  return true;
}

/**
 * @brief 切换到STA模式
 * @param ssid STA的SSID
 * @param password STA的密码
 * @return bool 切换是否成功
 */
bool switchToSTA(String ssid, String password){
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  int count = 0;
  while(WiFi.status() != WL_CONNECTED){
    if(count == 3){
      ESP_LOGI("Network", "connect to %s failed", ssid.c_str());
      return false;
    }
    delay(1000);
    esp_task_wdt_reset();  //喂狗
    ESP_LOGI("Network", "try connect %s count %d", ssid.c_str(), ++count);
  }
  g_sta_enable = true;
  ESP_LOGI("Network", "STA IP Address: %s", WiFi.localIP().toString().c_str());

  return true;
}

void getNetWorkConfig(){
  String sta_ssid_t;
  if(nvsGetString(NVS_NET_STA_SSID, &sta_ssid_t)){
    g_sta_ssid = sta_ssid_t;
  }
  String sta_password_t;
  if(nvsGetString(NVS_NET_STA_PASSWORD, &sta_password_t)){
    g_sta_password = sta_password_t;
  }

#if 0
  nvsGetString(NVS_NET_AP_SSID, &g_ap_ssid);
  nvsGetString(NVS_NET_AP_PASSWORD, &g_ap_password);
#endif

  ESP_LOGI("Network", "ap ssid=%s; ap password=%s; sta ssid=%s; sta password=%s",
  g_ap_ssid.c_str(), g_ap_password.c_str(), g_sta_ssid.c_str(), g_sta_password.c_str());
}

// 修改initNetWork函数
bool initNetWork(){

    getNetWorkConfig();

    if(!switchToSTA(g_sta_ssid, g_sta_password)){
        switchToAP(g_ap_ssid, g_ap_password);
    }

    startServer();

    ESP_LOGI("Network", "Web servers started successfully");
    return true;
}

void register_handler(httpd_handle_t server_handle, const char *url, esp_err_t (*handler)(httpd_req_t *r)){
      httpd_uri_t uri_get = {
      .uri = url,
      .method = HTTP_GET,
      .handler = handler,
      .user_ctx = NULL};
      httpd_register_uri_handler(server_handle , &uri_get);
}