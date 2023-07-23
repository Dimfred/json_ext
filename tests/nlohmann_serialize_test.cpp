#include <gtest/gtest.h>

#include <nlohmann/json.hpp>
#include <nlohmann/json_ext.hpp>

#include "utils.hpp"

using nlohmann::json;

////////////////////////////////////////////////////////////////////////////////
/// DEFAULT VALUES ONLY
struct OnlyDefaults
{
    // clang-format off
    NLOHMANN_SERIALIZE(OnlyDefaults,
        ((int, a, 1))
        ((int, b, 2))
    )
    // clang-format on
};

struct OnlyDefaultsStrict
{
    // clang-format off
    NLOHMANN_SERIALIZE_STRICT(OnlyDefaultsStrict,
        ((int, a, 1))
        ((int, b, 2))
    )
    // clang-format on
};

TEST(TestNlohmannSerialize, OkayDefaults)
{
    auto j = JSON({"a" : 1, "b" : 2});

    OnlyDefaults obj1;
    EXPECT_EQ(1, obj1.a);
    EXPECT_EQ(2, obj1.b);
    EXPECT_EQ(j, static_cast<json>(obj1));

    OnlyDefaultsStrict obj2;
    EXPECT_EQ(1, obj2.a);
    EXPECT_EQ(2, obj2.b);
    EXPECT_EQ(j, static_cast<json>(obj2));
}

TEST(TestNlohmannSerialize, OkayDefaultsFullOverride)
{
    auto j = JSON({"a" : 3, "b" : 4});

    auto obj1 = j.get<OnlyDefaults>();
    EXPECT_EQ(3, obj1.a);
    EXPECT_EQ(4, obj1.b);
    EXPECT_EQ(j, static_cast<json>(obj1));

    auto obj2 = j.get<OnlyDefaultsStrict>();
    EXPECT_EQ(3, obj2.a);
    EXPECT_EQ(4, obj2.b);
    EXPECT_EQ(j, static_cast<json>(obj2));
}

TEST(TestNlohmannSerialize, OkayDefaultsPartialOverride)
{
    auto j = JSON({"b" : 4});

    auto obj1 = j.get<OnlyDefaults>();
    EXPECT_EQ(1, obj1.a);
    EXPECT_EQ(4, obj1.b);

    auto obj2 = j.get<OnlyDefaultsStrict>();
    EXPECT_EQ(1, obj1.a);
    EXPECT_EQ(4, obj1.b);
}

////////////////////////////////////////////////////////////////////////////////
/// NON DEFAULTS ONLY
struct OnlyNonDefaults
{
    // clang-format off
    NLOHMANN_SERIALIZE(OnlyNonDefaults,
        ((int, a))
        ((int, b))
    )
    // clang-format on
};

struct OnlyNonDefaultsStrict
{
    // clang-format off
    NLOHMANN_SERIALIZE_STRICT(OnlyNonDefaultsStrict,
        ((int, a))
        ((int, b))
    )
    // clang-format on
};

TEST(TestNlohmannSerialize, OkayNonDefaultsFullOverride)
{
    auto j = JSON({"a" : 3, "b" : 4});

    auto obj1 = j.get<OnlyNonDefaults>();
    EXPECT_EQ(3, obj1.a);
    EXPECT_EQ(4, obj1.b);
    EXPECT_EQ(j, static_cast<json>(obj1));

    auto obj2 = j.get<OnlyNonDefaultsStrict>();
    EXPECT_EQ(3, obj2.a);
    EXPECT_EQ(4, obj2.b);
    EXPECT_EQ(j, static_cast<json>(obj2));
}

TEST(TestNlohmannSerialize, OkayNonDefaultsFullOverrideMore)
{
    auto j = JSON({"a" : 3, "b" : 4, "c" : 5});

    // okay with non-strict
    auto obj1 = j.get<OnlyNonDefaults>();
    EXPECT_EQ(3, obj1.a);
    EXPECT_EQ(4, obj1.b);

    // fails with strict
    EXPECT_EX(
        "[json.exception.other_error.600] type must have 2 args, but has 3 args, error in: {\"a\":3,\"b\":4,\"c\":5}",
        j.get<OnlyNonDefaultsStrict>());
}

TEST(TestNlohmannSerialize, FailNonDefaultsValueMissing)
{
    auto j = JSON({"a" : 3});
    EXPECT_EX("[json.exception.out_of_range.403] key 'b' not found", j.get<OnlyNonDefaults>());
    EXPECT_EX("[json.exception.out_of_range.403] key 'b' not found", j.get<OnlyNonDefaultsStrict>());

    j = JSON({"b" : 4});
    EXPECT_EX("[json.exception.out_of_range.403] key 'a' not found", j.get<OnlyNonDefaults>());
    EXPECT_EX("[json.exception.out_of_range.403] key 'a' not found", j.get<OnlyNonDefaultsStrict>());
}

////////////////////////////////////////////////////////////////////////////////
/// MIXED DEFAULTS AND NON DEFAULTS
struct MixedDefaultNonDefault
{
    // clang-format off
    NLOHMANN_SERIALIZE(MixedDefaultNonDefault,
        ((int, a, 1))
        ((int, b))
    )
    // clang-format on
};

struct MixedDefaultNonDefaultStrict
{
    // clang-format off
    NLOHMANN_SERIALIZE_STRICT(MixedDefaultNonDefaultStrict,
        ((int, a, 1))
        ((int, b))
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
    auto j = JSON({"c" : 3});

    EXPECT_EX("[json.exception.out_of_range.403] key 'b' not found", j.get<MixedDefaultNonDefault>());
    EXPECT_EX("[json.exception.out_of_range.403] key 'b' not found", j.get<MixedDefaultNonDefaultStrict>());
}

////////////////////////////////////////////////////////////////////////////////
/// EDGE CASE WHICH CIRCUMVENTS STRICT :(
struct StrictEdgeCase
{
    NLOHMANN_SERIALIZE_STRICT(StrictEdgeCase, ((int, a, 42)))
};

TEST(TestNlohmannSerialize, FailStrictEdgeCase)
{
    auto j1 = JSON({"c" : 3});
    auto obj1 = j1.get<StrictEdgeCase>();
    EXPECT_EQ(42, obj1.a);

    // at least that works, there are more in the json than define on the class
    auto j2 = JSON({"a" : 1, "c" : 42});
    EXPECT_EX("[json.exception.other_error.600] type must have 1 args, but has 2 args, error in: {\"a\":1,\"c\":42}",
              j2.get<StrictEdgeCase>());
}
