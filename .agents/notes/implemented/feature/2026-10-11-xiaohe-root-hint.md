# Agent Note: 小鹤音形字根提示（Windows）

Status: implemented

## Problem

用小鹤双拼 + 小鹤形码（辅助码）打字的人没有一处随时可看的字根对照：已有的「输入时显示双拼键位提示」只画声母 / 韵母，不画形码字根。小鹤官网上有一张排好版的「字根键位图」，用户希望双拼组字时能像键位提示那样把它显示出来。

## Decision

只做 Windows，做成一块与「双拼键位提示」相互独立、共用同一个窗口类（`ShuangpinKeymapWindow`，靠 `KeymapKind::{Shuangpin,XiaoheRoot}` 分支）的浮窗：

- **开关**：共享偏好 `xiaohe_root_hint`（`Option<bool>`，没写过不写进文档，缺省关），与 `shuangpin_keymap_hint` 并列；能力位 `HostCapabilities::xiaohe_root_hint` 只对 Windows 为真；WinUI 设置「输入方案 › 双拼方案」下多一行「输入时显示小鹤音形字根」。显示条件与双拼键位提示相同（scheme 为 1、`local_mode` 为 `none`、不在 Engine 英文模式、正在组字），但**与方案名无关**——小鹤形码可以配任何一种双拼。
- **内容**：不逐键画文本字根，直接画小鹤官方的字根键位图 `platforms/windows/resources/xhyx.png`（1285×555）。图片以 RCDATA（`IDR_MSIME_XHYX`）嵌进 Server 的 `ServerResources.rc`，`DeviceResources::GetBitmapFromMemory` 从内存解码，不依赖安装器把图片放到某个运行期目录。面板复用 620×203 DIP 的键位图卡片尺寸（`ShuangpinKeymapMetrics` 默认值），图片等比缩放到卡片内并居中，两侧留白而不拉伸。摆放复用 `shuangpin_keymap_placement`（候选窗离光标远的一侧）。

## Alternatives considered

- **逐键画文本字根（第一版）。** 与双拼键位表同构：三排键帽，每键列出该键承载的字根，高亮刚按下的键。放弃是因为官方字根表里有部首补充区的字形（`⺈ ⺮ ⺷ ⺧ ⺍ ⺌ ⺗ ⻊`）和「比左部 / 见下部 / 余下部 / 官下部 / 求、具其上部 / 寿、带字头」这类文字描述：要么另配字体，要么逐条改写成通行字形，渲染与保真都麻烦。`.agents/notes/implemented/feature/2026-10-11-wubi-reverse-lookup.md` 记过同一类问题（五笔字根拆分不做）。官方已经有一张排好版的图，直接画图更准、更省，也不用维护一份会漂移的字根表。
- **把字根表放进 Engine，经 host-api 发布（像 `msime_client_shuangpin_key_hints` 那样）。** 双拼键位表在引擎是因为它由引擎自己的方案表展开、必须跟方案一致；字根是静态的方案定义、只有 Windows 画，进引擎等于为一个平台加一条跨平台契约，没有回报。
- **从磁盘路径加载图片（`GetBitmapFromFile`）。** 需要安装器把 `xhyx.png` 复制到 Server 的运行期目录并在打包脚本里维护这份清单。RCDATA 嵌入不需要安装布局，删掉图片只在一处。

## Consequences

- **收益**：Windows 的双拼用户能随时看到小鹤音形字根图，且能与双拼键位提示同时打开，两者互不影响。
- **代价与已知上限**：字根图版权归小鹤方案作者，作为可选的显示资源内置，`platforms/windows/resources/NOTICE.md` 写明移除方式；它是官方排版的位图，不能像文本那样随主题换色或高亮当前键；宽高比（2.31）与键位图卡片（3.05）不同，按「同尺寸」缩放后左右各留约 75 DIP 空白。整个改动**没有在本机编译或真机验证**——开发环境缺少 MSVC / Rust / CMake 工具链，只能靠 fork 的 CI 或本地构建确认。

## Verification

- `windows-xiaohe-root-layout`（`platforms/windows/tests/candidate/xiaohe_root_layout.cpp`）：偏好读取（缺省关、非布尔关）与显示条件（双拼、无局部模式、非英文、有组字；与方案名无关）。
- 复用既有的 `windows-shuangpin-keymap-layout`（几何 / 摆放）不受影响；`ShuangpinKeymapWindow` 的改动只加了 `KeymapKind` 分支，未改双拼路径的数据来源。
- `scripts/test-preferences-field-parity.py`：`xiaohe_root_hint` 是共享设置页够不到的 Windows 专属项，登记在 `RUST_ONLY`。
