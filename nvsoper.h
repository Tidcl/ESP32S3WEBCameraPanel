#pragma once

#include <nvs_flash.h>

static nvs_handle_t g_nvs_handle; // NVS 句柄

/**
 * @brief 初始化NVS存储
 * @return true 初始化成功
 * @return false 初始化失败
 */
bool nvsInit() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // 如果 NVS 分区被截断或版本不匹配，则擦除并重新初始化
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            return false;
        }
    }
    ESP_ERROR_CHECK(ret);
    return true;
}

/**
 * @brief 打开NVS命名空间
 * @return true 打开成功
 * @return false 打开失败
 */
bool nvsOpenSpace() {
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &g_nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI("NVS", "Error opening NVS handle!");
        return false;
    } else {
        ESP_LOGI("NVS", "NVS handle opened successfully!");
        return true;
    }
}

/**
 * @brief 提交NVS更改
 * @return true 提交成功
 * @return false 提交失败
 */
bool nvsCommit() {
    esp_err_t err = nvs_commit(g_nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI("NVS", "Error committing changes to NVS!");
        return false;
    } else {
        ESP_LOGI("NVS", "Successfully committed changes to NVS!");
        return true;
    }
}

/**
 * @brief 从NVS获取字符串值
 * @param key 键名
 * @param value 存储返回值的指针
 * @return true 获取成功
 * @return false 获取失败
 */
bool nvsGetString(String key, String *value) {
    char read_str[50]; // 确保缓冲区足够大以容纳字符串
    size_t required_size;
    esp_err_t err = nvs_get_str(g_nvs_handle, key.c_str(), read_str, &required_size);
    if (err != ESP_OK) {
        return false;
    }

    *value = String(read_str);
    return true;
}

/**
 * @brief 从NVS获取整数值
 * @param key 键名
 * @param value 存储返回值的指针
 * @return true 获取成功
 * @return false 获取失败
 */
bool nvsGetNumber(String key, int *value) {
    int read_number = -1;
    size_t required_size;
    esp_err_t err = nvs_get_i32(g_nvs_handle, key.c_str(), value);
    if (err != ESP_OK) {
        return false;
    }
    return true;
}

/**
 * @brief 向NVS存储字符串值
 * @param key 键名
 * @param value 要存储的值
 * @return true 存储成功
 * @return false 存储失败
 */
bool nvsSetString(String key, String value) {
    esp_err_t err = nvs_set_str(g_nvs_handle, key.c_str(), value.c_str());
    if (err == ESP_OK && nvsCommit()) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief 向NVS存储整数值
 * @param key 键名
 * @param num 要存储的值
 * @return true 存储成功
 * @return false 存储失败
 */
bool nvsSetNumber(String key, int num) {
    esp_err_t err = nvs_set_i32(g_nvs_handle, key.c_str(), num);
    if (err == ESP_OK && nvsCommit()) {
        return true;
    } else {
        return false;
    }
}
