#include <gtest/gtest.h>
#include <valuable/value-ptr.hpp>

using namespace valuable;

TEST(value_ptr, lifetime) {

  static int constructions = 0;
  static int destructions = 0;
  static int moves = 0;
  static int copys = 0;

  struct Sideeffect {
    Sideeffect() { ++constructions; }

    Sideeffect(Sideeffect const &) { ++copys; }

    Sideeffect(Sideeffect &&) { ++moves; }

    ~Sideeffect() { ++destructions; }
  };

  {
    value_ptr<Sideeffect> x = Sideeffect();
    ASSERT_EQ(constructions, 1);
    ASSERT_EQ(destructions, 1);
    ASSERT_EQ(copys, 1);
    ASSERT_EQ(moves, 0);

    value_ptr<Sideeffect> y = x;

    ASSERT_EQ(constructions, 1);
    ASSERT_EQ(destructions, 1);
    ASSERT_EQ(copys, 2);

    value_ptr<Sideeffect> z;
    ASSERT_FALSE((bool)z);
    ASSERT_TRUE(constructions == 1);
    ASSERT_TRUE(copys == 2);
    z = x;
    ASSERT_TRUE(constructions == 1);
    ASSERT_TRUE(copys == 3);

    ASSERT_EQ(moves, 0);
    value_ptr<Sideeffect> m = std::move(z);
    ASSERT_FALSE((bool)z);
    ASSERT_TRUE((bool)m);
    // yes this is correct because we move the pointer not the value
    ASSERT_EQ(moves, 0);
  }

  ASSERT_EQ(destructions, 4);
}

TEST(value_ptr, cloner_transfer) {
  struct Cloner : DefaultCloner {
    int data = -1;
  };
  {
    Cloner c;
    c.data = 10;
    ASSERT_EQ(c.data, 10);

    value_ptr<int, Cloner> y;
    ASSERT_EQ(y.cloner().data, -1);
    
    value_ptr<int, Cloner> z1;
    ASSERT_EQ(z1.cloner().data, -1);
    value_ptr<int, Cloner> z2{c};
    ASSERT_EQ(z2.cloner().data, c.data);
    value_ptr<int, Cloner> z3(z2);
    ASSERT_EQ(z3.cloner().data, z2.cloner().data);
  }
}


TEST(value_ptr, empty_default_cloner) {
  {
    value_ptr<int> v;
    ASSERT_EQ(sizeof(v), sizeof(std::unique_ptr<int>));
  }
}