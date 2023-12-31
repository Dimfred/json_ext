#pragma once

#define EXPECT_EX(function, expected_exception_msg)                                                                    \
    {                                                                                                                  \
        bool exception_thrown = false;                                                                                 \
        const char *what;                                                                                              \
        try                                                                                                            \
        {                                                                                                              \
            function;                                                                                                  \
        }                                                                                                              \
        catch (const std::exception &e)                                                                                \
        {                                                                                                              \
            exception_thrown = true;                                                                                   \
            what = e.what();                                                                                           \
        }                                                                                                              \
        ASSERT_TRUE(exception_thrown);                                                                                 \
        EXPECT_STREQ(expected_exception_msg, what);                                                                    \
    }
