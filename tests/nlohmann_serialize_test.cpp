#include <iostream>

#include <gtest/gtest.h>

#include <nlohmann/json.hpp>
#include <nlohmann/json_ext.hpp>

#include "./utils.hpp"

using nlohmann::json;

////////////////////////////////////////////////////////////////////////////////
/// DEFAULT VALUES ONLY
struct OnlyDefaults
{
    // clang-format off
    NLOHMANN_SERIALIZE(OnlyDefaults,
        (int, a, 1)
        (int, b, 2)
    )
    // clang-format on
};

struct OnlyDefaultsStrict
{
    // clang-format off
    NLOHMANN_SERIALIZE_STRICT(OnlyDefaultsStrict,
        (int, a, 1)
        (int, b, 2)
    )
    // clang-format on
};

TEST(TestNlohmannSerialize, OkayDefaults)
{
    auto j = JSON({"a" : 1, "b" : 2});

    OnlyDefaults obj1;
    EXPECT_EQ(obj1.a, 1);
    EXPECT_EQ(obj1.b, 2);
    EXPECT_EQ(static_cast<json>(obj1), j);

    OnlyDefaultsStrict obj2;
    EXPECT_EQ(obj2.a, 1);
    EXPECT_EQ(obj2.b, 2);
    EXPECT_EQ(static_cast<json>(obj2), j);
}

TEST(TestNlohmannSerialize, OkayDefaultsFullOverride)
{
    auto j = JSON({"a" : 3, "b" : 4});

    auto obj1 = j.get<OnlyDefaults>();
    EXPECT_EQ(obj1.a, 3);
    EXPECT_EQ(obj1.b, 4);
    EXPECT_EQ(j, static_cast<json>(obj1));

    auto obj2 = j.get<OnlyDefaultsStrict>();
    EXPECT_EQ(obj2.a, 3);
    EXPECT_EQ(obj2.b, 4);
    EXPECT_EQ(static_cast<json>(obj2), j);
}

TEST(TestNlohmannSerialize, OkayDefaultsPartialOverride)
{
    auto j = JSON({"b" : 4});

    auto obj1 = j.get<OnlyDefaults>();
    EXPECT_EQ(obj1.a, 1);
    EXPECT_EQ(obj1.b, 4);

    auto obj2 = j.get<OnlyDefaultsStrict>();
    EXPECT_EQ(obj2.a, 1);
    EXPECT_EQ(obj2.b, 4);
}

////////////////////////////////////////////////////////////////////////////////
/// NON DEFAULTS ONLY
struct OnlyNonDefaults
{
    // clang-format off
    NLOHMANN_SERIALIZE(OnlyNonDefaults,
        (int, a)
        (int, b)
    )
    // clang-format on
};

struct OnlyNonDefaultsStrict
{
    // clang-format off
    NLOHMANN_SERIALIZE_STRICT(OnlyNonDefaultsStrict,
        (int, a)
        (int, b)
    )
    // clang-format on
};

TEST(TestNlohmannSerialize, OkayNonDefaultsFullOverride)
{
    auto j = JSON({"a" : 3, "b" : 4});

    auto obj1 = j.get<OnlyNonDefaults>();
    EXPECT_EQ(obj1.a, 3);
    EXPECT_EQ(obj1.b, 4);
    EXPECT_EQ(static_cast<json>(obj1), j);

    auto obj2 = j.get<OnlyNonDefaultsStrict>();
    EXPECT_EQ(obj2.a, 3);
    EXPECT_EQ(obj2.b, 4);
    EXPECT_EQ(static_cast<json>(obj2), j);
}

TEST(TestNlohmannSerialize, OkayNonDefaultsFullOverrideMore)
{
    auto j = JSON({"a" : 3, "b" : 4, "c" : 5});

    // okay with non-strict
    auto obj1 = j.get<OnlyNonDefaults>();
    EXPECT_EQ(obj1.a, 3);
    EXPECT_EQ(obj1.b, 4);

    // fails with strict
    EXPECT_EX(j.get<OnlyNonDefaultsStrict>(),
              "[json.exception.other_error.600] key 'c' not present in reflected keys: {\"a\":3,\"b\":4,\"c\":5}")
}

TEST(TestNlohmannSerialize, FailNonDefaultsValueMissing)
{
    auto j = JSON({"a" : 3});
    EXPECT_EX(j.get<OnlyNonDefaults>(), "[json.exception.out_of_range.403] key 'b' not found")
    EXPECT_EX(j.get<OnlyNonDefaultsStrict>(), "[json.exception.out_of_range.403] key 'b' not found")

    j = JSON({"b" : 4});
    EXPECT_EX(j.get<OnlyNonDefaults>(), "[json.exception.out_of_range.403] key 'a' not found")
    EXPECT_EX(j.get<OnlyNonDefaultsStrict>(), "[json.exception.out_of_range.403] key 'a' not found")
}

////////////////////////////////////////////////////////////////////////////////
/// MIXED DEFAULTS AND NON DEFAULTS
struct MixedDefaultNonDefault
{
    // clang-format off
    NLOHMANN_SERIALIZE(MixedDefaultNonDefault,
        (int, a, 1)
        (int, b)
    )
    // clang-format on
};

struct MixedDefaultNonDefaultStrict
{
    // clang-format off
    NLOHMANN_SERIALIZE_STRICT(MixedDefaultNonDefaultStrict,
        (int, a, 1)
        (int, b)
    )
    // clang-format on
};

TEST(TestNlohmannSerialize, OkayMixedDefaultNonDefault)
{
    auto j = JSON({"b" : 2});

    auto obj1 = j.get<MixedDefaultNonDefault>();
    EXPECT_EQ(1, obj1.a);
    EXPECT_EQ(2, obj1.b);

    auto obj2 = j.get<MixedDefaultNonDefaultStrict>();
    EXPECT_EQ(1, obj2.a);
    EXPECT_EQ(2, obj2.b);
}

TEST(TestNlohmannSerialize, FailMixedDefaultNonDefault)
{
    auto j1 = JSON({"c" : 3});
    EXPECT_EX(j1.get<MixedDefaultNonDefault>(), "[json.exception.out_of_range.403] key 'b' not found")

    // with strict this should fail before the key is checked
    EXPECT_EX(j1.get<MixedDefaultNonDefaultStrict>(),
              "[json.exception.other_error.600] key 'c' not present in reflected keys: {\"c\":3}")

    // if it is empty it is fine with strict, still b is not present
    auto j2 = JSON({});
    EXPECT_EX(j2.get<MixedDefaultNonDefaultStrict>(), "[json.exception.out_of_range.403] key 'b' not found")
}

////////////////////////////////////////////////////////////////////////////////
/// OLD EDGE CASE
/// should now be fixed
struct StrictEdgeCase
{
    NLOHMANN_SERIALIZE_STRICT(StrictEdgeCase, (int, a, 42))
};

TEST(TestNlohmannSerialize, FailStrictEdgeCase)
{
    auto j1 = JSON({"c" : 3});
    EXPECT_EX(j1.get<StrictEdgeCase>(),
              "[json.exception.other_error.600] key 'c' not present in reflected keys: {\"c\":3}")

    // at least that works, there are more in the json than define on the class
    auto j2 = JSON({"a" : 1, "c" : 42});
    EXPECT_EX(j2.get<StrictEdgeCase>(),
              "[json.exception.other_error.600] key 'c' not present in reflected keys: {\"a\":1,\"c\":42}")
}
