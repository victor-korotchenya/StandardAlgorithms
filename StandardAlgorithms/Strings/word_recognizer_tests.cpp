#include"word_recognizer_tests.h"
#include"../Utilities/test_utilities.h"
#include"word_recognizer.h"

namespace
{
    using int_t = std::uint32_t;
    using weight_t = std::int32_t;
    using set_t = std::unordered_set<std::string>;
    using offset_length_t = std::pair<int_t, int_t>;

    using word_recognizer_t = Standard::Algorithms::Strings::word_recognizer<int_t, weight_t, set_t, offset_length_t>;

    struct test_case final : Standard::Algorithms::Tests::base_test_case
    {
        // todo(p3): constexpr
        test_case(std::string &&name, std::string &&text, set_t &&words, weight_t expected_cost,
            std::vector<offset_length_t> &&expected_positions) noexcept
            : base_test_case(std::move(name))
            , Text(std::move(text))
            , Words(std::move(words))
            , Expected_cost(expected_cost)
            , Expected_positions(std::move(expected_positions))
        {
        }

        [[nodiscard]] constexpr auto text() const &noexcept -> const std::string &
        {
            return Text;
        }

        [[nodiscard]] constexpr auto words() const &noexcept -> const set_t &
        {
            return Words;
        }

        [[nodiscard]] constexpr auto expected_cost() const &noexcept -> const weight_t &
        {
            return Expected_cost;
        }

        [[nodiscard]] constexpr auto expected_positions() const &noexcept -> const std::vector<offset_length_t> &
        {
            return Expected_positions;
        }

        void print(std::ostream &str) const override
        {// todo(p3): Remove "append_separator".
            ::Standard::Algorithms::append_separator(str);
            ::Standard::Algorithms::print_value("Text", str, Text);

            ::Standard::Algorithms::append_separator(str);
            ::Standard::Algorithms::print("Words", Words.cbegin(), Words.cend(), str);

            str << " Expected cost " << Expected_cost;

            ::Standard::Algorithms::append_separator(str);
            ::Standard::Algorithms::print("Expected positions", Expected_positions, str);
        }

        // todo(p3): constexpr
        void validate() const override
        {
            base_test_case::validate();
            Standard::Algorithms::throw_if_empty("Text", Text);
            Standard::Algorithms::throw_if_empty("Words", Words);
            validate_expected_positions();
        }

private:
        // todo(p3): constexpr
        void validate_expected_positions() const
        {
            Standard::Algorithms::throw_if_empty("Expected positions", Expected_positions);
            check_position(0);

            const auto length = Expected_positions.size();

            for (std::size_t index = 1; index < length; ++index)
            {
                const auto &previous = Expected_positions[index - 1ZU];
                const auto &current = Expected_positions[index];

                check_position(index);

                if (previous.first + previous.second != current.first)
                {
                    auto str = ::Standard::Algorithms::Utilities::w_stream();
                    str << "Bad Offset at index " << index;

                    Standard::Algorithms::throw_exception(str);
                }
            }

            check_last_position();
        }

        // todo(p3): constexpr
        void check_position(const std::size_t index) const
        {
            const auto index_as_string = std::to_string(index);
            ::Standard::Algorithms::ert::greater(Expected_positions.size(), index, "Too large index " + index_as_string);

            const auto text_size = Text.size();
            const auto offset_name = "Offset at index " + index_as_string;
            const auto &position = Expected_positions[index];
            if (0U == index)
            {
                ::Standard::Algorithms::ert::are_equal(int_t{}, position.first, offset_name);
            }
            else
            {
                ::Standard::Algorithms::ert::greater(position.first, int_t{}, offset_name);
                ::Standard::Algorithms::ert::greater_or_equal(text_size, position.first, offset_name);
            }

            const auto length_name = "Length at index " + index_as_string;
            ::Standard::Algorithms::ert::greater(position.second, int_t{}, length_name);
            ::Standard::Algorithms::ert::greater_or_equal(text_size, position.second, length_name);

            const auto sum = position.first + position.second;
            ::Standard::Algorithms::ert::greater_or_equal(
                text_size, sum, "(Offset + Length) at index " + index_as_string);
        }

        constexpr void check_last_position() const
        {
            const auto &last = Expected_positions.at(Expected_positions.size() - 1ZU);
            const auto sum = last.first + last.second;

            ::Standard::Algorithms::ert::are_equal(Text.size(), sum, "Last position (Offset + Length)");
        }

