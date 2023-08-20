#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <boost/preprocessor/facilities/is_empty_variadic.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/json.hpp>

#define JSON(...) nlohmann::json::parse(#__VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
/// SERIALIZATION std::variant
template <typename T, typename... Ts>
void variant_from_json(const nlohmann::json &j, std::variant<Ts...> &data, bool &has_parsed)
{
    if (has_parsed)
        return;

    try
    {
        data = j.get<T>();
        has_parsed = true;
    }
    catch (...)
    {
        // if we couldn't parse the variant we don't care, from outside we will just try
        // the next one
    }
}

template <typename... Ts> struct nlohmann::adl_serializer<std::variant<Ts...>>
{
    static void to_json(nlohmann::json &j, const std::variant<Ts...> &data)
    {
        std::visit([&j](const auto &&unpacked) { j = unpacked; }, data);
    }

    static void from_json(const nlohmann::json &j, std::variant<Ts...> &data)
    {
        bool has_parsed = false;
        (variant_from_json<Ts>(j, data, has_parsed), ...);
        if (!has_parsed)
            throw nlohmann::detail::other_error::create(
                601, nlohmann::detail::concat("unable to find matching variant for: ", j.dump()), &j);
    }
};

////////////////////////////////////////////////////////////////////////////////
/// SERIALIZATION std::optional
template <typename T> struct nlohmann::adl_serializer<std::optional<T>>
{
    static void to_json(nlohmann::json &j, const std::optional<T> &data)
    {
        if (data)
            j = *data;
    }

    static void from_json(const nlohmann::json &j, std::optional<T> &data)
    {
        if (!j.is_null())
            data = j.get<T>();
    }
};

////////////////////////////////////////////////////////////////////////////////
/// PLACEHOLDER TO ADAPT ARRAY OF TUPLE
/// (https://stackoverflow.com/questions/24309309/how-to-use-boost-preprocessor-to-generate-accessors)
/// With this we can define the serialization with:
/// Serialization(Type, (type1, name1, value1)(type2, name2, value2))
/// insteadof: Serialization(Type, ((type1, name1, value1))((type2, name2, value2))
#define CREATE_PLACEHOLDER_FILLER_0(...) ((__VA_ARGS__)) CREATE_PLACEHOLDER_FILLER_1
#define CREATE_PLACEHOLDER_FILLER_0_END
#define CREATE_PLACEHOLDER_FILLER_1(...) ((__VA_ARGS__)) CREATE_PLACEHOLDER_FILLER_0
#define CREATE_PLACEHOLDER_FILLER_1_END

////////////////////////////////////////////////////////////////////////////////
/// VARIABLE DEFNITION

// @summary input (variable_type, variable_name, optional(variable_default_value) will extract tuple[0] aka the
// variable_type
#define GET_VARIABLE_TYPE(var_type_and_name_and_maybe_value)                                                           \
    BOOST_PP_TUPLE_ELEM(3, 0, var_type_and_name_and_maybe_value)

// @summary input (variable_type, variable_name, optional(variable_default_value) will extract tuple[1] aka the
// variable_name
#define GET_VARIABLE_NAME(var_type_and_name_and_maybe_value)                                                           \
    BOOST_PP_TUPLE_ELEM(3, 1, var_type_and_name_and_maybe_value)

// if we have a default value, this will help us to add the '=' sign between var_name and default value
// if we don't have a default value, then no '=' is added
#define DEFINE_VARIABLE_ DEFINE_VARIABLE_
#define DEFINE_VARIABLE_1
#define DEFINE_VARIABLE_0 =

#define _GET_VARIABLE_VALUE(...) BOOST_PP_CAT(DEFINE_VARIABLE_, BOOST_PP_IS_EMPTY(__VA_ARGS__)) __VA_ARGS__

// @summary input (variable_type, variable_name, optional(variable_default_value) will extract tuple[3] aka the
// variable_default_value
// if the we have defined a default value, then this will expand to: '= default_value'
// else it will be empty
#define GET_VARIABLE_VALUE(var_type_and_name_and_maybe_value)                                                          \
    _GET_VARIABLE_VALUE(BOOST_PP_TUPLE_ELEM(3, 2, var_type_and_name_and_maybe_value))

#define DEFINE_VARIABLE(R, data, var_type_and_name_and_maybe_value)                                                    \
    GET_VARIABLE_TYPE(var_type_and_name_and_maybe_value)                                                               \
    GET_VARIABLE_NAME(var_type_and_name_and_maybe_value) GET_VARIABLE_VALUE(var_type_and_name_and_maybe_value);

#define DEFINE_VARIABLES(var_types_and_names_and_maybe_values)                                                         \
    BOOST_PP_SEQ_FOR_EACH(DEFINE_VARIABLE, _,                                                                          \
                          BOOST_PP_CAT(CREATE_PLACEHOLDER_FILLER_0 var_types_and_names_and_maybe_values, _END))

////////////////////////////////////////////////////////////////////////////////
/// NLOHMANN SERIALIZATION DEFINITION FROM

// defines without default
#define DEFINE_JSON_FROM_WITHOUT_DEFAULT_1(var_name) NLOHMANN_JSON_FROM(var_name);

// defined with default
#define DEFINE_JSON_FROM_WITHOUT_DEFAULT_0(var_name) NLOHMANN_JSON_FROM_WITH_DEFAULT(var_name)
#define DEFINE_JSON_FROM_WITHOUT_DEFAULT_ DEFINE_JSON_FROM_WITHOUT_DEFAULT_

