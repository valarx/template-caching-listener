#include <functional>
#include <iostream>
#include <utility>

namespace
{

template<typename Function, typename Tuple, std::size_t ...Is>
void DestructImpl(const Function& f, const Tuple& t, std::index_sequence<Is...>)
{
  f(std::get<Is>(t)...);
}

template<template<typename> class F, typename T, typename ...Args, typename Indices = std::make_index_sequence<sizeof...(Args)>>
void Destruct(F<T(Args...)> func, std::tuple<Args...> args)
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
    _cache = std::make_tuple(args...);
    Apply();
  }
private:
  void Apply() const
  {
    Destruct(_cb, _cache);
  }
  F<T(Args...)> _cb;
  std::tuple<Args...> _cache;
};

int main()
{
  std::function<void(int)> func = [](int a) { std::cout << a << std::endl; };
  Listener<decltype(func)> cb(std::move(func));
  cb.Notify(5);
  return 0;
}
