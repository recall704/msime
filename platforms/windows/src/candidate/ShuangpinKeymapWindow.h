#pragma once
#include "CandidatePalette.h"
#include "ComponentFailure.h"
#include "ShuangpinKeymapLayout.h"
#include <optional>
#include <string>
// 先包含 windows.h：它的 DrawText 宏要在 Direct2D 的声明之前生效。
#include <windows.h>
#include <msimeui/DeviceResources.h>

namespace msime::windows {
class CandidateWindow;
// 一帧键位提示和它要跟随的光标锚点（TSF 报来的光标所在行左下角，物理像素）。
struct ShuangpinKeymapFrame {
  ShuangpinKeymapHint hint;
  long anchor_x = 0;
  long anchor_y = 0;
};

// 画哪张键位图：双拼的声母/韵母，还是小鹤音形的形码字根。两者共用同一个窗口类，只是内容、标题和高度不同。
enum class KeymapKind { Shuangpin, XiaoheRoot };

// 键位提示浮窗，对应 macOS 的 MSIMEShuangpinKeymapPanel：双拼组字时贴着候选窗显示一块键位图。不抢焦点、不接收鼠标，配色取候选窗的调色板；双拼模式画三排键帽（键位表经 host-api 取自 Engine），小鹤模式直接画内嵌的字根键位图。只在 Server 的界面线程上使用；画不出来时自己隐藏，Server 去掉这块浮窗继续运行。
class ShuangpinKeymapWindow final {
public:
  ShuangpinKeymapWindow();
  ~ShuangpinKeymapWindow();
  ShuangpinKeymapWindow(const ShuangpinKeymapWindow &) = delete;
  ShuangpinKeymapWindow &operator=(const ShuangpinKeymapWindow &) = delete;
  // 选定这张窗口画双拼键位还是小鹤字根图；换种类后下一次 update 会按新种类重建内容。
  void set_kind(KeymapKind kind);
  // 与候选窗同一份调色板，主题和皮肤换了跟着换。
  void set_palette(CandidatePalette palette);
  // 主循环每轮在候选窗 refresh 之后调用：`frame` 为空（开关关着、不在双拼组字）或候选窗不可见时隐藏，否则贴着候选卡片显示。贴的是卡片本身（CandidateWindow::card_on_screen），不是带透明阴影边距的窗口外框。
  void update(const std::optional<ShuangpinKeymapFrame> &frame, const CandidateWindow &candidates);
  void hide();
  bool failed() const { return failed_; }
  const std::optional<ComponentFailureSite> &failure_site() const { return failure_site_; }

private:
  static LRESULT CALLBACK procedure(HWND, UINT, WPARAM, LPARAM) noexcept;
  // 换了方案（或从小鹤字根换回来）时重新取内容：双拼取这个方案的键位和零声母说明，小鹤字根只置一个身份（图在绘制时载入）。
  void load_profile(const std::string &profile);
  // 取内嵌的小鹤字根图，失败返回 false（绘制时据此隐藏）。
  bool ensure_image();
  void paint();
  void fail(ComponentFailureSite site);
  msimeui::DeviceResources device_;
  CandidatePalette palette_;
  ShuangpinKeymapMetrics metrics_;
  KeymapKind kind_ = KeymapKind::Shuangpin;
  HWND window_ = nullptr;
  // 已载入内容的身份（"shuangpin:xiaohe" 或 "xiaohe-root"），与请求相同且已有内容时不再重建。
  std::string loaded_identity_;
  // 上一次传进来的原始方案名，用来判断内容是否变化（高亮键之外）。
  std::string last_profile_;
  ShuangpinKeymapRows rows_;
  std::wstring title_;
  std::wstring footer_;
  // 小鹤字根图，按需从 RCDATA 解码；设备重建后置空重取。
  ID2D1Bitmap *image_ = nullptr;
  char highlighted_ = 0;
  // 上次摆放的窗口位置和尺寸，没变时不再挪动窗口。
  RECT placed_{};
  bool failed_ = false;
  std::optional<ComponentFailureSite> failure_site_;
};
} // namespace msime::windows
