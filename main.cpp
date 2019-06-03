#include <functional>
#include <utility>
#include <type_traits>

#include <iostream>
#include <string>

namespace
{

template<typename Function, typename Tuple, std::size_t ...Is>
void DestructImpl(const Function& f, const Tuple& t, std::index_sequence<Is...>)
{
  f(std::forward<decltype(std::get<Is>(t))>(std::get<Is>(t))...);
}

template<template<typename> class F, typename T, typename ...Args, typename Indices = std::make_index_sequence<sizeof...(Args)>>
void Destruct(const F<T(Args...)>& func, const std::tuple<typename std::decay<Args>::type...>& args)
{
  DestructImpl(func, args, Indices{});
}
}

template<typename T> struct Listener;
template<template<typename> class F, typename T, typename ...Args>
class Listener<F<T(Args...)>>
{
public:
  Listener(F<T(Args...)>&& cb) : _cb(std::move(cb)) {}
  void UpdateCallback(F<T(Args...)>&& cb)
  {
    _cb = std::move(cb);
    Apply();
  }
  void Notify(Args&&... args)
  {
    _cache = std::make_tuple(std::forward<Args>(args)...);
    Apply();
  }
private:
  void Apply() const
  {
    Destruct(_cb, _cache);
  }
  F<T(Args...)> _cb;
  std::tuple<typename std::decay<Args>::type...> _cache;
};

struct MyStruct
{
  auto CreateListener()
  {
    std::function<void(const std::string&, int, int)> lambda = [this](const std::string& a, int b, int c) { SomeCallback(a, b, c); };
    return Listener<decltype(lambda)> { std::move(lambda) };
  }

  void SomeCallback(const std::string& a, int b, int c)
  {
    std::cout << a << b + c + d << std::endl;
  }
  int d = 10;
};


int main()
{
  MyStruct s;
  auto listener = s.CreateListener();

  listener.Notify("aaaa", 2, 3);
  return 0;
}