        std::string Text;
        set_t Words;
        weight_t Expected_cost;
        std::vector<offset_length_t> Expected_positions;
    };

    // todo(p3): constexpr
    void generate_test_cases(std::vector<test_case> &test_cases)
    {
        constexpr auto len3 = 3;
        constexpr auto len5 = 5;
        constexpr auto len8 = 8;
        constexpr auto len9 = 9;
        constexpr auto len10 = 10;
        constexpr auto len14 = 14;
        constexpr auto len15 = 15;
        {
            constexpr auto expected_cost =
                word_recognizer_t::evaluate_weight(len3) + word_recognizer_t::evaluate_weight(len10);

            test_cases.emplace_back("Simple1",
                //"the blackboard", lengths are 3 and 10.
                std::string("the") + "black" + "board", set_t({ "black", "blackboard", "board", "lack", "the" }),
                expected_cost, std::vector<offset_length_t>({ { 0, len3 }, { len3, len10 } }));
        }
        {
            // they outh event == 16 + 16 + 25 == 57 == not best.
            // the youth event == 59
            constexpr auto expected_cost =
                word_recognizer_t::evaluate_weight(len3) + (word_recognizer_t::evaluate_weight(len5) * 2);

            test_cases.emplace_back("Several combinations", std::string("the") + "youth" + "event",
                set_t({ "the", "they", "youth", "outh", "event", "vent" }), expected_cost,
                std::vector<offset_length_t>({ { 0, len3 }, { len3, len5 }, { len8, len5 } }));
        }
        {
            constexpr auto expected_cost = word_recognizer_t::evaluate_weight(1);

            test_cases.emplace_back(
                "One letter", "a", set_t({ "a" }), expected_cost, std::vector<offset_length_t>({ { 0, 1 } }));
        }
        {
            constexpr auto expected_cost = word_recognizer_t::evaluate_weight(1) * 2;

            test_cases.emplace_back("Two letters - two words", "ab", set_t({ "a", "b" }), expected_cost,
                std::vector<offset_length_t>({ { 0, 1 }, { 1, 1 } }));
        }
        {
            constexpr auto expected_cost = word_recognizer_t::evaluate_weight(2);

            test_cases.emplace_back("Two letters - one word", "ab", set_t({ "a", "b", "ab" }), expected_cost,
                std::vector<offset_length_t>({ { 0, 2 } }));
        }
        {
            constexpr auto expected_cost = word_recognizer_t::evaluate_weight(4) +
                (word_recognizer_t::evaluate_weight(2) * 2) - (word_recognizer_t::evaluate_weight(1) * 3);

            test_cases.emplace_back("Some not-existing words",
                // a h is this it ?
                "ahisthisit?", set_t({ "is", "it", "this" }), expected_cost,
                std::vector<offset_length_t>({ { 0, 1 }, { 1, 1 }, { 2, 2 }, { 4, 4 }, { len8, 2 }, { len10, 1 } }));
        }
        {
            constexpr auto expected_cost = -word_recognizer_t::evaluate_weight(1) * 4;

            test_cases.emplace_back("All not-existing words", "ABBA", set_t({ "Mamma", "Mia", "!" }), expected_cost,
                std::vector<offset_length_t>({ { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 } }));
        }
        {
            constexpr auto expected_cost = word_recognizer_t::evaluate_weight(len9) +
                2 * word_recognizer_t::evaluate_weight(4) - 2 * word_recognizer_t::evaluate_weight(1);

            test_cases.emplace_back("Long text", "extremely long text",
                set_t({ "text", "tex", "re", "long", "extremely", "extreme", "ext", "ex" }), expected_cost,
                std::vector<offset_length_t>({ { 0, len9 }, { len9, 1 }, { len10, 4 }, { len14, 1 }, { len15, 4 } }));
        }
    }

    constexpr void run_test_case(const test_case &test)
    {
        const auto cost_positions = word_recognizer_t::recognize(test.text(), test.words());

        ::Standard::Algorithms::ert::are_equal(test.expected_cost(), cost_positions.first, "Cost");
        ::Standard::Algorithms::ert::are_equal(test.expected_positions(), cost_positions.second, "Positions");
    }
} // namespace

void Standard::Algorithms::Strings::Tests::word_recognizer_tests()
{
    // todo(p3): static_assert
    (Standard::Algorithms::Tests::test_utilities<test_case>::run_tests(run_test_case, generate_test_cases));
}
