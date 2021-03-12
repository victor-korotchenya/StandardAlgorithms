#include "add_tests.h"
#include "IsWithinTests.h"

//#include "SubSetsNumberTests.h"
//#include "SumToMinOneTests.h"
//#include "WeightedHousesOn2DTests.h"

#include "adjacent_divide_permutation_tests.h"
#include "all_pairs_gcd_sum_tests.h"
#include "ArithmeticTests.h"
#include "array_is_cycle_tests.h"
#include "array_utilities_tests.h"
#include "ArrayConcatenateMinimumTests.h"
#include "ArrayInversionCountTests.h"
#include "ArrayItemsCountingOverKTimesTests.h"
#include "ArrayMaximumXorOf2ElementsTests.h"
#include "ArrayMinMoves2ZeroSumTests.h"
#include "ArrayMinMovesToMakeNonDecreasingTests.h"
#include "ArrayProductDividingCurrentIndexTests.h"
#include "ArraySumOfThreeElementsTests.h"
#include "bit_utilities_tests.h"
#include "Catalan_moduloTests.h"
#include "chinese_remainder_garner_tests.h"
#include "coin_change_count_tests.h"
#include "CoinLineMaxSumTests.h"
#include "count_add_sub_put_parentheses_distinct_tests.h"
#include "count_arrays_different_adjacent_same_1n_tests.h"
#include "count_catalan_braces_tests.h"
#include "count_linear_equation_solutions_tests.h"
#include "count_partitions_tests.h"
#include "count_subset_with_product_tests.h"
#include "count_unique_20_choices_of_100_pets_tests.h"
#include "count_xor_subsets_tests.h"
#include "CrossPointOTwofIncrDecrLinesTests.h"
#include "del_min_side_items_2min_gt_max_tests.h"
#include "DiceCountsTests.h"
#include "digit_and_sum_of_ab_tests.h"
#include "digit_product_count_tests.h"
#include "digit_sum_no0_divisible_tests.h"
#include "digit_sum_tests.h"
#include "DigitSum_RobotInGridTests.h"
#include "disjoint_set_tests.h"
#include "DistinctItemsInRangeTests.h"
#include "DropFloorTests.h"
#include "elevator_min_moves.h"
#include "enum_catalan_braces_tests.h"
#include "enumerate_partitions_tests.h"
#include "eratosthenes_sieve_tests.h"
#include "FastFourierTransformTests.h"
#include "FibonacciTests.h"
#include "fib3_tests.h"
#include "Find2RepeatingDistinctItemsTests.h"
#include "FindSorted2DMatrixTests.h"
#include "FindStringPatternIn2DArrayTests.h"
#include "gaussian_elimination_tests.h"
#include "GerryManderingTests.h"
#include "GreatestRectangleInHistogramTest.h"
#include "HashedBitArrayTests.h"
#include "inclusion_exclusion_tests.h"
#include "IsPushPopStackSequenceTests.h"
#include "IteratorUtilitiesTests.h"
#include "KnapsackDynamoTests.h"
#include "land2d_cut_vert_horiz_line_tests.h"
#include "longest_alternative_subsequence_tests.h"
#include "longest_arithmetic_progression_in_sorted_tests.h"
#include "longest_palindromic_substring_manacher_tests.h"
#include "longest_subsequence_palindrome_tests.h"
#include "LongestIncreasingSubsequenceTests.h"
#include "Matrix_UtilitiesTests.h"
#include "max_contiguous_sum_removing_most_m_tests.h"
#include "max_gcd_pair_in_array_tests.h"
#include "max_in_sliding_window_tests.h"
#include "maximum_share_speculation_tests.h"
#include "MaximumDifference2ItemsTests.h"
#include "MaximumIndexDifference2ItemsTests.h"
#include "MaxSubMatrixIn2DMatrixTests.h"
#include "MaxSumIncreasingSequenceTests.h"
#include "MaxSumNonDecreasingSubsequenceTests.h"
#include "MedianSelection.h"
#include "min_copy_paste_to_sort_permutation_tests.h"
#include "min_merges_get_all_binary_1_tests.h"
#include "min_stick_cuts_tests.h"
#include "min_sum_array_to1_replacing_consecutive_by_sum_tests.h"
#include "min_swaps_between_permutations_tests.h"
#include "min_time_array_abs_dif_sum_tests.h"
#include "min_time_copy_books_tests.h"
#include "Min3Tests.h"
#include "MinimumMovesToSortTests.h"
#include "mobius_binary_sequence_period_tests.h"
#include "most_recent_used_cache_tests.h"
#include "next_greater_element_tests.h"
#include "number_mod_tests.h"
#include "NumberComparerTests.h"
#include "NumberSplitterTests.h"
#include "NumberUtilitiesTests.h"
#include "OccurrencesInSortedArrayTests.h"
#include "one_time_queue_tests.h"
#include "power_sum_tests.h"
#include "prime_number_utility_tests.h"
#include "PrintFrequenciesInEditableArrayTests.h"
#include "product_lines_knapsack_tests.h"
#include "quadratic_residue_tests.h"
#include "quick_select_kth_smallest_tests.h"
#include "report_differences_tests.h"
#include "smawk_find_row_mins_in_tot_monotone_matrix_tests.h"
#include "SortedArraySumOfTwoElementsTests.h"
#include "SortTest.h"
#include "SpanTests.h"
#include "StackAndMinSimpleTests.h"
#include "StackAndMinTests.h"
#include "stirling_numbers_modulo_tests.h"
#include "string_cut_into_min_palindromes_tests.h"
#include "string_min_appends_to_palindrome_tests.h"
#include "string_pattern_adjacent_swap_count_tests.h"
#include "SubsetSumTests.h"
#include "SubSetTests.h"
#include "SumOfMin2ConsecutiveNumbersTests.h"
#include "swap_knights_min_moves_tests.h"
#include "two_subsets_min_diff_knapsack_tests.h"
#include "TwoRepetitionsInterleavingTests.h"
#include "vector_clock_tests.h"
#include "WeightedHousesOnRoadTests.h"
#include "WeightedIntervalSchedulingTests.h"

