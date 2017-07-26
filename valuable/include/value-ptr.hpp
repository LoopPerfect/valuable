#ifndef VALUABLE_VALUE_PTR_HPP
#define VALUABLE_VALUE_PTR_HPP
#include <memory>

#ifdef _MSC_VER
#define VALUABLE_DECLSPEC_EMPTY_BASES __declspec(empty_bases)
#else
#define VALUABLE_DECLSPEC_EMPTY_BASES
#endif

namespace valuable {

namespace detail {
  struct spacer {};
  // For details of class_tag use here, see
  // https://mortoray.com/2013/06/03/overriding-the-broken-universal-reference-t/
  template <typename T> struct class_tag {};

  template <class T, class Deleter, class T2>
  struct VALUABLE_DECLSPEC_EMPTY_BASES compressed_ptr : std::unique_ptr<T, Deleter>, T2 {
    using T1 = std::unique_ptr<T, Deleter>;
    compressed_ptr() = default;
    compressed_ptr(compressed_ptr &&) = default;
    compressed_ptr(const compressed_ptr &) = default;
    compressed_ptr(T2 &&a2) : T2(std::move(a2)) {}
    compressed_ptr(const T2 &a2) : T2(a2) {}
    template <typename A1>
    compressed_ptr(A1 &&a1)
        : compressed_ptr(std::forward<A1>(a1), class_tag<typename std::decay<A1>::type>(),
                         spacer(), spacer()) {}
    template <typename A1, typename A2>
    compressed_ptr(A1 &&a1, A2 &&a2)
        : compressed_ptr(std::forward<A1>(a1), std::forward<A2>(a2),
                         class_tag<typename std::decay<A2>::type>(), spacer()) {}
    template <typename A1, typename A2, typename A3>
    compressed_ptr(A1 &&a1, A2 &&a2, A3 &&a3)
        : T1(std::forward<A1>(a1), std::forward<A2>(a2)), T2(std::forward<A3>(a3)) {}

    template <typename A1>
    compressed_ptr(A1 && a1, class_tag<typename std::decay<A1>::type>, spacer, spacer)
        : T1(std::forward<A1>(a1)) {}
    template <typename A1, typename A2>
    compressed_ptr(A1 &&a1, A2 &&a2, class_tag<Deleter>, spacer)
        : T1(std::forward<A1>(a1), std::forward<A2>(a2)) {}
    template <typename A1, typename A2>
    compressed_ptr(A1 &&a1, A2 &&a2, class_tag<T2>, spacer)
        : T1(std::forward<A1>(a1)), T2(std::forward<A2>(a2)) {}
  };
}

template <typename T>
struct default_clone {
  default_clone() = default;
  T *operator()(T const &x) const { return new T(x); }
  T *operator()(T &&x) const { return new T(std::move(x)); }
};

template <class T, class Cloner = default_clone<T>, class Deleter = std::default_delete<T> >
class value_ptr {
  detail::compressed_ptr<T, Deleter, Cloner> ptr_;

  std::unique_ptr<T, Deleter> &ptr() { return ptr_; }
  std::unique_ptr<T, Deleter> const &ptr() const { return ptr_; }

  T *clone(T const &x) const { return get_cloner()(x); }

public:
  using pointer = T*;
  using element_type = T;
  using cloner_type = Cloner;
  using deleter_type = Deleter;

  value_ptr() = default;

  value_ptr(const T &value) : ptr_(cloner_type()(value)) {}
  value_ptr(T &&value) : ptr_(cloner_type()(std::move(value))) {}

  value_ptr(const Cloner &value) : ptr_(value) {}
  value_ptr(Cloner &&value) : ptr_(value) {}

  template<typename V, typename ClonerOrDeleter>
  value_ptr(V &&value, ClonerOrDeleter &&a2)
      : ptr_(std::forward<V>(value), std::forward<ClonerOrDeleter>(a2)) {}

  template<typename V, typename C, typename D>
  value_ptr(V &&value, C &&cloner, D &&deleter)
      : ptr_(std::forward<V>(value), std::forward<D>(deleter), std::forward<C>(cloner)) {}

  value_ptr(value_ptr const &v) : ptr_{nullptr, v.get_cloner()} {
    if (v) {
      ptr().reset(clone(*v));
    }
  }
  value_ptr(value_ptr &&v) = default;

  explicit value_ptr(pointer value) : ptr_(value) {}
  pointer release() {
    return ptr().release();
  }

  T *get() noexcept { return ptr().get(); }
  T const *get() const noexcept { return ptr().get(); }

  Cloner &get_cloner() noexcept { return ptr_; }
  Cloner const &get_cloner() const noexcept { return ptr_; };

  Deleter &get_deleter() noexcept { return ptr_; }
  Deleter const &get_deleter() const noexcept { return ptr_; }

  T &operator*() { return *get(); }
  T const &operator*() const { return *get(); }

  T const *operator->() const noexcept { return get(); }
  T *operator->() noexcept { return get(); }

  value_ptr<T> &operator=(value_ptr &&v) {
    ptr() = std::move(v.ptr());
    get_cloner() = std::move(v.get_cloner());
    return *this;
  }

  value_ptr<T> &operator=(value_ptr const &v) {
    ptr().reset(v.get_cloner()(*v));
    get_cloner() = v.get_cloner();
    return *this;
  }

  operator bool() const noexcept { return !!ptr(); }
  ~value_ptr() = default;
};
}

#undef VALUABLE_DECLSPEC_EMPTY_BASES

#endif
