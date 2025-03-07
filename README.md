<style>
  .rounded-font {
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
    font-weight: 400;
    letter-spacing: 0.5px;
  }
</style>

<div class="rounded-font">

# ESP32-S3 摄像头控制面板 📸

![ESP32-S3](https://img.shields.io/badge/ESP32--S3-CAM-blue?style=for-the-badge&logo=espressif&logoColor=white) ![Arduino](https://img.shields.io/badge/Arduino-IDE-00979D?style=for-the-badge&logo=arduino&logoColor=white) ![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

## 📋 项目简介

本项目是一个基于 **ESP32-S3CAM** 开发板的摄像头控制面板，通过 Web 界面实现对摄像头的实时监控、设备状态查看和参数设置。项目集成了 **OV2640 摄像头模块**，支持实时视频流、拍照、设备信息监控等功能，适用于智能家居、安防监控等场景。

---

## ✨ 功能特性

- 🎥 **实时视频流**：通过 Web 界面查看摄像头实时视频流
- 📊 **设备信息监控**：实时显示 CPU 频率、内存使用、WiFi 状态、存储空间等设备信息
- 📷 **拍照功能**：支持通过 Web 界面拍照并保存照片
- ⚙️ **参数设置**：支持设置 WiFi SSID、密码、摄像头质量、像素格式等参数
- 💾 **NVS 存储**：使用 NVS（非易失性存储）保存设备配置，重启后自动加载

---

## 🛠️ 硬件要求

- **ESP32-S3CAM 开发板**：基于 ESP32-S3 芯片，集成 OV2640 摄像头模块，支持 WiFi 和蓝牙连接
- **OV2640 摄像头模块**：200 万像素，支持 JPEG 压缩输出
- 其他：电源、连接线等

---

## 📦 软件要求

| 软件 | 版本 | 说明 |
|------|------|------|
| ![Arduino IDE](https://img.shields.io/badge/Arduino-IDE-00979D?style=flat-square&logo=arduino&logoColor=white) | 2.0+ | 开发环境 |
| ![ESP32](https://img.shields.io/badge/ESP32-开发板支持包-red?style=flat-square&logo=espressif&logoColor=white) | 最新版 | 硬件支持 |

### 依赖库：
- `esp_camera` - 摄像头控制
- `LittleFS` - 文件系统管理
- `Arduino_JSON` - JSON 数据处理
- `WiFi` - 网络连接
- `nvs_flash` - 非易失性存储

---

## ⚙️ Arduino IDE 配置

如果你使用相同配置的开发板（ESP32-S3CAM），请在 Arduino IDE 的"工具"菜单中进行以下配置：

| 配置项 | 推荐值 |
|--------|--------|
| Flash Mode | `QIO 80MHz` |
| Flash Size | `16MB (128Mb)` |
| PSRAM | `OPI PSRAM` |

---

## 📝 分区表配置

### 自定义分区表

如果您需要自定义分区表，可以使用以下配置：

```plaintext
# Name,   Type, SubType, Offset,  Size, Flags
nvs,      data, nvs,     0x9000,  0x5000,
otadata,  data, ota,     0xe000,  0x2000,
app0,     app,  ota_0,   0x10000, 0x480000,
spiffs,   data, spiffs,  0x490000,0xB60000,
coredump, data, coredump,0xFF0000,0x10000,
```

### 使用 Arduino 默认分区表

如果不选择自定义分区表，可以在 Arduino IDE 中选择带有 `spiffs` 分区的默认分区表，例如：
- **`8M with spiffs(3MB APP/1.5MB SPIFFS)`**：默认分区表，包含 `spiffs` 分区。

---

## 📱 设备说明

### ESP32-S3CAM 开发板

| 组件 | 规格 |
|------|------|
| 芯片 | ESP32-S3，双核 Xtensa LX7 处理器，主频高达 240MHz |
| 内存 | 内置 512KB SRAM，支持外部 PSRAM（8MB） |
| 存储 | 支持 SPI Flash（4MB），可通过 SPIFFS 或 LittleFS 管理文件系统 |
| 摄像头 | 集成 OV2640 摄像头模块，支持 200 万像素，JPEG 压缩输出 |
| 网络 | 支持 2.4GHz WiFi 和蓝牙 5.0 |
| 其他 | 支持 MicroSD 卡扩展存储（可选） |

### OV2640 摄像头模块

| 特性 | 参数 |
|------|------|
| 分辨率 | 支持最高 1600x1200（UXGA）分辨率 |
| 输出格式 | 支持 JPEG、RGB565、YUV422 等格式 |
| 帧率 | 最高 15fps（UXGA 分辨率下） |
| 接口 | 通过 DVP 接口与 ESP32-S3 通信 |

---

## 📂 项目结构

```
ESP32S3WEBCameraPanel/
├── 📁 data/                  # 网页文件目录
│   ├── 📄 main.html          # 主网页文件
│   ├── 📄 bootstrap.min.css  # Bootstrap CSS 文件
│   ├── 📄 all.min.css        # FontAwesome CSS 文件
│   └── 📄 ...                # 其他网页资源文件
├── 📄 deviceInfo.h           # 设备信息相关代码
├── 📄 camera.h               # 摄像头相关代码
├── 📄 network.h              # 网络相关代码
├── 📄 nvsoper.h              # NVS 存储相关代码
├── 📄 ESP32S3WEBCameraPanel.ino      # 主程序文件
├── 📄 README.md              # 项目说明文档
└── 📄 ESP32S3WEBCameraPanel.code-workspace
```

---

## 🔌 OV2640 与开发板 GPIO 连线

以下是 OV2640 摄像头模块与 ESP32-S3CAM 开发板的 GPIO 连接方式：

| OV2640 引脚 | ESP32-S3CAM 引脚 | 功能描述 |
|-------------|------------------|----------|
| PWDN        | -                | 电源控制引脚（未使用） |
| RESET       | -                | 复位引脚（未使用） |
| XCLK        | GPIO15           | 时钟信号引脚 |
| SIOD        | GPIO4            | I2C 数据引脚 |
| SIOC        | GPIO5            | I2C 时钟引脚 |
| D7          | GPIO16           | 数据引脚 7 |
| D6          | GPIO17           | 数据引脚 6 |
| D5          | GPIO18           | 数据引脚 5 |
| D4          | GPIO12           | 数据引脚 4 |
| D3          | GPIO10           | 数据引脚 3 |
| D2          | GPIO8            | 数据引脚 2 |
| D1          | GPIO9            | 数据引脚 1 |
| D0          | GPIO11           | 数据引脚 0 |
| VSYNC       | GPIO6            | 垂直同步信号 |
| HREF        | GPIO7            | 水平参考信号 |
| PCLK        | GPIO13           | 像素时钟 |

---

## 📥 安装与使用

### 1. 克隆项目

```bash
git clone https://github.com/yourusername/esp32-s3-camera-control-panel.git
cd esp32-s3-camera-control-panel
```

### 2. 安装依赖库

在 Arduino IDE 中，打开库管理器，搜索并安装以下库：
- `esp_camera`
- `LittleFS`
- `Arduino_JSON`


### 3. 安装 LittleFS 上传插件

为了将网页文件上传到 ESP32-S3 的文件系统中，您需要安装 **Upload LittleFS to ESP32** 插件。安装步骤如下：

1. 在 [https://github.com/lorol/arduino-esp32fs-plugin/releases](https://github.com/lorol/arduino-esp32fs-plugin/releases) 下载 `esp32fs` 的 Arduino 插件。
2. 将下载的 `esp32fs.jar` 文件放入 Arduino 安装目录的 `\Arduino IDE\tools\ESP32FS\tool\` 文件夹中。
3. 重启 Arduino IDE。
4. 按 `Ctrl+Shift+P`，输入 `Upload LittleFS to Pico/ESP8266/ESP32`，插件会将项目的 `data` 目录中的文件上传到 ESP32-S3 的文件系统中。

### 4. 配置开发板

在 Arduino IDE 中，选择 `工具` -> `开发板` -> `ESP32 Dev Module`。

### 5. 上传代码

将代码上传到 ESP32-S3 开发板。


### 6. 上传网页文件

在代码上传完成后，确保已安装 LittleFS 上传插件，并按 `Ctrl+Shift+P`，输入 `Upload LittleFS to Pico/ESP8266/ESP32`，将 `data` 目录中的网页文件上传到 ESP32-S3 的文件系统中。

### 7. 连接 WiFi

设备首次启动时，默认以 **AP 模式** 运行，WiFi 节点名称为 `esp32s3c`，密码为 `12345678`。连接后，通过浏览器访问 `192.168.1.1` 进入控制面板。

### 8. 设置 STA 模式

在控制面板的 **WiFi 设置** 中，输入您的 WiFi SSID 和密码，保存设置后设备将自动重启并尝试连接 WiFi。

### 9. 获取设备 IP

STA 模式连接成功后，设备会通过串口打印其 IP 地址。您可以通过串口监视器（波特率 115200）查看 IP 地址，然后在浏览器中输入该 IP 地址访问控制面板。

---

## 🎮 使用说明

| 功能 | 操作方法 |
|------|----------|
| 🎥 实时视频流 | 点击"打开"按钮启动摄像头，查看实时视频流 |
| 📊 设备信息监控 | 在设备信息面板中查看 CPU 频率、内存使用、WiFi 状态等信息 |
| 📷 拍照功能 | 点击"拍照"按钮，拍摄照片并保存到设备中 |
| ⚙️ 参数设置 | 在设置面板中修改 WiFi SSID、密码、摄像头质量、像素格式等参数，并保存 |


---

## 🤝 贡献

欢迎任何形式的贡献！如果您有建议或发现了问题，请提交 issue 或 pull request。

<p align="center">
  <a href="https://github.com/yourusername/esp32-s3-camera-control-panel/issues/new">
    <img src="https://img.shields.io/badge/提交-Issue-blue.svg?style=for-the-badge&logo=github" alt="提交 Issue">
  </a>
  <a href="https://github.com/yourusername/esp32-s3-camera-control-panel/pulls">
    <img src="https://img.shields.io/badge/创建-Pull%20Request-blue.svg?style=for-the-badge&logo=github" alt="创建 Pull Request">
  </a>
</p>

---

## 🙏 致谢

- 感谢 [ESP32](https://www.espressif.com/) 提供的强大硬件支持。
- 感谢 [Arduino](https://www.arduino.cc/) 社区提供的丰富资源。

<p align="center">
  <a href="https://www.espressif.com/">
    <img src="https://img.shields.io/badge/Powered%20by-Espressif-red?style=for-the-badge&logo=espressif" alt="Espressif">
  </a>
  <a href="https://www.arduino.cc/">
    <img src="https://img.shields.io/badge/Built%20with-Arduino-00979D?style=for-the-badge&logo=arduino" alt="Arduino">
  </a>
</p>

