#include "XiaoheRootLayout.h"
#include <iostream>
#include <stdexcept>
#include <string>

using namespace msime::windows;
namespace {
[[noreturn]] void require_failed(int line) {
  throw std::runtime_error("Xiaohe root layout test failed at line " + std::to_string(line));
}
#define require(value)                                                         \
  do {                                                                         \
    if (!(value))                                                              \
      require_failed(__LINE__);                                                \
  } while (false)

nlohmann::json view(const std::string &editing) {
  return nlohmann::json{{"scheme", 1},
                        {"local_mode", "none"},
                        {"dedicated_english", false},
                        {"shuangpin_profile", "xiaohe"},
                        {"editing_text", editing}};
}

void preference() {
  // 没选过时文档里没有这一项，按关处理。
  require(!xiaohe_root_preference(nlohmann::json::object()));
  require(xiaohe_root_preference(nlohmann::json{{"xiaohe_root_hint", true}}));
  require(!xiaohe_root_preference(nlohmann::json{{"xiaohe_root_hint", false}}));
  require(!xiaohe_root_preference(nlohmann::json{{"xiaohe_root_hint", "yes"}}));
  require(!xiaohe_root_preference(nlohmann::json()));
}

void hint_follows_the_view() {
  // 双拼组字时显示；没有组字、不是双拼、局部模式、Engine 的英文模式时都不显示。
  require(xiaohe_root_hint(view("ni")));
  require(!xiaohe_root_hint(view("")));
  auto other = view("ni");
  other["scheme"] = 0;
  require(!xiaohe_root_hint(other));
  other = view("ni");
  other["local_mode"] = "unicode";
  require(!xiaohe_root_hint(other));
  other = view("ni");
  other["dedicated_english"] = true;
  require(!xiaohe_root_hint(other));
  // 与方案名无关：小鹤形码可以配任何一种双拼，自然码、微软、不认识的方案都显示。
  other = view("ni");
  other["shuangpin_profile"] = "microsoft";
  require(xiaohe_root_hint(other));
  other = view("ni");
  other["shuangpin_profile"] = "unknown";
  require(xiaohe_root_hint(other));
  other = view("ni");
  other.erase("shuangpin_profile");
  require(xiaohe_root_hint(other));
  require(!xiaohe_root_hint(nlohmann::json::array()));
}
} // namespace

int main() {
  try {
    preference();
    hint_follows_the_view();
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
  return 0;
}
