#include <gtest/gtest.h>
#include <valuable/value-ptr.hpp>

using namespace valuable;

TEST(value_ptr, lifetime) {

  static int constructions = 0;
  static int destructions = 0;
  static int copys;

  struct Sideeffect {
    Sideeffect() {
      ++constructions;
    }

    Sideeffect(Sideeffect const&) {
      ++copys;
    }


    Sideeffect(Sideeffect&&) {}

    ~Sideeffect() {
      ++destructions;
    }
  };

  {
    value_ptr<Sideeffect> x = Sideeffect();
    ASSERT_TRUE(constructions == 1);
    ASSERT_TRUE(destructions == 1);
    ASSERT_TRUE(copys == 1);

    value_ptr<Sideeffect> y = x;

    ASSERT_TRUE(constructions == 3);
    ASSERT_TRUE(copys == 1);

    value_ptr<Sideeffect> z;
    ASSERT_TRUE(constructions == 2);
    ASSERT_TRUE(copys == 1);
  }

  ASSERT_TRUE(destructions == 2);

}

