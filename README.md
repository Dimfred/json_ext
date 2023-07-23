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
        ((int, required))       /* no default value here */
        ((int, optional, 42))   /* with default value */
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

Doesn't work 100% you can build an edge case, when mixing default values and required values (good enough for me with this I can parse an `std::variant` more safely).

```cpp
#include <nlohmann/json_ext.hpp>

struct serialize_me_daddy
{
    NLOHMANN_SERIALIZE_STRICT(serialize_me_daddy,
        ((int, required))
    )
};

struct edge_case
{
    NLOHMANN_SERIALIZE_STRICT(edge_case,
        ((int, a, 1))
    )
};

int main()
{
    // this is okay only required is present on the json
    auto j1 = JSON({"required": 1337});
    auto obj1 = j1.get<serialize_me_daddy>();

    // this is not okay! more variables are present in the json than on the cls itself
    auto j2 = JSON({"required": 1337, "optional": 43});
    // auto obj2 = j2.get<serialize_me_daddy>();

    // edge case will still work, was able to circumvent :(
    auto j3 = JSON({"c": 3})
    auto obj3 = j3.get<edge_case>();
}
```

## Run the tests

### Ubuntu

```bash
sudo apt install -y libboost-dev
# this library also expects that you have `nlohmann/json.hpp` in your include directories
```

### Arch Linux

```bash
# on Arch
paru -Sy nlohmann-json boost
```

### Compile and Run tests

```bash
make test
```

## TODOs

- [ ] fix the edge case for variants
- [ ] improve the counting of the supplied members, currently done at runtime, but all this information is available at compile time
- [ ] improve the serialization interface, it is possible to supply just a one dimensional array as in: `(int, a)(int, b)` instead of `((int, a))((int b))`
