#ifndef VALUABLE_VALUE_PTR_HPP
#define VALUABLE_VALUE_PTR_HPP
#include <memory>

namespace valuable {

namespace detail {
  template <class T, class T2> struct compressed_ptr : std::unique_ptr<T>, T2 {
    using T1 = std::unique_ptr<T>;
    compressed_ptr() = default;
    compressed_ptr(T *a) : T1(a) {}
    compressed_ptr(T1 const &a1) : T1(a1) {}
    compressed_ptr(T1 &&a1) : T1(std::move(a1)) {}
    compressed_ptr(T2 const &a2) : T2(a2) {}
    compressed_ptr(T2 &&a2) : T2(std::move(a2)) {}
    compressed_ptr(T* a, T2 const &a2) : T1(a), T2(a2) {}
    compressed_ptr(T* a, T2 &&a2) : T1(a), T2(std::move(a2)) {}
    compressed_ptr(T1 const &a1, T2 const &a2) : T1(a1), T2(a2) {}
    compressed_ptr(T1 &&a1, T2 &&a2) : T1(std::move(a1)), T2(std::move(a2)) {}
    compressed_ptr(const compressed_ptr &) = default;
    compressed_ptr(compressed_ptr &&) = default;
  };
}

struct DefaultCloner {
  DefaultCloner() = default;
  template <class T> T *operator()(T const &x) const { return new T(x); }
};

template <class T, class Cloner = DefaultCloner> struct value_ptr {
  detail::compressed_ptr<T, Cloner> ptr_;
  
  std::unique_ptr<T> &ptr() { return ptr_; }
  std::unique_ptr<T> const &ptr() const { return ptr_; }
  Cloner &cloner() { return ptr_; }
  Cloner const &cloner() const { return ptr_; };
  T *clone(T const &x) const { return cloner()(x); }

  value_ptr() = default;
  
  value_ptr(Cloner const &cloner)
     : ptr_{cloner} {}

  value_ptr(T const &value, Cloner const &cloner = {})
      : ptr_{cloner(value), cloner} {}

  value_ptr(value_ptr &&v) = default;

  value_ptr(value_ptr const &v) : ptr_{v.cloner()} {
    if (v) {
      ptr().reset(clone(*v));
    }
  }

  T *get() { return ptr().get(); }
  T const *get() const { return ptr().get(); }

  T &operator*() { return *get(); }
  T const &operator*() const { return *get(); }

  value_ptr<T> &operator=(value_ptr &&v) {
    ptr() = std::move(v.ptr());
    cloner() = std::move(v.cloner());
    return *this;
  }

  value_ptr<T, Cloner> clone() const { return *this; }

  T const *operator->() const { return get(); }

  T *operator->() { return get(); }

  value_ptr<T> &operator=(value_ptr const &v) {
    ptr().reset(v.cloner()(*v));
    cloner() = v.cloner();
    return *this;
  }

  operator bool() const { return !!ptr(); }
  ~value_ptr() = default;
};
}

#endif
