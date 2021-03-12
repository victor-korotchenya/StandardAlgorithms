#pragma once
#include <string>
#include <type_traits>

template <typename, typename T>
class HasFunctionGetName final
{
    static_assert(std::integral_constant<T, false>::value, "Second template parameter must be function.");
};

//The "value" indicates
//whether the Class has function 'ReturnType get_Name(Arguments).
template <typename Class, typename ReturnType, typename... Arguments>
class HasFunctionGetName<Class, ReturnType(Arguments...)> final
{
    template <typename>
    static constexpr std::false_type Has(...);

    template <typename T>
    static constexpr auto Has(void*) -> typename
        std::is_same<ReturnType,
        decltype(std::declval<T>().get_Name(std::declval<Arguments>()...))>::type;

    using _Type = decltype(Has<Class>(nullptr));

public:
    static constexpr bool value = _Type::value;
};

//The "value" indicates
//whether the Class has field 'Type Name'.
template <typename Class, typename FieldType>
class HasFieldName final
{
    template <typename>
    static constexpr std::false_type Has(...);

    template <typename T>
    static constexpr auto Has(void*) -> typename
        std::is_same<FieldType,
        decltype(std::declval<T>().Name)>::type;

    using _Type = decltype(Has<Class>(nullptr));

public:
    static constexpr bool value = _Type::value;
};

//The "value" indicates
//whether the Class has field 'Type first'.
template <typename Class, typename FieldType = std::string>
class HasFieldFirst final
{
    template <typename >
    static constexpr std::false_type Has(...);

    template <typename T>
    static constexpr auto Has(void*) -> typename
        std::is_same<FieldType,
        decltype(std::declval<T>().first)>::type;

    using _Type = decltype(Has<Class>(nullptr));

public:
    static constexpr bool value = _Type::value;
};