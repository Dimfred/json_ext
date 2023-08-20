#include <gtest/gtest.h>

#include <nlohmann/json.hpp>
#include <nlohmann/json_ext.hpp>

#include "utils.hpp"

using nlohmann::json;

struct Optional
{
    // clang-format off
    NLOHMANN_SERIALIZE(Optional,
        (std::optional<int>, opt)
    );
    // clang-format on
};

TEST(TestOptional, Okay)
{
    auto j = JSON({"opt" : null});
    auto obj = Optional();
    EXPECT_EQ(static_cast<json>(obj), j);
    EXPECT_EQ(static_cast<json>(obj).dump(), j.dump());

    j = JSON({"opt" : 42});
    obj = j.get<Optional>();
    EXPECT_EQ(static_cast<json>(obj), j);
    EXPECT_EQ(static_cast<json>(obj).dump(), j.dump());
}
