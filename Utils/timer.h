#pragma once
#include <chrono>
namespace Utils {
class Timer {
 public:
  void Start() { start_ = std::chrono::high_resolution_clock::now(); }
  void End() { end_ = std::chrono::high_resolution_clock::now(); }
  float Get() {
    std::chrono::duration<float> diff = end_ - start_;
    return diff.count();
  }

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
  std::chrono::time_point<std::chrono::high_resolution_clock> end_;
};
}  // namespace Utils