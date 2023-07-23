# json_ext.hpp

An extension library for [nlohmann/json](https://github.com/nlohmann/json).

It uses `boost/preprocessor` to generate the required members and functions.

## Rational

In my work I require often to serialize, deserialize classes, nlohmann has the `NLOHMANN_DEFINE_TYPE*` macros for that to quickly reflect elements of a class.

The problem here is that I often forget to add a variable to the reflected set, which then results in errors I have to track down (minor issue just don't forget to do it).

The bigger problem is that the classes either support all values as default or no values as default. This is what I also tried to tackle with my macro, you should be able to supply some variables with defaults, and some should be required during deserialization.

Another feature I am missing is the serialization of `std::variant` and `std::optional`, so I have implemented those based on [this](https://github.com/nlohmann/json#how-do-i-convert-third-party-types), there was also another issue for `std::variant`, but I can't find it (basically just want to say that someone else did the hardcore work).

## Features

- Definition of the variables to serialize is done in one macro, if you don't set a default value, `from_json` will throw an error.
- There is a second macro `NLOHMANN_SERIALIZE_STRICT`, which if on the json other variables are present will also error out. See examples for more information.

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

Doesn't work 100% you can build edge cases, when mixing default values and required values, but good enough for me with this I can parse an `std::variant` more safely.

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

```bash
# on Arch
paru -Sy nlohmann-json boost
make test
```

