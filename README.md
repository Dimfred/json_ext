# json_ext.hpp

Header-only extension library for [nlohmann/json](https://github.com/nlohmann/json).

- Adds `NLOHMANN_SERIALIZE` and `NLOHMANN_SERIALIZE_STRICT` macros, which defines and reflects member variables in one macro, it also allows for default / non-default values in one macro (basically a mix of `NLOHMANN_DEFINE_TYPE_INTRUSIVE` and `NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULTS`
- Adds serialization / deserialization for `std::optional` and `std::variant`
- uses `boost/preprocessor` to generate the reflected members and functions.

## Rational

In my work I require often to serialize, deserialize classes, `nlohmann::json` has the `NLOHMANN_DEFINE_TYPE*` macros for that to quickly reflect elements of a class.

The problem here is that I often forget to add a variable to the reflected set, which then results in errors I have to track down (minor issue just don't forget to do it).

The bigger problem is that the classes either support all values as defaults or no values as defaults. You should be able to supply some variables with defaults, and some should be required during deserialization. I incorporated this in my macro.

Another feature I am missing is the serialization of `std::variant` and `std::optional`, so I have implemented those based on [this](https://github.com/nlohmann/json#how-do-i-convert-third-party-types), there was also another issue for `std::variant`, but I can't find it (basically just want to say that someone else did the hardcore work).

Also I wanted to play with `boost/preprocessor` :)

## Examples

Check the tests for more usage examples.

### Basic

```cpp
#include <nlohmann/json_ext.hpp>

struct serialize_me_daddy
{
    NLOHMANN_SERIALIZE(serialize_me_daddy,
        (int, required)       /* no default value here */
        (int, optional, 42)   /* with default value */
    )
};

int main()
{
    // json macro also allows you to directly create a json
    auto j1 = JSON({"required": 1337});
    auto obj1 = j1.get<serialize_me_daddy>();

    auto j2 = JSON({"optional": 43});
    // this will fail, because 'required' is not given
    // auto obj2 = j2.get<serialize_me_daddy>();
}
```

### Using NLOHMANN_SERIALIZE_STRICT

Will error out if you have more variables present in the json than on the class itself.

`NLOHMANN_SERIALIZE_STRICT`, will store the reflected keys.
In `from_json` it will check whether every key in the json is also present in the reflected set of keys.
This is particularly useful when you serialize a `std::variant`.

```cpp
struct TypeA
{
    NLOHMANN_SERIALIZE_STRICT(TypeA,
        (int, a)
    );
};

struct TypeB
{
    NLOHMANN_SERIALIZE_STRICT(TypeB,
        (int, a)
        (int, b)
    );
};

using TypeAB = std::variant<TypeA, TypeB>;
using TypeBA = std::variant<TypeB, TypeA>;

auto j1 = JSON({"a": 1})
j1.get<TypeAB>() // will be TypeA
j1.get<TypeBA>() // will be TypeA

auto j2 = JSON({"a": 1, "b": 2})
j2.get<TypeAB>() // will be TypeB
j2.get<TypeBA>() // will be TypeB
```

There is still an edge case left, which can probably not be fixed, at least not with the deserialization I use.

```cpp
struct StrictTypeIntersectionA
{
    NLOHMANN_SERIALIZE_STRICT(StrictTypeIntersectionA,
        (int, a)
    )
};

struct StrictTypeIntersectionB
{
    NLOHMANN_SERIALIZE_STRICT(StrictTypeIntersectionB,
        (int, a)
        (int, b, 2)
    )
};

using StrictTypeIntersectionAB = std::variant<StrictTypeIntersectionA, StrictTypeIntersectionB>;
using StrictTypeIntersectionBA = std::variant<StrictTypeIntersectionB, StrictTypeIntersectionA>;

auto j = JSON({"a":1});
j.get<StrictTypeIntersectionAB>(); // will result in StrictTypeIntersectionA
j.get<StrictTypeIntersectionBA>(); // will result in StrictTypeIntersectionB
```

## Run the tests

### Ubuntu

```bash
sudo apt install -y libboost-dev cmake gcc
# this library also expects that you have `nlohmann/json.hpp` in your include directories
```

### Arch Linux

```bash
# on Arch
paru -Sy nlohmann-json boost cmake gcc
```

### Compile and Run tests

```bash
make test
```

## TODOs

- [ ] maybe fix variant with optional value case, but probably impossible to do that
- [X] fix the edge case for variants
- [X]  no counting done anymore ~~improve the counting of the supplied members, currently done at runtime, but all this information is available at compile time~~
- [X] improve the serialization interface, it is possible to supply just a one dimensional array as in: `(int, a)(int, b)` instead of `((int, a))((int b))`
