#include <gtest/gtest.h>
#include <valuable/value-ptr.hpp>

using namespace valuable;

TEST(value_ptr, lifetime) {

  static int constructions = 0;
  static int destructions = 0;
  static int moves = 0;
  static int copys = 0;

  struct Sideeffect {
    Sideeffect() {
      ++constructions;
    }

    Sideeffect(Sideeffect const&) {
      ++copys;
    }


    Sideeffect(Sideeffect&&) {
      ++moves;
    }

    ~Sideeffect() {
      ++destructions;
    }
  };

  {
    value_ptr<Sideeffect> x = Sideeffect();
    ASSERT_EQ(constructions, 1);
    ASSERT_EQ(destructions, 1);
    ASSERT_EQ(copys, 1);

    value_ptr<Sideeffect> y = x;

    ASSERT_EQ(constructions,  1);
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

