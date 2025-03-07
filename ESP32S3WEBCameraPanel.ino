#include "network.h"
#include "camera.h"
#include "deviceInfo.h"
// #include "driver/temperature_sensor.h"

/**
 * @brief 初始化温度传感器（已注释）
 */
/*
void initTemperatureSensor() {
    // 1. 配置传感器参数
    temperature_sensor_config_t config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-20, 110);
  
    // 2. 初始化传感器
    if (temperature_sensor_install(&config, &temp_sensor) != ESP_OK) {
        Serial.println("温度传感器初始化失败");
        return;
    }
  
    // 3. 启用传感器
    temperature_sensor_enable(temp_sensor);
}
*/

/**
 * @brief 将内容写入文件
 * @param fileName 文件名
 * @param content 要写入的内容
 * @return true 写入成功
 * @return false 写入失败
 */
bool writeContentToFile(const char* fileName, const char* content) {
    File f = LittleFS.open(fileName, "w");
    if (f) {
        f.print(content);
        f.close();
        return true;
    }
    return false;
}


// 定义喂狗任务
void feedWatchdogTask(void * parameter) {
    while(true) {
        esp_task_wdt_reset(); // 重置看门狗
        vTaskDelay(pdMS_TO_TICKS(1000)); // 每1秒执行一次
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
// 初始化程序
////////////////////////////////////////////////////////////////////////////////////////////
void setup(void) {
    // 初始化串口通信
    Serial.begin(115200);
    
    // 初始化看门狗
    esp_task_wdt_init(30, false); // 30秒超时

    // 创建喂狗任务
    xTaskCreate(
        feedWatchdogTask,    // 任务函数
        "FeedWatchdog",      // 任务名称
        2048,                // 堆栈大小
        NULL,                // 任务参数
        1,                   // 任务优先级（较低）
        NULL                 // 任务句柄
    );

    // 初始化文件系统
    if (!LittleFS.begin(true)) {
        ESP_LOGI("Setup", "LittleFS 初始化失败");
        return;
    }

#if 1
    // 初始化NVS
    if (nvsInit()) {
        if (nvsOpenSpace()) {
            ESP_LOGI("Setup", "NVS 命名空间打开成功");
        } else {
            ESP_LOGI("Setup", "NVS 命名空间打开失败");
            return;
        }
    } else {
        ESP_LOGI("Setup", "NVS 初始化失败");
        return;
    }

    // 初始化网络
    initNetWork();

    // 打开摄像头
    openCamera();

    ESP_LOGI("Setup", "初始化完成");
#endif

    // 更新并打印设备信息
    DI::updateInfo();
    ESP_LOGI("DeviceInfo", "%s", DI::toJsonString().c_str());
}

/**
 * @brief 主循环
 */
void loop(void) {
    // delay(1);
}