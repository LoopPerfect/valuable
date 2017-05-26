#ifndef VALUABLE_VALUE_PTR_HPP
#define VALUABLE_VALUE_PTR_HPP
#include <memory>

namespace valuable {

struct DefaultCloner {
  DefaultCloner() = default;
  template <class T> T *operator()(T const &x) const { return new T(x); }
};

template <class T, class Cloner = DefaultCloner> struct value_ptr {
  std::unique_ptr<T> ptr;
  Cloner cloner;

  value_ptr(Cloner const &cloner = Cloner()) : cloner(cloner) {}

  value_ptr(T const &value, Cloner const &cloner = Cloner())
      : ptr{cloner(value)}, cloner(cloner) {}

  value_ptr(value_ptr &&v)
      : ptr{std::move(v.ptr)}, cloner{std::move(v.cloner)} {}

  value_ptr(value_ptr const &v) : cloner(v.cloner) {
    if (v) {
      ptr.reset(cloner(*v.ptr));
    }
  }

  T *get() { return ptr.get(); }
  T const *get() const { return ptr.get(); }

  T &operator*() { return *ptr; }
  T const &operator*() const { return *ptr; }

  value_ptr<T> &operator=(value_ptr &&v) {
    ptr = move(v.ptr);
    cloner = move(v.cloner);
    return *this;
  }

  value_ptr<T, Cloner> clone() const { return *this; }

  T const *operator->() const { return get(); }

  T *operator->() { return get(); }

  value_ptr<T> &operator=(value_ptr const &v) {
    cloner = v.cloner;
    ptr.reset(cloner(*v.get()));

    return *this;
  }

  operator bool() const { return !!ptr; }
  ~value_ptr() = default;
};
}

#endif
