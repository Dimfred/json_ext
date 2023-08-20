#include <gtest/gtest.h>

#include <nlohmann/json.hpp>
#include <nlohmann/json_ext.hpp>

#include "./utils.hpp"

struct A
{
    // clang-format off
    NLOHMANN_SERIALIZE_STRICT(A,
        (int, a)
    )
    // clang-format on
};

struct B
{
    // clang-format off
    NLOHMANN_SERIALIZE_STRICT(B,
        (int, a)
        (int, b)
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

struct WithOptionalA
{
    // clang-format off
    NLOHMANN_SERIALIZE_STRICT(WithOptionalA,
        (int, a, 1)
        (int, b)
    )
    // clang-format on
};

struct WithOptionalB
{
    // clang-format off
    NLOHMANN_SERIALIZE_STRICT(WithOptionalB,
        (int, a, 1)
        (int, c, 2)
    )
    // clang-format on
};

using WithOptionalAB = std::variant<WithOptionalA, WithOptionalB>;
using WithOptionalBA = std::variant<WithOptionalB, WithOptionalA>;

TEST(TestVariant, OkayWithOptionals)
{
    auto j1 = JSON({"a" : 1, "b" : 2, "c" : 3});
    EXPECT_EX(j1.get<WithOptionalAB>(),
              "[json.exception.other_error.601] unable to find matching variant for: {\"a\":1,\"b\":2,\"c\":3}");
    EXPECT_EX(j1.get<WithOptionalBA>(),
              "[json.exception.other_error.601] unable to find matching variant for: {\"a\":1,\"b\":2,\"c\":3}");

    auto j2 = JSON({"a" : 1, "b" : 2});
    auto v2_ab = j2.get<WithOptionalAB>();
    auto v2_ba = j2.get<WithOptionalBA>();
    EXPECT_TRUE(std::holds_alternative<WithOptionalA>(v2_ab));
    EXPECT_TRUE(std::holds_alternative<WithOptionalA>(v2_ba));

    auto j3 = JSON({"a" : 1, "c" : 2});
    auto v3_ab = j3.get<WithOptionalAB>();
    auto v3_ba = j3.get<WithOptionalBA>();
    EXPECT_TRUE(std::holds_alternative<WithOptionalB>(v3_ab));
    EXPECT_TRUE(std::holds_alternative<WithOptionalB>(v3_ba));
}

struct StrictTypeIntersectionA
{
    // clang-format off
    NLOHMANN_SERIALIZE_STRICT(StrictTypeIntersectionA,
        (int, a)
    )
    // clang-format on
};

struct StrictTypeIntersectionB
{
    // clang-format off
    NLOHMANN_SERIALIZE_STRICT(StrictTypeIntersectionB,
        (int, a)
        (int, b, 2)
    )
    // clang-format on
};

using StrictTypeIntersectionAB = std::variant<StrictTypeIntersectionA, StrictTypeIntersectionB>;
using StrictTypeIntersectionBA = std::variant<StrictTypeIntersectionB, StrictTypeIntersectionA>;

TEST(TestVariant, OkayStrictTypeIntersection)
{
    auto j1 = JSON({"a" : 1, "b" : 2, "c" : 3});
    EXPECT_EX(j1.get<StrictTypeIntersectionAB>(),
              "[json.exception.other_error.601] unable to find matching variant for: {\"a\":1,\"b\":2,\"c\":3}");
    EXPECT_EX(j1.get<StrictTypeIntersectionBA>(),
              "[json.exception.other_error.601] unable to find matching variant for: {\"a\":1,\"b\":2,\"c\":3}");

    auto j2 = JSON({"a" : 1});
    auto v2_ab = j2.get<StrictTypeIntersectionAB>();
    auto v2_ba = j2.get<StrictTypeIntersectionBA>();
    // edge case this effects the type depending on the variant order
    EXPECT_TRUE(std::holds_alternative<StrictTypeIntersectionA>(v2_ab));
    EXPECT_TRUE(std::holds_alternative<StrictTypeIntersectionB>(v2_ba));

    auto j3 = JSON({"a" : 1, "b" : 2});
    auto v3_ab = j3.get<StrictTypeIntersectionAB>();
    auto v3_ba = j3.get<StrictTypeIntersectionBA>();
    EXPECT_TRUE(std::holds_alternative<StrictTypeIntersectionB>(v3_ab));
    EXPECT_TRUE(std::holds_alternative<StrictTypeIntersectionB>(v3_ba));
}
