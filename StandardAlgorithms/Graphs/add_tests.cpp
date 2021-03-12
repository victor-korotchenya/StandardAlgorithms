#include "add_tests.h"
#include "avl_tree_tests.h"
#include "AVLTreeTests.h"
#include "bellman_ford_shortest_paths_tests.h"
#include "BinaryIndexedTreeTests.h"
#include "bridge_detector_tests.h"
#include "BST_To_DoubleLinkedListTests.h"
#include "centroid_decomposition_tests.h"
#include "closest_select_update_tests.h"
#include "ConstructFromDistinctNumbersTests.h"
#include "DAG_LongestWeightedPathTests.h"
#include "detect_many_cycles_tests.h"
#include "difference_array__range_update_in_o1_tests.h"
#include "digraph_cycle_detector_tests.h"
#include "DijkstraAlgorithmTests.h"
#include "euler_path_tests.h"
#include "FreeTreeTests.h"
#include "Graph_Girth.h"
#include "HeapTests.h"
#include "InOrderThreadedNodeTests.h"
#include "is_graphic_degree_sequence_tests.h"
#include "IsBinarySearchTreeTests.h"
#include "IsTreeSymmetricalTests.h"
#include "LargestBSTSizeTests.h"
#include "LeastCommonAncestorTest.h"
#include "light_heavy_tree_decomposition_tests.h"
#include "lowest_common_ancestor_tests.h"
#include "max_flow_tests.h"
#include "min_cost_perfect_matching_tests.h"
#include "min_value_in_path_tests.h"
#include "min_weight_hamiltonian_path_tests.h"
#include "MinimalSpanningTreeTests.h"
#include "minimum_vertex_cover_tests.h"
#include "next_inorder_sucessor_tests.h"
#include "order_statistics_tree_tests.h"
#include "page_rank_tests.h"
#include "persistent_segment_tree_tests.h"
#include "persistent_trie_tests.h"
#include "PrefixTreeTests.h"
#include "ReConstructTreeTests.h"
#include "RevertedSegmentTreeTests.h"
#include "segm_tree_sum_invert_tests.h"
#include "segment_tree_add_bool_tests.h"
#include "segment_tree_tests.h"
#include "SegmentTreeTests.h"
#include "SkipListTests.h"
#include "sparse_table_min_tests.h"
#include "SparseGraphTests.h"
#include "SparseTable_RangeMinimumQuery_2DTests.h"
#include "StableMatchingTests.h"
#include "SuffixTreeTests.h"
#include "tarjan_scc_tests.h"
#include "TopologicalSortTests.h"
#include "tree_longest_path_tests.h"
#include "trie_map_tests.h"
#include "XorQueueTests.h"
//#include "PrefixTreePerformanceTest.h"

using namespace std;
using namespace Privet::Algorithms::Graphs::Tests;
using namespace Privet::Algorithms::Trees::Tests;
using namespace Privet::Algorithms;

