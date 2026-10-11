# 小鹤音形字根键位图

`xhyx.png` 是「小鹤音形」（小鹤双拼 + 小鹤形）的字根键位图，双拼组字时由 Windows Server 直接画在候选窗旁
（共享偏好 `xiaohe_root_hint`，见 `../README.md` 的「双拼键位提示」一节）。它以 RCDATA 嵌进 Server
（`../ServerResources.rc`），不随安装包另存。

图片与其中的字根、键位安排版权归小鹤方案作者（https://flypy.cc）。本仓库只把它作为可选的显示资源内置，
未作改动；若方案作者要求移除，删掉本目录与 `ServerResources.rc` 里的 `IDR_MSIME_XHYX` 即可。
