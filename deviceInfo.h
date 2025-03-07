#pragma once
#include <ESP.h>
#include <esp_partition.h>
#include <esp_heap_caps.h>
#include <FS.h>
#include <SPIFFS.h>
#include <LittleFS.h>
#include <Arduino_JSON.h>

namespace DI {

// 设备信息结构体
struct deviceInfo_s {
    unsigned int cpuMHz;        // CPU频率
    int temperature;            // 温度
    
    const char *sdk_version;    // SDK版本
    unsigned long uptime;       // 运行时间
    unsigned int totalRAM;      // 总RAM
    unsigned int freeRAM;       // 可用RAM
    unsigned int totalPSRAM;    // 总PSRAM
    unsigned int freePSRAM;     // 可用PSRAM

    String wifiSSID;            // WiFi SSID
    String wifiIP;              // WiFi IP地址
    char wifidBm;               // WiFi信号强度
    char wifiContented;         // WiFi连接状态

    unsigned int totalfs;       // 总存储空间
    unsigned int freefs;        // 可用存储空间
} deviceInfo_t;

// 更新设备信息
void updateInfo();

// 打印设备信息
void printInfo() {
    ESP_LOGI("", "CPU Frequency: %d MHz", deviceInfo_t.cpuMHz);
    ESP_LOGI("", "Temperature: %d C", deviceInfo_t.temperature);
    ESP_LOGI("", "SDK Version: %s", deviceInfo_t.sdk_version);
    ESP_LOGI("", "Uptime: %d ms", deviceInfo_t.uptime);
    ESP_LOGI("", "Total RAM: %d bytes", deviceInfo_t.totalRAM);
    ESP_LOGI("", "Free RAM: %d bytes", deviceInfo_t.freeRAM);
    ESP_LOGI("", "Total PSRAM: %d bytes", deviceInfo_t.totalPSRAM);
    ESP_LOGI("", "Free PSRAM: %d bytes", deviceInfo_t.freePSRAM);

    ESP_LOGI("", "WiFi SSID: %s", deviceInfo_t.wifiSSID.c_str());
    ESP_LOGI("", "WiFi IP: %s", deviceInfo_t.wifiIP.c_str());
    ESP_LOGI("", "WiFi Signal Strength: %d dBm", deviceInfo_t.wifidBm);
    ESP_LOGI("", "WiFi Connected: %d", deviceInfo_t.wifiContented);

    ESP_LOGI("", "Total Storage: %d bytes", deviceInfo_t.totalfs);
    ESP_LOGI("", "Free Storage: %d bytes", deviceInfo_t.freefs);
}

// 将设备信息转换为JSON字符串
String toJsonString() {
    JSONVar doc;
    doc["cpuMHz"] = deviceInfo_t.cpuMHz;
    doc["temperature"] = deviceInfo_t.temperature;
    doc["sdk_version"] = deviceInfo_t.sdk_version;
    doc["uptime"] = deviceInfo_t.uptime;
    doc["totalRAM"] = deviceInfo_t.totalRAM;
    doc["freeRAM"] = deviceInfo_t.freeRAM;
    doc["totalPSRAM"] = deviceInfo_t.totalPSRAM;
    doc["freePSRAM"] = deviceInfo_t.freePSRAM;
    doc["wifiSSID"] = deviceInfo_t.wifiSSID;
    doc["wifiIP"] = deviceInfo_t.wifiIP;
    doc["wifiConnected"] = deviceInfo_t.wifiContented;
    doc["wifidBm"] = deviceInfo_t.wifidBm;
    doc["totalfs"] = deviceInfo_t.totalfs;
    doc["freefs"] = deviceInfo_t.freefs;
    
    return JSON.stringify(doc);
}

// 获取CPU信息
void cpu() {
    deviceInfo_t.cpuMHz = getCpuFrequencyMhz();
}

// 获取温度信息
void temperature() {
    // TODO: 实现温度获取
    deviceInfo_t.temperature = 0;
}

// 获取SDK版本
void sdk_version() {
    deviceInfo_t.sdk_version = ESP.getSdkVersion();
}

// 获取运行时间
void runTime() {
    deviceInfo_t.uptime = millis();
}

// 获取内存信息
void ram() {
    deviceInfo_t.totalRAM = ESP.getHeapSize();
    deviceInfo_t.freeRAM = ESP.getFreeHeap();
    deviceInfo_t.totalPSRAM = ESP.getPsramSize();
    deviceInfo_t.freePSRAM = ESP.getFreePsram();
}

// 获取WiFi信息
void wifi() {
    deviceInfo_t.wifiIP = WiFi.localIP().toString();
    deviceInfo_t.wifiSSID = WiFi.SSID();
    deviceInfo_t.wifidBm = WiFi.RSSI();

    wl_status_t status = WiFi.status();
    deviceInfo_t.wifiContented = (status == WL_CONNECTED) ? 1 : 0;
}

// 获取文件系统信息
void littlefs() {
    deviceInfo_t.totalfs = LittleFS.totalBytes();
    deviceInfo_t.freefs = deviceInfo_t.totalfs - LittleFS.usedBytes();
}

// 更新所有设备信息
void updateInfo() {
    cpu();
    sdk_version();
    runTime();
    ram();
    wifi();
    littlefs();
}

} // namespace DI
