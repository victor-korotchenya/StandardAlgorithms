#pragma once
#include<ostream>
#include<set>

namespace Standard::Algorithms::Numbers
{
    // todo(p3): vector could be faster than set.
    template<class item_t, class sub_sequence_t1 = std::set<item_t>>
    struct subset final
    {
        using sub_sequence_t = sub_sequence_t1;
        using sub_sets_t = std::set<subset>;

        static constexpr char begin_symbol = '{';
        static constexpr char end_symbol = '}';

        constexpr explicit subset(const sub_sequence_t &sub_sequence = {}, const sub_sets_t &sub_sets = {})
            : Sub_sequence(sub_sequence)
            , Sub_sets(sub_sets)
        {
        }

        constexpr explicit subset(const sub_sets_t &sub_sets, const sub_sequence_t &sub_sequence = {})
            : Sub_sequence(sub_sequence)
            , Sub_sets(sub_sets)
        {
        }

        [[nodiscard]] inline constexpr auto is_empty() const noexcept -> bool
        {
            auto result = Sub_sequence.empty() && Sub_sets.empty();
            return result;
        }

        [[nodiscard]] inline constexpr auto operator== (const subset &other) const noexcept -> bool
        {
            auto result = Sub_sets == other.Sub_sets && Sub_sequence == other.Sub_sequence;
            return result;
        }

        [[nodiscard]] inline constexpr auto operator<(const subset &other) const noexcept -> bool
        {
            // todo(p2): 3-way comparison.
            if (Sub_sequence < other.Sub_sequence)
            {
                return true;
            }

            if (other.Sub_sequence < Sub_sequence)
            {
                return false;
            }

            auto result = Sub_sets < other.Sub_sets;
            return result;
        }

        friend auto operator<< (std::ostream &str, const subset &subs) -> std::ostream &
        {
            const auto is_empty_sub_sequence = subs.Sub_sequence.empty();
            const auto is_empty_sub_sets = subs.Sub_sets.empty();

            if (is_empty_sub_sequence && is_empty_sub_sets)
            {
                str << begin_symbol << end_symbol;
                return str;
            }

            if (!is_empty_sub_sets)
            {
                str << begin_symbol;
            }

            if (!is_empty_sub_sequence)
            {
                str << begin_symbol;
                print_collection(str, subs.Sub_sequence);
                str << end_symbol;

                if (!is_empty_sub_sets)
                {
                    print_separator(str);
                }
            }

            if (!is_empty_sub_sets)
            {
                print_collection(str, subs.Sub_sets);
                str << end_symbol;
            }

            return str;
        }

private:
        inline static void print_separator(std::ostream &str)
        {
            str << ", ";
        }

        template<class collection_t>
        static void print_collection(std::ostream &str, const collection_t &collection)
        {
            auto was_before = false;

            for (const auto &item : collection)
            {
                if (was_before)
                {
                    print_separator(str);
                }
                else
                {
                    was_before = true;
                }

                str << item;
            }
        }

        sub_sequence_t Sub_sequence;
        sub_sets_t Sub_sets;
    };
} // namespace Standard::Algorithms::Numbers
