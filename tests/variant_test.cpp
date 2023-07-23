#include <gtest/gtest.h>

#include <nlohmann/json.hpp>
#include <nlohmann/json_ext.hpp>

struct A
{
    // clang-format off
    NLOHMANN_SERIALIZE_STRICT(A,
        ((int, a))
    )
    // clang-format on
};

struct B
{
    // clang-format off
    NLOHMANN_SERIALIZE_STRICT(B,
        ((int, a))
        ((int, b))
    )
    // clang-format on
};

using AB = std::variant<A, B>;
using BA = std::variant<B, A>;

TEST(TestVariant, Okay)
{
    auto j = JSON({"a" : 1});

    auto ab = j.get<AB>();
    EXPECT_TRUE(std::holds_alternative<A>(ab));

    auto ba = j.get<BA>();
    EXPECT_TRUE(std::holds_alternative<A>(ba));

    j = JSON({"a" : 1, "b" : 2});

    ab = j.get<AB>();
    EXPECT_TRUE(std::holds_alternative<B>(ab));

    ba = j.get<BA>();
    EXPECT_TRUE(std::holds_alternative<B>(ba));
}
