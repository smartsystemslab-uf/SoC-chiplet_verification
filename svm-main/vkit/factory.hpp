// vkit/factory.hpp
#pragma once
#include <functional>
#include <unordered_map>
#include <string>

template<class T>
struct factory {
  using ctor_t = std::function<T*(const std::string& name)>;
  std::unordered_map<std::string, ctor_t> map;
  void reg(const std::string& key, ctor_t c) { map[key]=c; }
  T* make(const std::string& key, const std::string& name) const {
    auto it = map.find(key);
    if (it==map.end()) return nullptr;
    return (it->second)(name);
  }
};
