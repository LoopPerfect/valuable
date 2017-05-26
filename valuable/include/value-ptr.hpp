#ifndef VALUABLE_VALUE_PTR_HPP
#define VALUABLE_VALUE_PTR_HPP
#include <memory>

namespace valuable {

struct DefaultCloner {
  DefaultCloner() = default;
  template <class T> T *operator()(T const &x) const { return new T(x); }
};

template <class T, class Cloner = DefaultCloner> struct value_ptr : private Cloner {
  std::unique_ptr<T> ptr;
  Cloner &cloner() { return *this; }
  Cloner const &cloner() const { return *this; };
  T *clone(T const &x) const { return cloner()(x); }

  value_ptr(Cloner const &cloner = Cloner()) : Cloner{cloner} {}

  value_ptr(T const &value, Cloner const &cloner = Cloner())
      : Cloner{cloner}, ptr{clone(value)} {}

  value_ptr(value_ptr &&v)
      : Cloner{std::move(v)}, ptr{std::move(v.ptr)} {}

  value_ptr(value_ptr const &v) : Cloner{v} {
    if (v) {
      ptr.reset(clone(*v));
    }
  }

  T *get() { return ptr.get(); }
  T const *get() const { return ptr.get(); }

  T &operator*() { return *ptr; }
  T const &operator*() const { return *ptr; }

  value_ptr<T> &operator=(value_ptr &&v) {
    ptr = std::move(v.ptr);
    cloner() = std::move(v);
    return *this;
  }

  value_ptr<T, Cloner> clone() const { return *this; }

  T const *operator->() const { return get(); }

  T *operator->() { return get(); }

  value_ptr<T> &operator=(value_ptr const &v) {
    cloner() = v;
    ptr.reset(clone(*v));

    return *this;
  }

  operator bool() const { return !!ptr; }
  ~value_ptr() = default;
};
}

#endif