using namespace std;
using namespace Privet::Algorithms::Numbers::Tests;
using namespace Privet::Algorithms;

void Privet::Algorithms::Numbers::Tests::add_tests(vector<pair<string, test_function>>& tests, const string& longTimeRunningPrefix)
{
    tests.emplace_back("IsWithinTests", &IsWithinTests);

    //tests.emplace_back("SumToMinOneTests", &SumToMinOneTests::Test);

   //tests.emplace_back("WeightedHousesOn2DTests", &WeightedHousesOn2DTests::Test);

   //tests.emplace_back("TestNextArrayPermutation", &TestNextArrayPermutation);

   //tests.emplace_back("SubSetsNumberTests", &SubSetsNumberTests::Test);

    tests.emplace_back("maximum_share_speculation_tests", &maximum_share_speculation_tests);

    tests.emplace_back("count_xor_subsets_tests", &count_xor_subsets_tests);

    tests.emplace_back("most_recent_used_cache_tests", &most_recent_used_cache_tests);

    tests.emplace_back("longest_alternative_subsequence_tests", &longest_alternative_subsequence_tests);

    tests.emplace_back("quadratic_residue_tests", &quadratic_residue_tests);

    tests.emplace_back("prime_number_utility_tests", &prime_number_utility_tests);

    tests.emplace_back("count_linear_equation_solutions_tests", &count_linear_equation_solutions_tests);

    tests.emplace_back("gaussian_elimination_tests", &gaussian_elimination_tests);

    tests.emplace_back("min_time_copy_books_tests", &min_time_copy_books_tests);

    tests.emplace_back("land2d_cut_vert_horiz_line_tests", &land2d_cut_vert_horiz_line_tests);

    tests.emplace_back(longTimeRunningPrefix + longTimeRunningPrefix + "min_copy_paste_to_sort_permutation_tests", &min_copy_paste_to_sort_permutation_tests);

    tests.emplace_back("count_add_sub_put_parentheses_distinct_tests", &count_add_sub_put_parentheses_distinct_tests);

    tests.emplace_back("min_swaps_between_permutations_tests", &min_swaps_between_permutations_tests);

    tests.emplace_back("min_stick_cuts_tests", &min_stick_cuts_tests);

    tests.emplace_back(longTimeRunningPrefix + "smawk_find_row_mins_in_tot_monotone_matrix_tests", &smawk_find_row_mins_in_tot_monotone_matrix_tests);

    tests.emplace_back("product_lines_knapsack_tests", &product_lines_knapsack_tests);

    tests.emplace_back("del_min_side_items_2min_gt_max_tests", &del_min_side_items_2min_gt_max_tests);

    tests.emplace_back("longest_arithmetic_progression_in_sorted_tests", &longest_arithmetic_progression_in_sorted_tests);

    tests.emplace_back(longTimeRunningPrefix + "swap_knights_min_moves_tests", &swap_knights_min_moves_tests);

    tests.emplace_back("count_partitions_tests", &count_partitions_tests);

    tests.emplace_back("array_is_cycle_tests", &array_is_cycle_tests);

    tests.emplace_back("string_min_appends_to_palindrome_tests", &string_min_appends_to_palindrome_tests);

    tests.emplace_back("longest_palindromic_substring_manacher_tests", &longest_palindromic_substring_manacher_tests);

    tests.emplace_back("eratosthenes_sieve_tests", &eratosthenes_sieve_tests);

    tests.emplace_back("prime_number_count_tests", &prime_number_count_tests);

    tests.emplace_back("string_cut_into_min_palindromes_tests", &string_cut_into_min_palindromes_tests);

    tests.emplace_back("longest_subsequence_palindrome_tests", &longest_subsequence_palindrome_tests);

    tests.emplace_back(longTimeRunningPrefix + "coin_change_count_tests", &coin_change_count_tests);

    tests.emplace_back("min_time_array_abs_dif_sum_tests", &min_time_array_abs_dif_sum_tests);

    tests.emplace_back("min_sum_array_to1_replacing_consecutive_by_sum_tests", &min_sum_array_to1_replacing_consecutive_by_sum_tests);

    tests.emplace_back(longTimeRunningPrefix + "count_unique_20_choices_of_100_pets_tests", &count_unique_20_choices_of_100_pets_tests);

    tests.emplace_back("min_merges_get_all_binary_1_tests", &min_merges_get_all_binary_1_tests);

    tests.emplace_back(longTimeRunningPrefix + longTimeRunningPrefix + "digit_and_sum_of_ab_tests", &digit_and_sum_of_ab_tests);

    tests.emplace_back(longTimeRunningPrefix + "digit_sum_no0_divisible_tests", &digit_sum_no0_divisible_tests);

    tests.emplace_back(longTimeRunningPrefix + "digit_product_count_tests", &digit_product_count_tests);

    tests.emplace_back("digit_sum_tests", &digit_sum_tests);

    tests.emplace_back("stirling_numbers_modulo_tests", &stirling_numbers_modulo_tests);

    tests.emplace_back("count_catalan_braces_tests", &count_catalan_braces_tests);

    tests.emplace_back("enum_catalan_braces_tests", &enum_catalan_braces_tests);

    tests.emplace_back(longTimeRunningPrefix + "adjacent_divide_permutation_tests", &adjacent_divide_permutation_tests);

    tests.emplace_back("array_utilities_tests", &array_utilities_tests);

    tests.emplace_back("bit_utilities_tests", &bit_utilities_tests);

    tests.emplace_back("enumerate_partitions_tests", &enumerate_partitions_tests);

    tests.emplace_back("report_differences_tests", &report_differences_tests);

    tests.emplace_back("mobius_binary_sequence_period_tests", &mobius_binary_sequence_period_tests);

    tests.emplace_back("chinese_remainder_garner_tests", &chinese_remainder_garner_tests);

    tests.emplace_back(longTimeRunningPrefix + "vector_clock_tests", &vector_clock_tests);

    tests.emplace_back("power_sum_tests", &power_sum_tests);

    tests.emplace_back("inclusion_exclusion_tests", &inclusion_exclusion_tests);

    tests.emplace_back("quick_select_kth_smallest_tests", &quick_select_kth_smallest_tests);

    tests.emplace_back("all_pairs_gcd_sum_tests", &all_pairs_gcd_sum_tests);

    tests.emplace_back("next_greater_element_tests", &next_greater_element_tests);

    tests.emplace_back(longTimeRunningPrefix + "max_contiguous_sum_removing_most_m_tests", &max_contiguous_sum_removing_most_m_tests);

    tests.emplace_back("count_subset_with_product_tests", &count_subset_with_product_tests);

    tests.emplace_back(longTimeRunningPrefix + "count_arrays_different_adjacent_same_1n_tests", &count_arrays_different_adjacent_same_1n_tests);

    tests.emplace_back("string_pattern_adjacent_swap_count_tests", &string_pattern_adjacent_swap_count_tests);

    tests.emplace_back("two_subsets_min_diff_knapsack_tests", &two_subsets_min_diff_knapsack_tests);

    tests.emplace_back("max_gcd_pair_in_array_tests", &max_gcd_pair_in_array_tests);

    tests.emplace_back("elevator_min_moves_test", &elevator_min_moves_test);

    tests.emplace_back("number_mod_tests", &number_mod_tests);

    tests.emplace_back("ArrayMinMovesToMakeNonDecreasingTests", &ArrayMinMovesToMakeNonDecreasingTests);

    tests.emplace_back("one_time_queue_tests", &one_time_queue_tests);

    tests.emplace_back("Matrix_UtilitiesTests", &Matrix_UtilitiesTests);

    tests.emplace_back("ArrayMinMoves2ZeroSumTests", &ArrayMinMoves2ZeroSumTests);

    tests.emplace_back(longTimeRunningPrefix + "ArrayMaximumXorOf2ElementsTests", &ArrayMaximumXorOf2ElementsTests);

    tests.emplace_back("CrossPointOTwofIncrDecrLinesTests", &CrossPointOTwofIncrDecrLinesTests);

    tests.emplace_back("DistinctItemsInRangeTests", &DistinctItemsInRangeTests);

    tests.emplace_back("MaxSumIncreasingSequenceTests", &MaxSumIncreasingSequenceTests);

    tests.emplace_back(longTimeRunningPrefix + "Catalan_moduloTests", &Catalan_moduloTests);

    tests.emplace_back(longTimeRunningPrefix + longTimeRunningPrefix + "DropFloorTests", &DropFloorTests);

    tests.emplace_back("MaxSumNonDecreasingSubsequenceTests", &MaxSumNonDecreasingSubsequenceTests::Test);

    tests.emplace_back("SubSetTests", &SubSetTests);

    tests.emplace_back(longTimeRunningPrefix + "CoinLineMaxSumTests", &CoinLineMaxSumTests);

    tests.emplace_back("ArrayProductDividingCurrentIndexTests", &ArrayProductDividingCurrentIndexTests);

    tests.emplace_back(longTimeRunningPrefix +
        "GerryManderingTests", &GerryManderingTests);

    tests.emplace_back(longTimeRunningPrefix + "WeightedHousesOnRoadTests", &WeightedHousesOnRoadTests::Test);

    tests.emplace_back(longTimeRunningPrefix + "DigitSum_RobotInGridTests", &DigitSum_RobotInGridTests);

    tests.emplace_back(longTimeRunningPrefix + "FibonacciTests", &FibonacciTests);

    tests.emplace_back("fib3_tests", &fib3_tests);

    tests.emplace_back(longTimeRunningPrefix + "MaxSubMatrixIn2DMatrixTests", &MaxSubMatrixIn2DMatrixTests::Test);

    tests.emplace_back(longTimeRunningPrefix + "NumberSplitterTests", &NumberSplitterTests::Test);

    tests.emplace_back("DiceCountsTests", &DiceCountsTests);

    tests.emplace_back("MinimumMovesToSortTests", &MinimumMovesToSortTests);

    tests.emplace_back("NumberUtilitiesTests", &NumberUtilitiesTests);

    tests.emplace_back("SumOfMin2ConsecutiveNumbersTests", &SumOfMin2ConsecutiveNumbersTests::Test);

    tests.emplace_back("ArraySumOfThreeElementsTests", &ArraySumOfThreeElementsTests::Test);

    tests.emplace_back("SortedArraySumOfTwoElementsTests", &SortedArraySumOfTwoElementsTests::Test);

    tests.emplace_back("OccurrencesInSortedArrayTests", &OccurrencesInSortedArrayTests::Test);

    tests.emplace_back("ArrayInversionCountTests", &ArrayInversionCountTests::Test);

    tests.emplace_back("ArrayItemsCountingOverKTimesTests", &ArrayItemsCountingOverKTimesTests);

    tests.emplace_back("ArrayConcatenateMinimumTests", &ArrayConcatenateMinimumTests);

    tests.emplace_back("IsPushPopStackSequenceTests", &IsPushPopStackSequenceTests);

    tests.emplace_back("StackAndMinTests", &StackAndMinTests::Test);

    tests.emplace_back("StackAndMinSimpleTests", &StackAndMinSimpleTests::Test);

    tests.emplace_back("IteratorUtilitiesTests", &IteratorUtilitiesTests::Test);

    tests.emplace_back("FindSorted2DMatrixTests", &FindSorted2DMatrixTests);

    tests.emplace_back("Min3Tests", &Min3Tests);

    tests.emplace_back("FindStringPatternIn2DArrayTests", &FindStringPatternIn2DArrayTests::Test);

    tests.emplace_back("PrintFrequenciesInEditableArrayTests", &PrintFrequenciesInEditableArrayTests::Test);

    tests.emplace_back("MaximumIndexDifference2ItemsTests", &MaximumIndexDifference2ItemsTests);

    tests.emplace_back("MaximumDifference2ItemsTests", &MaximumDifference2ItemsTests::Test);

    tests.emplace_back(longTimeRunningPrefix + "ArithmeticTests", &ArithmeticTests::Test);

    tests.emplace_back("Find2RepeatingDistinctItemsTests", &Find2RepeatingDistinctItemsTests::Test);

    tests.emplace_back("max_in_sliding_window_tests", &max_in_sliding_window_tests);

    tests.emplace_back("GreatestRectangleInHistogramTest", &GreatestRectangleInHistogramTest::Test);

    tests.emplace_back("SpanTests", &SpanTests::Test);

    tests.emplace_back("NumberComparerTests", &NumberComparerTests::Test);

    tests.emplace_back("FastFourierTransformTests", &FastFourierTransformTests::Test);

    tests.emplace_back(//TODO: p1. get function name from the pointer?
        "sort_tests", &sort_tests);

    tests.emplace_back("disjoint_set_tests", &disjoint_set_tests);

    tests.emplace_back("LongestIncreasingSubsequenceTests", &LongestIncreasingSubsequenceTests);

    tests.emplace_back("MedianSelectionTests", &MedianSelectionTests);

    tests.emplace_back("HashedBitArrayTests", &HashedBitArrayTests::Test);

    tests.emplace_back("TwoRepetitionsInterleavingTests", &TwoRepetitionsInterleavingTests::Test);

    tests.emplace_back("WeightedIntervalSchedulingTests", &WeightedIntervalSchedulingTests::Test);

    tests.emplace_back("SubsetSumTests", &SubsetSumTests);

    tests.emplace_back("KnapsackDynamoTests", &knapsack_dynamo_tests);
}