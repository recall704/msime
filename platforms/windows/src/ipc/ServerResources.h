// Resource ids for the Server executable. Included from ServerResources.rc as
// well as from C++, so nothing in here may be anything but a #define.
#ifndef MSIME_SERVER_RESOURCES_H
#define MSIME_SERVER_RESOURCES_H

// The product mark. Id 1 on purpose: Windows shows the icon with the lowest id
// as the executable's own icon, so the Server gets the right icon in Task
// Manager for free rather than the blank default.
#define IDI_MSIME_LOGO 1

// 小鹤音形字根键位图（resources/xhyx.png），双拼组字时直接画在候选窗旁。作为 RCDATA 嵌入 Server，
// 免得依赖安装器把图片复制到某个运行期目录。
#define IDR_MSIME_XHYX 2

#endif // MSIME_SERVER_RESOURCES_H