#define __DEFINE_FROM_JSON(...) BOOST_PP_CAT(DEFINE_JSON_FROM_WITHOUT_DEFAULT_, BOOST_PP_IS_EMPTY(__VA_ARGS__))

#define _DEFINE_FROM_JSON(R, data, var_type_and_name_and_maybe_value)                                                  \
    __DEFINE_FROM_JSON(BOOST_PP_TUPLE_ELEM(3, 2, var_type_and_name_and_maybe_value))                                   \
    (GET_VARIABLE_NAME(var_type_and_name_and_maybe_value))

#define DEFINE_FROM_JSON(Type, var_types_and_names_and_maybe_values)                                                   \
    friend void from_json(const nlohmann::json &nlohmann_json_j, Type &nlohmann_json_t)                                \
    {                                                                                                                  \
        const Type nlohmann_json_default_obj{};                                                                        \
        BOOST_PP_SEQ_FOR_EACH(_DEFINE_FROM_JSON, _,                                                                    \
                              BOOST_PP_CAT(CREATE_PLACEHOLDER_FILLER_0 var_types_and_names_and_maybe_values, _END))    \
    }

#define __DEFINE_POSSIBLE_KEYS(var_type_and_name_and_maybe_value)                                                      \
    BOOST_PP_STRINGIZE(GET_VARIABLE_NAME(var_type_and_name_and_maybe_value)),

#define _DEFINE_POSSIBLE_KEYS(R, data, var_type_and_name_and_maybe_value)                                              \
    __DEFINE_POSSIBLE_KEYS(var_type_and_name_and_maybe_value)

#define DEFINE_FROM_JSON_STRICT(Type, var_types_and_names_and_maybe_values)                                            \
    friend void from_json(const nlohmann::json &nlohmann_json_j, Type &nlohmann_json_t)                                \
    {                                                                                                                  \
        /* strict serialization stores the possible keys in the function, to check if additional keys are present */   \
        static const std::vector<std::string> possible_keys = {BOOST_PP_SEQ_FOR_EACH(                                  \
            _DEFINE_POSSIBLE_KEYS, _,                                                                                  \
            BOOST_PP_CAT(CREATE_PLACEHOLDER_FILLER_0 var_types_and_names_and_maybe_values, _END))};                    \
                                                                                                                       \
        /* check that every key in our json, exists in the reflected keys of our serialized class */                   \
        for (const auto &item : nlohmann_json_j.items())                                                               \
        {                                                                                                              \
            bool is_allowed_key =                                                                                      \
                std::find(possible_keys.begin(), possible_keys.end(), item.key()) != possible_keys.end();              \
            /* if an additional aka. non-allowed key is found kill the serialization */                                \
            if (!is_allowed_key)                                                                                       \
            {                                                                                                          \
                throw nlohmann::detail::other_error::create(                                                           \
                    600,                                                                                               \
                    nlohmann::detail::concat("key '", item.key(),                                                      \
                                             "' not present in reflected keys: ", nlohmann_json_j.dump()),             \
                    &nlohmann_json_j);                                                                                 \
            }                                                                                                          \
        }                                                                                                              \
                                                                                                                       \
        const Type nlohmann_json_default_obj{};                                                                        \
        /* define the parsing as in the non-strict version */                                                          \
        BOOST_PP_SEQ_FOR_EACH(_DEFINE_FROM_JSON, _,                                                                    \
                              BOOST_PP_CAT(CREATE_PLACEHOLDER_FILLER_0 var_types_and_names_and_maybe_values, _END))    \
    }

////////////////////////////////////////////////////////////////////////////////
/// NLOHMANN SERIALIZATION DEFINITION TO
#define __DEFINE_TO_JSON(var_name) NLOHMANN_JSON_TO(var_name)

#define _DEFINE_TO_JSON(R, data, var_type_and_name_and_maybe_value)                                                    \
    __DEFINE_TO_JSON(GET_VARIABLE_NAME(var_type_and_name_and_maybe_value))

#define DEFINE_TO_JSON(Type, var_types_and_names_and_maybe_values)                                                     \
    friend void to_json(nlohmann::json &nlohmann_json_j, const Type &nlohmann_json_t)                                  \
    {                                                                                                                  \
        BOOST_PP_SEQ_FOR_EACH(_DEFINE_TO_JSON, _,                                                                      \
                              BOOST_PP_CAT(CREATE_PLACEHOLDER_FILLER_0 var_types_and_names_and_maybe_values, _END))    \
    }

////////////////////////////////////////////////////////////////////////////////
/// NEW SERIALIZATION INTERFACE
#define NLOHMANN_SERIALIZE(Type, var_types_and_names_and_maybe_values)                                                 \
    DEFINE_VARIABLES(var_types_and_names_and_maybe_values)                                                             \
    DEFINE_TO_JSON(Type, var_types_and_names_and_maybe_values)                                                         \
    DEFINE_FROM_JSON(Type, var_types_and_names_and_maybe_values)

#define NLOHMANN_SERIALIZE_STRICT(Type, var_types_and_names_and_maybe_values)                                          \
    DEFINE_VARIABLES(var_types_and_names_and_maybe_values)                                                             \
    DEFINE_TO_JSON(Type, var_types_and_names_and_maybe_values)                                                         \
    DEFINE_FROM_JSON_STRICT(Type, var_types_and_names_and_maybe_values)
