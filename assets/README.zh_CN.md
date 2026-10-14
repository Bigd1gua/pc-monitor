<p align="right">
  <strong>简体中文</strong> · <a href="README.md">English</a>
</p>

# 资源目录（Assets）

本目录集中存放可复用的资源（字库、图片、音乐等），按资源类型分子目录管理。每个资源放在其类型对应的子目录，并记录放置路径、命名方式、集成方式与来源/许可。二进制资源（字体、图片、音频）不属于纯 markdown 文档，请勿与文档混放。涉及版权/授权的资源需注明来源与许可。

## 字库（fonts）

可复用的字库文件与生成的字库源码放在 `fonts/`。

- 命名要能反映字族、字重、字级与格式。
- 记录来源、许可、字符范围、转换命令与目标放置路径。
- 添加字库前评估 Flash 与内部 RAM 影响；ESP32-C3 无 PSRAM。
- 不提交许可不允许分发的字库。

### 生成的界面中文子集

PC 状态监控应用(`main/app_pcmon.c`)会显示少量中文。默认 Montserrat 字库不含中文
字形,故基于[思源黑体 Source Han Sans SC](https://github.com/adobe-fonts/source-han-sans)
(Normal,采用 SIL Open Font License 1.1 许可)生成以下子集。源 OTF **不提交**,随 LVGL
提供,位于 `managed_components/lvgl__lvgl/scripts/built_in_font/SourceHanSansSC-Normal.otf`。

| 文件 | 覆盖范围 | 用途 |
| --- | --- | --- |
| [`fonts/app_font_14.c`](fonts/app_font_14.c) | ASCII `0x20-0x7E` + 23 个中文字码,14 px,4 bpp | 页面标题(如 `核心`、`内存`)。 |
| [`fonts/app_font_20.c`](fonts/app_font_20.c) | 同上覆盖,20 px,4 bpp | 中等字号中文取值(AGENT 页 `psutil` 项)。 |
| [`fonts/app_font_40.c`](fonts/app_font_40.c) | 同上覆盖,40 px,4 bpp | AGENT 页大号中文状态词。 |

- 字符清单(Unicode):`U+6838 U+5FC3 U+5904 U+7406 U+5668 U+5185 U+5B58 U+78C1 U+76D8 U+7F51 U+7EDC U+8D8B U+52BF U+7CFB U+7EDF U+4EE3 U+672A U+8FDE U+63A5 U+6B63 U+5E38 U+662F U+5426`。
- 生成工具:[`lv_font_conv`](https://github.com/lvgl/lv_font_conv) 1.5.3,运行于 Node.js v20.12.0。
- 重新生成(在仓库根目录执行;每个字号各执行一次):

  ```bash
  npx lv_font_conv@1.5.3 \
    --font managed_components/lvgl__lvgl/scripts/built_in_font/SourceHanSansSC-Normal.otf \
    --range 0x20-0x7E,0x6838,0x5FC3,0x5904,0x7406,0x5668,0x5185,0x5B58,0x78C1,0x76D8,0x7F51,0x7EDC,0x8D8B,0x52BF,0x7CFB,0x7EDF,0x4EE3,0x672A,0x8FDE,0x63A5,0x6B63,0x5E38,0x662F,0x5426 \
    --size 14 --bpp 4 --format lvgl --no-compress \
    --lv-font-name app_font_14 --lv-include lvgl.h \
    --output assets/fonts/app_font_14.c
  ```

- Flash 占用(取自 `idf.py size-files`):14 px 8417 B、20 px 13362 B、40 px 41641 B;静态 RAM 0 B(字形位图直接从 Flash 读取)。新增中文前先扩充字符清单,并重新生成所有受影响的字号。

## 图片（images）

可复用的源图与生成的显示资产放在 `images/`。

| 文件 | 尺寸与格式 | 用途与来源 |
| --- | --- | --- |
| [`images/home.jpg`](images/home.jpg) | 3840 × 2160，JPEG | 嵌入中英文项目 README 的产品主图，突出 AI Passport 产品形象与开放、人人可创作的理念。 |
| [`images/readme-hardware-specs.png`](images/readme-hardware-specs.png) | 2172 × 724，PNG RGBA | 保留为可选技术参考图，不再用于首页主视觉。于 2026-09-17 使用内置图像生成工具为本仓库生成；已根据文档中的硬件能力契约核对图中的六项标签与参数。 |
| [`images/logo-wordmark.png`](images/logo-wordmark.png) | 1648 × 336，PNG RGBA | 从仓库原始 `images/logo.png` 中精确裁切并去除背景的黑色字标；用于中英文项目 README 的浅色主题。 |
| [`images/logo-wordmark-dark.png`](images/logo-wordmark-dark.png) | 1648 × 336，PNG RGBA | 提取字标的白色版本；README 使用 `<picture>` 在 GitHub 深色主题下显示。 |

- 使用描述性命名，并记录尺寸、像素格式、转换步骤与目标路径。
- 优先采用适合 240 × 320 RGB565 显示的格式，并纳入 Flash 与内部 RAM 考量。
- 许可允许时保留可编辑源文件，并记录来源与许可。
- 图片中不得包含设备二维码秘密、凭证或个人数据。

## 音乐与音效（music）

可复用的音乐与音效源码放在 `music/`。

- 记录来源、许可、采样率、位深、声道、转换命令与目标路径。
- 与当前 BSP 音频路径匹配时优先采用 16 kHz、16 位单声道 PCM。
- 嵌入音频前评估 Flash 与内部 RAM 成本；长录音应流式或分块。
- 无再分发许可不提交媒体文件。
