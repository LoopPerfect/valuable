# valuable 💎

A C++ library implementing `value_ptr` - a smart-pointer with value-semantics.

Value types are simple and intuitive. Unfortunately sometimes an implementation detail requires us to use pointers.
The standard library demonstrates how handy smart-pointers can be for encoding ownership and lifetime semantics, but none of the smart-pointers implement value-semantics.

This is why `valuable::value_ptr` was made. 

Read [value_ptr — The Missing C++ Smart-pointer](https://hackernoon.com/value-ptr-the-missing-c-smart-pointer-1f515664153e) on Hackernoon

## Example

```c++

#include <string>
#include <valuable/value_ptr.hpp>

using namespace std;
using namespace valuable;

struct Tree {
  string const name;
  value_ptr<Tree> left;
  value_ptr<Tree> right;

  Tree(
    string const& name,
    value_ptr<Tree> const& left = value_ptr<Tree>{},
    value_ptr<Tree> const& right = value_ptr<Tree>{})
    : name{name}
    , left{left}
    , right{right}
  {}
};


int main() {
  Tree root = Tree{
    "root",
      Tree{"l"}
      Tree{"r"}
  };

  root.left = root; // copy of root assigned to left
  root.right = root; // and we don't have cyclic references
}

```

# Installation

This library requires a C++ 14 compiler.

Install with [Buckaroo](https://www.buckaroo.pm):

```
buckaroo install loopperfect/valuable
```

The [Buck](https://buckbuild.com/) target is `:valuable`

Alternatively, you can copy & paste the headers into your include path:

```
cp valuable/include/*.hpp $InstallPath/include/valuable
```

# Contributing

We are accepting contributions! 

Before you submit a PR, please run the tests: 

```
buck test
```
