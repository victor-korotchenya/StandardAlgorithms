#include"add_tests.h"
#include"aho_corasick_tests.h"
#include"burrows_wheeler_transform_tests.h"
#include"count_distinct_substrings_of_length_tests.h"
#include"double_dispatch_tests.h"
#include"edit_distance_vintsiuk_tests.h"
#include"enumerate_all_valid_brace_strings_tests.h"
#include"full_class_tests.h"
#include"knuth_morris_pratt_tests.h"
#include"lcp_tests.h"
#include"longest_common_factor_tests.h"
#include"longest_common_subsequence_tests.h"
#include"longest_palindromic_factor_manacher_tests.h"
#include"longest_repeated_substring_tests.h"
#include"longest_subsequence_palindrome_tests.h"
#include"regular_expression_tests.h"
#include"suffix_array_search_tests.h"
#include"suffix_array_tests.h"
#include"test_name_utilities_tests.h"
#include"text_justification_tests.h"
#include"text_recognition_tests.h"
#include"three_strings_interleaving_tests.h"
#include"ukkonen_suffix_tree_tests.h"
#include"word_recognizer_tests.h"
#include"z_alg_tests.h"

namespace
{
    constexpr auto five = 5;
} // namespace

void Standard::Algorithms::Strings::Tests::add_tests(
    std::vector<test_function> &tests, const std::string &long_time_running_prefix)
{
    tests.emplace_back("double_dispatch_tests", &double_dispatch_tests, 1);

    tests.emplace_back("burrows_wheeler_transform_tests", &burrows_wheeler_transform_tests, 3);

    tests.emplace_back("ukkonen_suffix_tree_tests", &ukkonen_suffix_tree_tests, 3);

    tests.emplace_back("longest_common_factor_tests", &longest_common_factor_tests, five);

    tests.emplace_back("longest_subsequence_palindrome_tests", &longest_subsequence_palindrome_tests, 3);

    tests.emplace_back("longest_palindromic_factor_manacher_tests", &longest_palindromic_factor_manacher_tests, 2);

    tests.emplace_back("longest_repeated_substring_tests", &longest_repeated_substring_tests, 3);

    tests.emplace_back("lcp_tests", &lcp_tests, 1);

    tests.emplace_back("suffix_array_search_tests", &suffix_array_search_tests, 2);
    tests.emplace_back("suffix_array_tests", &suffix_array_tests, 4);

    tests.emplace_back("string_border_tests", &string_border_tests, 2);
    tests.emplace_back("z_array_tests", &z_array_tests, 2);

    tests.emplace_back("count_distinct_substrings_of_length_tests", &count_distinct_substrings_of_length_tests, 2);

    tests.emplace_back("text_justification_tests", &text_justification_tests, 2);
    tests.emplace_back("text_recognition_tests", &text_recognition_tests, 1);

    tests.emplace_back("three_strings_interleaving_tests", &three_strings_interleaving_tests, 1);

    tests.emplace_back("enumerate_all_valid_brace_strings_tests", &enumerate_all_valid_brace_strings_tests, 1);

    tests.emplace_back("knuth_morris_pratt_str_tests", &knuth_morris_pratt_str_tests, 1);
    tests.emplace_back(long_time_running_prefix + "knuth_morris_pratt_tests", &knuth_morris_pratt_tests, five * 3 - 4);

    tests.emplace_back(
        long_time_running_prefix + long_time_running_prefix + "aho_corasick_tests", &aho_corasick_tests, 1);

    tests.emplace_back("regular_expression_tests", &regular_expression_tests, 3);

    tests.emplace_back("full_class_tests", &full_class_tests, 1);

    tests.emplace_back("test_name_utilities_tests", &test_name_utilities_tests, 1);

    tests.emplace_back("edit_distance_vintsiuk_tests", &edit_distance_vintsiuk_tests, 1);

    tests.emplace_back(
        long_time_running_prefix + "longest_common_subsequence_tests", &longest_common_subsequence_tests, 4);

    tests.emplace_back("word_recognizer_tests", &word_recognizer_tests, 1);
}
