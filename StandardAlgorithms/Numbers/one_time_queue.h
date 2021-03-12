#pragma once
#include"../Utilities/require_utilities.h"

namespace Standard::Algorithms::Numbers
{
    // No individual item death-tractors are required to call which simplifiers the class design.
    template<class item_type1, std::integral size_type1>
    requires(std::is_fundamental_v<item_type1> || std::is_pointer_v<item_type1>)
    struct one_time_queue final
    {
        using item_type = item_type1;
        using size_type = size_type1;

        constexpr explicit one_time_queue(const size_type capacity)
            : Capacity(require_positive(capacity, "capacity"))
            , Data(new item_type[capacity])
        {
            assert(Data != nullptr);

            if constexpr (::Standard::Algorithms::is_debug)
            {
                self_check();
            }
        }

        one_time_queue(const one_time_queue &) = delete;
        auto operator= (const one_time_queue &) & -> one_time_queue & = delete;
        one_time_queue(one_time_queue &&) noexcept = delete;
        auto operator= (one_time_queue &&) &noexcept -> one_time_queue & = delete;

        constexpr ~one_time_queue() noexcept
        {
            // No need to call ~item_type().
            delete[] Data;
        }

        [[nodiscard]] constexpr auto size() const noexcept(!::Standard::Algorithms::is_debug) -> size_type
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                self_check();
            }

            auto siz = static_cast<size_type>(Tail - Head);
            return siz;
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept(!::Standard::Algorithms::is_debug) -> bool
        {
            auto emp = size_type{} == size();
            return emp;
        }

        [[nodiscard]] constexpr auto data() const &noexcept(!::Standard::Algorithms::is_debug) -> const item_type *
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                self_check();
            }

            return Data;
        }

        [[nodiscard]] constexpr auto top() const & -> item_type
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                self_check();
            }

            if (is_empty()) [[unlikely]]
            {
                throw std::runtime_error("One time queue is empty at top.");
            }

            assert(Head < Capacity);

            const auto &result = Data[Head];
            return result;
        }

        constexpr void reset() &noexcept(!::Standard::Algorithms::is_debug)
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                // Better safe than sorry.
                self_check();
            }

            Tail = Head = size_type{};

            if constexpr (::Standard::Algorithms::is_debug)
            {
                self_check();
            }
        }

        constexpr void push(item_type value) &
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                self_check();
            }

            if (!(Tail < Capacity))
            {
                auto str = ::Standard::Algorithms::Utilities::w_stream();
                str << "One time queue is full, capacity " << Capacity << ".";

                throw_exception(str);
            }

            Data[Tail] = value;
            ++Tail;

            if constexpr (::Standard::Algorithms::is_debug)
            {
                self_check();
            }
        }

        [[maybe_unused]] constexpr auto pop() & -> item_type
        {
            if constexpr (::Standard::Algorithms::is_debug)
            {
                self_check();
            }

            if (is_empty()) [[unlikely]]
            {
                throw std::runtime_error("Cannot pop from an empty One time queue.");
            }

            assert(Head < Capacity);

            auto result = Data[Head];
            ++Head;

            if constexpr (::Standard::Algorithms::is_debug)
            {
                self_check();
            }

            return result;
        }

private:
        constexpr void self_check() const noexcept(!::Standard::Algorithms::is_debug)
        {
            if constexpr (!::Standard::Algorithms::is_debug)
            {
                return;
            }

            throw_if_null("Data", Data);
            require_positive(Capacity, "Capacity");
            require_less_equal(Tail, Capacity, "tail vs capacity");
            require_less_equal(Head, Tail, "head vs tail");
        }

        const size_type Capacity;
        size_type Head{};
        item_type *const Data;
        size_type Tail{};
    };
} // namespace Standard::Algorithms::Numbers
