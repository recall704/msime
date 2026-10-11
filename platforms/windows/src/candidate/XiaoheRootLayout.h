#pragma once
#include "ShuangpinKeymapLayout.h"
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace msime::windows {
// 小鹤音形字根提示：双拼组字时在候选窗旁直接显示一张小鹤音形的字根键位图（platforms/windows/resources/xhyx.png，作为 RCDATA 嵌入 Server，
// 权利归小鹤方案作者），与「双拼键位提示」是两块相互独立的浮窗。这里只做纯计算：开关怎么读、这一帧要不要显示。

// 共享偏好 `xiaohe_root_hint`：没选过时文档里没有这一项，按关处理；不是布尔值时同样按关。
inline bool xiaohe_root_preference(const nlohmann::json &preferences) {
  if (!preferences.is_object())
    return false;
  const auto value = preferences.find("xiaohe_root_hint");
  return value != preferences.end() && value->is_boolean() && value->get<bool>();
}

// 从 Engine 的 view 判断这一帧要不要显示字根图：条件与双拼键位提示相同（双拼方案、没有局部模式、不在 Engine 英文模式、正在组字）；
// 与方案名无关——小鹤形码可以配任何一种双拼。开关和候选窗是否可见由调用方另外判断。
inline std::optional<ShuangpinKeymapHint> xiaohe_root_hint(const nlohmann::json &view) {
  if (!view.is_object())
    return std::nullopt;
  const auto scheme = view.find("scheme");
  const auto mode = view.find("local_mode");
  const auto english = view.find("dedicated_english");
  const auto editing = view.find("editing_text");
  if (scheme == view.end() || !scheme->is_number_integer() || scheme->get<int64_t>() != 1 ||
      mode == view.end() || !mode->is_string() || mode->get_ref<const std::string &>() != "none" ||
      english == view.end() || !english->is_boolean() || english->get<bool>() ||
      editing == view.end() || !editing->is_string() || editing->get_ref<const std::string &>().empty())
    return std::nullopt;
  return ShuangpinKeymapHint{"xiaohe", 0};
}
} // namespace msime::windows
