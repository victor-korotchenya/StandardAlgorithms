#pragma once
#include <climits>
#include <unordered_set>
#include "StreamUtilities.h"

//For pointer.
inline void ThrowNullException(const std::string& argumentName, const std::string& extraMessage = "")
{
#ifdef _DEBUG
    if (argumentName.empty())
    {
        throw std::runtime_error("The 'argumentName' must be not empty in ThrowNullException.");
    }
#endif

    std::ostringstream ss;
    ss << "The '" << argumentName << "' argument must be not null.";

    throw StreamUtilities::throw_exception(ss, extraMessage);
}

//For "string".
inline void ThrowNullExceptionString(const std::string& argumentName, const std::string& extraMessage = "")
{
#ifdef _DEBUG
    if (argumentName.empty())
        throw std::runtime_error("The 'argumentName' must be not empty in ThrowNullExceptionString.");
#endif

    std::ostringstream ss;
    ss << "The '" << argumentName << "' argument must be not empty string.";

    throw StreamUtilities::throw_exception(ss, extraMessage);
}

inline void ThrowIfNull(const std::string& value, const std::string& argumentName, const std::string& extraMessage = "")
{
#ifdef _DEBUG
    if (argumentName.empty())
    {
        throw std::runtime_error("The 'argumentName' must be not empty in ThrowNullException.");
    }
#endif

    if (value.empty())
    {
        ThrowNullExceptionString(argumentName, extraMessage);
    }
}

template <typename T, typename = std::enable_if_t<std::is_pointer<T>::value>>
T ThrowIfNull(T value, const std::string& argumentName, const std::string& extraMessage = "")
{
#ifdef _DEBUG
    if (argumentName.empty())
        throw std::runtime_error("The 'argumentName' must be not empty in ThrowNullException.");
#endif

    if (nullptr == value)
        ThrowNullException(argumentName, extraMessage);

    return value;
}

inline void ThrowEmptyException(const std::string& argumentName, const std::string& extraMessage = "")
{
#ifdef _DEBUG
    if (argumentName.empty())
        throw std::runtime_error("The 'argumentName' must be not empty in ThrowEmptyException.");
#endif

    std::ostringstream ss;
    ss << "The '" << argumentName << "' argument must be not empty.";

    throw StreamUtilities::throw_exception(ss, extraMessage);
}

//For collections.
template <typename T>
void ThrowIfEmpty(const T& value, const std::string& argumentName, const std::string& extraMessage = "")
{
#ifdef _DEBUG
    if (argumentName.empty())
        throw std::runtime_error("The 'argumentName' must be not empty in ThrowIfEmpty.");
#endif

    if (value.empty())
        ThrowEmptyException(argumentName, extraMessage);
}

template<typename Number>
Number RequirePositive(const Number& value, const std::string& message = "")
{
    if (value <= Number(0))
    {
        std::ostringstream ss;
        ss << "The value (" << value << ") must be positive.";
        StreamUtilities::throw_exception<std::out_of_range>(ss, message);
    }

    return value;
}

template<typename Number>
Number RequireNonNegative(const Number& value, const std::string& message = "")
{
    if (value < Number(0))
    {
        std::ostringstream ss;
        ss << "The value (" << value << ") must be non-negative.";
        StreamUtilities::throw_exception<std::out_of_range>(ss, message);
    }

    return value;
}

template<typename Number, typename Number2>
Number RequireNotGreater(const Number& value, const Number2& max_value, const std::string& message = "")
{
    if (value > max_value)
    {
        std::ostringstream ss;
        ss << "The value (" << value << ") must non exceed " << max_value << ".";
        StreamUtilities::throw_exception<std::out_of_range>(ss, message);
    }

    return value;
}

template<typename Number, typename Number2>
Number RequireGreater(const Number& value, const Number2& edge_value, const std::string& message = "")
{
    if (value <= edge_value)
    {
        std::ostringstream ss;
        ss << "The value (" << value << ") cannot be less than or equal to " << edge_value << ".";
        StreamUtilities::throw_exception<std::out_of_range>(ss, message);
    }

    return value;
}

template <class iter_t, class iter_t2, class number_t = typename iter_t::value_type>
void RequireAllPositive(iter_t begin, iter_t2 end, const std::string& message)
{
    assert(message.size());

    const auto found = std::find_if(begin, end,
        [](const number_t& num) -> bool {
            return num <= number_t{};
        });
    if (end == found)
        return;

    std::ostringstream ss;
    ss << "The value (" << (*found)
        << ") at index " << (found - begin)
        << " must be strictly positive.";
    StreamUtilities::throw_exception(ss, message);
}

template <class number_t>
void RequireAllPositive(const std::vector<number_t>& data, const std::string& message)
{
    RequireAllPositive(data.cbegin(), data.cend(), message);
}

template <class number_t>
void RequireAllNonNegative(const std::vector<number_t>& data, const std::string& message)
{
    static_assert(std::is_signed_v<number_t>);
    assert(message.size());

    const auto found = std::find_if(data.cbegin(), data.cend(),
        [](const number_t num) -> bool {
            return num < number_t(0);
        });

    if (data.cend() != found)
    {
        std::ostringstream ss;
        ss << "The value (" << (*found)
            << ") at index " << (found - data.cbegin())
            << " must be non-negative.";
        StreamUtilities::throw_exception(ss, message);
    }
}

template <class v_t, class item_t = typename v_t::value_type, class set_t = std::unordered_set<item_t>>
void require_unique(const v_t& data, const std::string& message)
{
    assert(message.size());
    if (data.empty())
        return;

    set_t s;
    for (const auto& d : data)
    {
        if (!s.insert(d).second)
        {
            std::ostringstream ss;
            ss << "The value (" << d
                << ") must not repeat.";
            StreamUtilities::throw_exception(ss, message);
        }
    }
}

template <class t>
std::runtime_error throw_type_mismatch()
{
    std::string s("Type mismatch for '");
    s += typeid(t).name();
    s += "'.";
    throw std::runtime_error(s.c_str());
}