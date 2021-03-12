#include "add_tests.h"
#include "aho_corasick_tests.h"
#include "count_distinct_substrings_of_length.h"
#include "double_dispatch_tests.h"
#include "EditDistanceTests.h"
#include "EnumerateAllValidBraceStringsTests.h"
#include "knuth_morris_pratt_tests.h"
#include "longest_common_substring_tests.h"
#include "longest_repeated_substring_tests.h"
#include "LongestCommonSubsequenceTests.h"
#include "regular_expression_tests.h"
#include "suffix_array.h"
#include "TestNameUtilitiesTests.h"
#include "text_justificationTests.h"
#include "ThreeStringsInterleavingTests.h"
#include "ukkonen_suffix_tree_tests.h"
#include "WordRecognizerTests.h"
#include "z_alg.h"

using namespace std;
using namespace Privet::Algorithms::Strings::Tests;
using namespace Privet::Algorithms;

void Privet::Algorithms::Strings::Tests::add_tests(vector<pair<string, test_function>>& tests, const string& longTimeRunningPrefix)
{
    tests.emplace_back("double_dispatch_tests", &double_dispatch_tests);

    tests.emplace_back("ukkonen_suffix_tree_tests", &ukkonen_suffix_tree_tests);

    tests.emplace_back("longest_common_substring_tests", &longest_common_substring_tests);

    tests.emplace_back("longest_repeated_substring_tests", &longest_repeated_substring_tests);

    tests.emplace_back("suffix_array_test", &suffix_array_test);
    tests.emplace_back("string_border_test", &string_border_test);
    tests.emplace_back("z_array_test", &z_array_test);
    tests.emplace_back("count_distinct_substrings_of_length_tests", &count_distinct_substrings_of_length_tests);

    tests.emplace_back("text_justificationTests", &text_justificationTests);

    tests.emplace_back("ThreeStringsInterleavingTests", &ThreeStringsInterleavingTests);

    tests.emplace_back("EnumerateAllValidBraceStringsTests", &EnumerateAllValidBraceStringsTests);

    tests.emplace_back("KnuthMorrisPrattStrTests", &KnuthMorrisPrattStrTests);
    tests.emplace_back(longTimeRunningPrefix + "knuth_morris_pratt_tests", &knuth_morris_pratt_tests);

    tests.emplace_back(longTimeRunningPrefix + longTimeRunningPrefix + "aho_corasick_tests", &aho_corasick_tests);

    tests.emplace_back("regular_expression_tests", &regular_expression_tests);

    tests.emplace_back("TestNameUtilitiesTests", &TestNameUtilitiesTests);

    tests.emplace_back("EditDistanceTests", &EditDistanceTests);

    tests.emplace_back("LongestCommonSubsequenceTests", &LongestCommonSubsequenceTests);

    tests.emplace_back("WordRecognizerTests", &WordRecognizerTests);
}