void Privet::Algorithms::Graphs::Tests::add_tests(vector<pair<string, test_function>>& tests, const string& longTimeRunningPrefix)
{
    //tests.emplace_back("PrefixTreePerformanceTests", PrefixTreePerformanceTests);

    tests.emplace_back("order_statistics_tree_tests", &order_statistics_tree_tests);

    tests.emplace_back("closest_select_update_tests", &closest_select_update_tests);

    tests.emplace_back("sparse_table_min_tests", &sparse_table_min_tests);

    tests.emplace_back("trie_map_tests", trie_map_tests);

    tests.emplace_back("max_flow_tests", &max_flow_tests);

    tests.emplace_back("persistent_trie_tests", &persistent_trie_tests);

    tests.emplace_back("persistent_segment_tree_tests", &persistent_segment_tree_tests);

    tests.emplace_back("segment_tree_add_bool_tests", &segment_tree_add_bool_tests);

    tests.emplace_back("euler_path_tests", &euler_path_tests);

    tests.emplace_back("light_heavy_tree_decomposition_tests", &light_heavy_tree_decomposition_tests);

    tests.emplace_back("difference_array__range_update_in_o1_tests", &difference_array__range_update_in_o1_tests);

    tests.emplace_back("segment_tree_tests", &segment_tree_tests);

    tests.emplace_back(longTimeRunningPrefix + "segm_tree_sum_invert_tests", &segm_tree_sum_invert_tests);

    tests.emplace_back("min_value_in_path_tests", &min_value_in_path_tests);

    tests.emplace_back("centroid_decomposition_tests", &centroid_decomposition_tests);

    tests.emplace_back("euler_tour_tests", euler_tour_tests);

    tests.emplace_back("tree_longest_path_tests", &tree_longest_path_tests);

    tests.emplace_back("SparseTable_RangeMinimumQuery_2DTests", &SparseTable_RangeMinimumQuery_2DTests);

    tests.emplace_back("ConstructFromDistinctNumbersTests", &ConstructFromDistinctNumbersTests);

    tests.emplace_back("RevertedSegmentTreeTests", &RevertedSegmentTreeTests);

    tests.emplace_back("SegmentTreeTests", &SegmentTreeTests);

    tests.emplace_back("BinaryIndexedTreeTests", &BinaryIndexedTreeTests);

    tests.emplace_back(longTimeRunningPrefix + "PrefixTreeTests", &PrefixTreeTests);

    tests.emplace_back(longTimeRunningPrefix + "ReConstructTreeTests", &ReConstructTreeTests);

    tests.emplace_back(longTimeRunningPrefix + "avl_tree_tests", &avl_tree_tests);

    tests.emplace_back(longTimeRunningPrefix + "AVLTreeTests", &AVLTreeTests);

    tests.emplace_back(longTimeRunningPrefix + "MST_PerformanceTests", &MST_PerformanceTests);

    //#ifndef _DEBUG
    tests.emplace_back(longTimeRunningPrefix + "LeastCommonAncestorTest", &LeastCommonAncestorTest);
    //#endif

    tests.emplace_back("BST_To_DoubleLinkedListTests", &BST_To_DoubleLinkedListTests);

    tests.emplace_back("FreeTreeTests", &FreeTreeTests);

    tests.emplace_back("IsTreeSymmetricalTests", &IsTreeSymmetricalTests);

    tests.emplace_back("LargestBSTSizeTests", &LargestBSTSizeTests);

    tests.emplace_back("IsBinarySearchTreeTests", &IsBinarySearchTreeTests);

    tests.emplace_back("next_inorder_sucessor_tests", &next_inorder_sucessor_tests);

    tests.emplace_back("SuffixTreeTests", &SuffixTreeTests);

    tests.emplace_back("InOrderThreadedNodeTests", &InOrderThreadedNodeTests);

    tests.emplace_back("lowest_common_ancestor_tests", &lowest_common_ancestor_tests);

    tests.emplace_back(longTimeRunningPrefix + "HeapTests", &HeapTests);

    tests.emplace_back("XorQueueTests", &XorQueueTests);

    tests.emplace_back("SkipListTests", &SkipListTests);

    tests.emplace_back("bellman_ford_shortest_paths_tests", &bellman_ford_shortest_paths_tests);

    tests.emplace_back(longTimeRunningPrefix + "min_cost_perfect_matching_tests", &min_cost_perfect_matching_tests);

    tests.emplace_back(longTimeRunningPrefix + "bridge_detector_tests", &bridge_detector_tests);

    tests.emplace_back(longTimeRunningPrefix + "min_weight_hamiltonian_path_tests", &min_weight_hamiltonian_path_tests);

    tests.emplace_back(longTimeRunningPrefix + "minimum_vertex_cover_tests", &minimum_vertex_cover_tests);

    tests.emplace_back("detect_many_cycles_tests", &detect_many_cycles_tests);

    tests.emplace_back("is_graphic_degree_sequence_tests", &is_graphic_degree_sequence_tests);

    tests.emplace_back("tarjan_scc_tests", &tarjan_scc_tests);

    tests.emplace_back("TopologicalSortTests", &TopologicalSortTests);

    tests.emplace_back("page_rank_tests", &page_rank_tests);

    tests.emplace_back("digraph_cycle_detector_tests", &digraph_cycle_detector_tests);

    tests.emplace_back("SparseGraphTests", &SparseGraphTests);

    tests.emplace_back("MinimalSpanningTreeTests", &MinimalSpanningTreeTests);

    tests.emplace_back("DAG_LongestWeightedPathTests", &DAG_LongestWeightedPathTests);

    tests.emplace_back("DijkstraAlgorithmTests", &DijkstraAlgorithmTests);

    tests.emplace_back("FindGirthTests", &FindGirthTests);

    tests.emplace_back("StableMatchingTests", &StableMatchingTests);
}