<p align="right">
  <a href="README.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# Assets

This directory stores reusable fonts, images, music, and sound effects, organized by asset type.

Keep each asset in the matching subdirectory and document its destination, naming, integration method, and source/license. Do not mix binary assets with Markdown documentation.

## Fonts

Store reusable font files and generated font sources in `fonts/`.

- Use descriptive names that include the family, weight, size, and format when relevant.
- Document the source, license, character range, conversion command, and expected destination.
- Check Flash and internal-RAM impact before adding a font; the ESP32-C3 has no PSRAM.
- Do not commit fonts whose license does not permit redistribution.

### Generated Chinese UI subsets

The PC status monitor (`main/app_pcmon.c`) renders a few Chinese words. The default
Montserrat fonts have no Chinese glyphs, so these subsets were generated from
[Source Han Sans SC](https://github.com/adobe-fonts/source-han-sans) (Normal), which is
distributed under the SIL Open Font License 1.1. The OTF itself is **not** committed; it
ships with LVGL at `managed_components/lvgl__lvgl/scripts/built_in_font/SourceHanSansSC-Normal.otf`.

| File | Coverage | Use |
| --- | --- | --- |
| [`fonts/app_font_14.c`](fonts/app_font_14.c) | ASCII `0x20-0x7E` + 29 Chinese code points, 14 px, 4 bpp | Page titles (top bar). |
| [`fonts/app_font_20.c`](fonts/app_font_20.c) | Same coverage, 20 px, 4 bpp | Medium Chinese values (AGENT page `psutil` flag). |
| [`fonts/app_font_40.c`](fonts/app_font_40.c) | Same coverage, 40 px, 4 bpp | Large Chinese status word on the AGENT page. |

- Character inventory (Unicode): `U+6838 U+5FC3 U+5904 U+7406 U+5668 U+5185 U+5B58 U+78C1 U+76D8 U+7F51 U+7EDC U+8D8B U+52BF U+7CFB U+7EDF U+4EE3 U+672A U+8FDE U+63A5 U+6B63 U+5E38 U+662F U+5426 U+8FDB U+7A0B U+7AEF U+53E3 U+663E U+5361`.
- Generator: [`lv_font_conv`](https://github.com/lvgl/lv_font_conv) 1.5.3 on Node.js v20.12.0.
- Regeneration (from the repository root; repeat for each size):

  ```bash
  npx lv_font_conv@1.5.3 \
    --font managed_components/lvgl__lvgl/scripts/built_in_font/SourceHanSansSC-Normal.otf \
    --range 0x20-0x7E,0x6838,0x5FC3,0x5904,0x7406,0x5668,0x5185,0x5B58,0x78C1,0x76D8,0x7F51,0x7EDC,0x8D8B,0x52BF,0x7CFB,0x7EDF,0x4EE3,0x672A,0x8FDE,0x63A5,0x6B63,0x5E38,0x662F,0x5426,0x8FDB,0x7A0B,0x7AEF,0x53E3,0x663E,0x5361 \
    --size 14 --bpp 4 --format lvgl --no-compress \
    --lv-font-name app_font_14 --lv-include lvgl.h \
    --output assets/fonts/app_font_14.c
  ```

- Flash (from `idf.py size-files`): 14 px 9030 B, 20 px 14483 B, 40 px 45629 B; 0 B static RAM (bitmaps are read from Flash). Extend the code-point inventory before adding new Chinese text, then regenerate every affected size.

## Images

Store reusable source images and generated display assets in `images/`.

| File | Dimensions and format | Use and source |
| --- | --- | --- |
| [`images/home.jpg`](images/home.jpg) | 3840 × 2160, JPEG | Product hero image embedded in both project README files to foreground AI Passport and its open, maker-oriented identity. |
| [`images/readme-hardware-specs.png`](images/readme-hardware-specs.png) | 2172 × 724, PNG RGBA | Optional technical infographic retained as a reference asset; it is no longer used as the homepage hero. Generated for this repository with the built-in image generation tool on 2026-09-17; the six labels and values were checked against the documented hardware contract. |
| [`images/logo-wordmark.png`](images/logo-wordmark.png) | 1648 × 336, PNG RGBA | Transparent black wordmark extracted from the repository's original `images/logo.png`; embedded in both project README files for light backgrounds. |
| [`images/logo-wordmark-dark.png`](images/logo-wordmark-dark.png) | 1648 × 336, PNG RGBA | White version of the extracted wordmark, used by the README `<picture>` element when GitHub is in dark mode. |

- Use descriptive names and document dimensions, pixel format, conversion steps, and destination.
- Prefer formats suitable for the 240 × 320 RGB565 display and account for Flash and internal RAM.
- Preserve editable sources where licensing permits, and record the source and license.
- Never commit device QR secrets, credentials, or personal data in images.

## Music and sound effects

Store reusable music and sound-effect sources in `music/`.

- Document the source, license, sample rate, bit depth, channels, conversion command, and destination.
- Prefer 16 kHz, 16-bit mono PCM when it matches the current BSP audio path.
- Check Flash and internal-RAM cost before embedding audio; stream or chunk long recordings.
- Do not commit media without redistribution permission.
