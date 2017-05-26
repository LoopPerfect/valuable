#include <gtest/gtest.h>
#include <valuable/value-ptr.hpp>

using namespace valuable;

TEST(value_ptr, lifetime) {

  static int constructions;
  static int destructions;
  static int moves;
  static int copys;

  struct Sideeffect {
    Sideeffect() { ++constructions; }

    Sideeffect(Sideeffect const &) { ++copys; }

    Sideeffect(Sideeffect &&) { ++moves; }

    ~Sideeffect() { ++destructions; }

    static void reset() {
      constructions = 0;
      destructions = 0;
      moves = 0;
      copys = 0;
    }
  };

  {
    Sideeffect w;
    value_ptr<Sideeffect> x = w;
    ASSERT_EQ(constructions, 1);
    ASSERT_EQ(destructions, 0); // w is still alive
    ASSERT_EQ(copys, 1);
    ASSERT_EQ(moves, 0);

    Sideeffect::reset();
    value_ptr<Sideeffect> y = x;
    ASSERT_EQ(constructions, 0);
    ASSERT_EQ(destructions, 0);
    ASSERT_EQ(copys, 1);
    ASSERT_EQ(moves, 0);

    Sideeffect::reset();
    value_ptr<Sideeffect> z;
    ASSERT_FALSE((bool)z);
    ASSERT_EQ(constructions, 0);
    ASSERT_EQ(destructions, 0);
    ASSERT_EQ(copys, 0);
    ASSERT_EQ(moves, 0);

    Sideeffect::reset();
    z = x;
    ASSERT_EQ(constructions, 0);
    ASSERT_EQ(destructions, 0);
    ASSERT_EQ(copys, 1);
    ASSERT_EQ(moves, 0);

    Sideeffect::reset();
    value_ptr<Sideeffect> m = std::move(z);
    ASSERT_FALSE((bool)z);
    ASSERT_TRUE((bool)m);
    ASSERT_EQ(constructions, 0);
    ASSERT_EQ(destructions, 0);
    ASSERT_EQ(copys, 0);
    ASSERT_EQ(moves, 0); // we move the pointer, not the value

    Sideeffect::reset();
    m = Sideeffect();
    ASSERT_EQ(constructions, 1); // temporary
    ASSERT_EQ(destructions, 2); // value in m and the temporary
    ASSERT_EQ(copys, 0);
    ASSERT_EQ(moves, 1); // we move the value

    Sideeffect::reset();
  }

  ASSERT_EQ(destructions, 4); // w, x, y, m (z is empty)
}

TEST(value_ptr, cloner_transfer) {
  struct Cloner : default_clone<int> {
    int data = -1;
  };
  {
    Cloner c;
    c.data = 10;
    ASSERT_EQ(c.data, 10);

    value_ptr<int, Cloner> y;
    ASSERT_EQ(y.get_cloner().data, -1);
    
    value_ptr<int, Cloner> z1;
    ASSERT_EQ(z1.get_cloner().data, -1);
    value_ptr<int, Cloner> z2{c};
    ASSERT_EQ(z2.get_cloner().data, c.data);
    value_ptr<int, Cloner> z3(z2);
    ASSERT_EQ(z3.get_cloner().data, z2.get_cloner().data);
  }
}

TEST(value_ptr, C_full_api) {
  static int constructions;
  static int destructions;
  static int clones;
  
  struct Value {
    int data;
  };
  // This is a C API that we might wrap
  struct API {
    static Value *create() {
      ++constructions;
      auto val = (Value*)malloc(sizeof(Value));
      val->data = -1;
      return val;
    }
    static Value *clone(const Value *src) {
      ++clones;
      auto dst = (Value*)malloc(sizeof(Value));
      dst->data = src->data;
      return dst;
    }
    static void destroy(Value *val) {
      ++destructions;
      free(val);
    }
    static void reset() {
      constructions = 0;
      destructions = 0;
      clones = 0;
    }
  };
  struct Cloner {
    Value *operator()(const Value &src) const { return API::clone(&src); }
  };
  struct Deleter {
    void operator()(Value *val) const { return API::destroy(val); }
  };
  {
    using Value_ptr = value_ptr<Value, Cloner, Deleter>;
    ASSERT_EQ(sizeof(Value_ptr), sizeof(std::unique_ptr<Value>));

    Value_ptr a(API::create());
    ASSERT_EQ(constructions, 1);
    ASSERT_EQ(destructions, 0);
    ASSERT_EQ(clones, 0);

    API::reset();
    Value_ptr b(std::move(a));
    ASSERT_EQ(clones, 0);

    API::reset();
    Value_ptr c(b);
    ASSERT_EQ(constructions, 0);
    ASSERT_EQ(destructions, 0);
    ASSERT_EQ(clones, 1);
  }

  ASSERT_EQ(destructions, 2); // b, c
}