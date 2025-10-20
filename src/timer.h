#pragma once
#include "fmt/base.h"
#include <chrono>

struct Timer {
  Timer() { Reset(); }
  void Reset() { m_start = std::chrono::high_resolution_clock::now(); }
  float Elapsed() const {
    return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::high_resolution_clock::now() - m_start)
               .count() *
           0.001f * 0.001f;
  }
  float ElapsedMillis() const {
    return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::high_resolution_clock::now() - m_start)
               .count() *
           0.001f;
  }

  std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

#define SCOPED_TIMER(x) ScopedTimer scoped_timer{x};

struct ScopedTimer {
  ScopedTimer(std::string name) {
    m_start = std::chrono::high_resolution_clock::now();
    this->name = name;
  }

  ~ScopedTimer() {
    fmt::println("[ScopedTimer]: {} : {} ms", name, ElapsedMillis());
  }

  float ElapsedMillis() const {
    return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::high_resolution_clock::now() - m_start)
               .count() *
           0.001f;
  }

  std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
  std::string name;
};
