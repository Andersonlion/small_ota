# FAL (Flash Abstraction Layer) 使用说明

## 版本

定制版 FAL v0.5.99，已去除 RT-Thread 强依赖，可用于 bare metal / RTOS / RT-Thread。

## 文件结构

```
lib/lib_fal/
├── fal/              # FAL 核心源码（无需修改）
│   ├── inc/
│   │   ├── fal.h
│   │   └── fal_def.h
│   └── src/
│       ├── fal.c
│       ├── fal_flash.c
│       ├── fal_partition.c
│       └── fal_rtt.c       # RT-Thread 适配（#ifdef RT_VER_NUM 保护）
├── port/             # 移植层（需要修改）
│   ├── fal_cfg.h           # Flash 设备表 + 分区表
│   ├── fal_flash_port.c    # Flash 驱动模板 + sysprintf 实现
│   └── example/
│       └── fal_flash_swm320.c  # SWM320 移植参考
└── FAL_USAGE.md
```

## 移植步骤

### 1. 实现 `sysprintf`

在 `port/fal_flash_port.c` 中已提供默认实现，包装了标准 `vprintf`：

```c
void sysprintf(char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
```

如果你的平台 printf 函数名不同（如 `rt_kprintf`、`debug_printf`），替换实现即可。

### 2. 实现 Flash 驱动 ops

在 `port/fal_flash_port.c` 中实现四个回调：

```c
static int init(void)
{
    // 初始化 MCU Flash 控制器（可空）
    return 0;
}

static int read(long offset, uint8_t *buf, size_t size)
{
    // 从 onchip_flash.addr + offset 读取 size 字节到 buf
    // 返回实际读取字节数，失败返回 -1
    return size;
}

static int write(long offset, const uint8_t *buf, size_t size)
{
    // 向 onchip_flash.addr + offset 写入 size 字节
    // 注意对齐到 write_gran
    // 返回实际写入字节数，失败返回 -1
    return size;
}

static int erase(long offset, size_t size)
{
    // 从 onchip_flash.addr + offset 开始擦除 size 字节
    // 注意对齐到 blk_size
    // 返回实际擦除字节数，失败返回 -1
    return size;
}
```

### 3. 定义 Flash 设备

```c
const struct fal_flash_dev onchip_flash =
{
    .name       = "onchip",          // 设备名（分区表 flash_name 引用此名称）
    .addr       = 0x00000000,        // Flash 起始地址
    .len        = 512 * 1024,        // Flash 总大小
    .blk_size   = 4 * 1024,          // 最小擦除粒度
    .ops        = {init, read, write, erase},
    .write_gran = 8,                 // 写粒度(bit): NOR=1, STM32F2/F4=8, STM32F1=32, STM32L4=64
};
```

### 4. 配置分区表

在 `port/fal_cfg.h` 中：

```c
// 注册 Flash 设备表
extern const struct fal_flash_dev onchip_flash;

#define FAL_FLASH_DEV_TABLE      \
{                                \
    &onchip_flash,               \
}

// 定义分区表
#define FAL_PART_TABLE                                                        \
{                                                                             \
    {FAL_PART_MAGIC_WORD, "boot",   "onchip", 0,        64*1024,  0},        \
    {FAL_PART_MAGIC_WORD, "config", "onchip", 64*1024,   4*1024,  0},        \
    {FAL_PART_MAGIC_WORD, "app",    "onchip", 68*1024, 444*1024,  0},        \
}
```

分区表条目格式：`{FAL_PART_MAGIC_WORD, "分区名", "设备名", 偏移, 大小, 0}`

### 5. 初始化

```c
#include "fal.h"

int main(void)
{
    int ret = fal_init();
    if (ret < 0) {
        // 初始化失败
    }
    // 初始化成功，可以操作分区了
}
```

## API 速查

### 初始化

```c
int  fal_init(void);           // 返回 >= 0 为分区数量，< 0 失败
int  fal_init_check(void);     // 返回 1 已初始化，0 未初始化或失败
```

### 查找

```c
const struct fal_flash_dev  *fal_flash_device_find(const char *name);  // 按名找设备
const struct fal_partition   *fal_partition_find(const char *name);     // 按名找分区
void                         fal_show_part_table(void);                 // 打印分区表
```

### 分区操作

```c
int fal_partition_read(const struct fal_partition *part, uint32_t addr, uint8_t *buf, size_t size);
int fal_partition_write(const struct fal_partition *part, uint32_t addr, const uint8_t *buf, size_t size);
int fal_partition_erase(const struct fal_partition *part, uint32_t addr, size_t size);
int fal_partition_erase_all(const struct fal_partition *part);
```

- `addr` 是分区内偏移，不是绝对地址
- 返回值 >= 0 为成功操作的字节数，-1 为错误

### 使用示例

```c
const struct fal_partition *cfg_part = fal_partition_find("config");
if (cfg_part) {
    uint8_t buf[256];
    fal_partition_read(cfg_part, 0, buf, sizeof(buf));   // 读 config 分区前 256 字节
    fal_partition_write(cfg_part, 0, buf, sizeof(buf));  // 写入
    fal_partition_erase_all(cfg_part);                    // 全擦
}
```

## 配置选项

| 宏 | 位置 | 说明 |
|----|------|------|
| `FAL_DEBUG` | `fal_cfg.h` | 0=关闭调试日志，1=开启 |
| `FAL_PART_HAS_TABLE_CFG` | `fal_cfg.h` | 定义后使用静态分区表（推荐） |
| `FAL_PRINTF` | 可在编译时 `-D` 覆盖 | 日志输出函数，默认 `sysprintf` |

## 多设备支持

如果有一个外部 SPI Flash，添加第二个设备：

```c
// 注册
extern const struct fal_flash_dev spi_flash;  // 在另一个 port 文件中实现

#define FAL_FLASH_DEV_TABLE      \
{                                \
    &onchip_flash,               \
    &spi_flash,                  \
}

// 分区可跨设备
#define FAL_PART_TABLE                                                        \
{                                                                             \
    {FAL_PART_MAGIC_WORD, "boot",    "onchip",    0,         64*1024,  0},    \
    {FAL_PART_MAGIC_WORD, "app",     "onchip",    64*1024,  444*1024,  0},    \
    {FAL_PART_MAGIC_WORD, "storage", "spi_flash", 0,         2*1024*1024, 0}, \
}
```
