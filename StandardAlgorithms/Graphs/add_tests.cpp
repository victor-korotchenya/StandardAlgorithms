#include"add_tests.h"
#include"avl_tree_tests.h"
#include"b_tree_tests.h"
#include"bellman_ford_shortest_paths_tests.h"
#include"binary_heap_tests.h"
#include"binary_indexed_tree_tests.h"
#include"binary_tree_serialize_tests.h"
#include"binomial_heap_tests.h"
#include"boruvka_mst_tests.h"
#include"bridge_detector_tests.h"
#include"bst_to_doubly_linked_list_tests.h"
#include"centroid_decomposition_tests.h"
#include"clique_maximum_tests.h"
#include"closest_select_update_tests.h"
#include"construct_from_distinct_numbers_tests.h"
#include"dag_longest_weighted_path_tests.h"
#include"deap_tests.h"
#include"detect_many_cycles_tests.h"
#include"difference_array_range_update_in_o1_tests.h"
#include"digraph_cycle_detector_tests.h"
#include"dijkstra_tests.h"
#include"euler_tour_tests.h"
#include"fibonacci_heap_tests.h"
#include"free_tree_tests.h"
#include"graph_girth_tests.h"
#include"hungarian_matching_tests.h"
#include"in_order_threaded_node_tests.h"
#include"interval_tree_tests.h"
#include"is_bst_tests.h"
#include"is_graphic_degree_sequence_tests.h"
#include"is_singly_connected_graph_tests.h"
#include"is_tree_symmetrical_tests.h"
#include"k_d_tree_tests.h"
#include"largest_bst_size_tests.h"
#include"least_common_ancestor_tests.h"
#include"leftist_height_biased_tree_tests.h"
#include"light_heavy_tree_decomposition_tests.h"
#include"lowest_common_ancestor_tests.h"
#include"max_2c_matching_karzanov_hopcroft_karp_tests.h"
#include"max_flow_tests.h"
#include"max_weight_cycle_cover_tests.h"
#include"min_cost_perfect_matching_tests.h"
#include"min_cut_stoer_wagner_tests.h"
#include"min_max_heap_tests.h"
#include"min_value_in_path_tests.h"
#include"min_weight_hamiltonian_path_tests.h"
#include"minimal_spanning_tree_tests.h"
#include"minimum_set_cover_tests.h"
#include"minimum_vertex_cover_tests.h"
#include"next_inorder_successor_tests.h"
#include"one_based_index_t_tests.h"
#include"optimal_bst_tests.h"
#include"order_statistics_tree_tests.h"
#include"page_rank_tests.h"
#include"persistent_segment_tree_tests.h"
#include"persistent_trie_tests.h"
#include"prefix_codes_tests.h"
#include"prefix_tree_performance_tests.h"
#include"prefix_tree_tests.h"
#include"range_tree_tests.h"
#include"reconstruct_tree_tests.h"
#include"red_black_tree_tests.h"
#include"reverted_segment_tree_tests.h"
#include"segm_tree_sum_invert_tests.h"
#include"segment_tree_add_bool_tests.h"
#include"segment_tree_lazy_tests.h"
#include"segment_tree_tests.h"
#include"skip_list_tests.h"
#include"sparse_graph_tests.h"
#include"splay_tree_tests.h"
#include"stable_matching_tests.h"
#include"suffix_tree_tests.h"
#include"tarjan_scc_tests.h"
#include"topological_sort_tests.h"
#include"treap_tests.h"
#include"tree_longest_path_tests.h"
#include"trie_map_tests.h"
#include"van_emde_boas_tree_tests.h"
#include"work_critical_path_tests.h"

namespace
{
    constexpr auto five = 5;
}

void Standard::Algorithms::Graphs::Tests::add_tests(
    std::vector<test_function> &tests, const std::string &long_time_running_prefix)
{
    // tests.emplace_back("prefix_tree_performance_tests",
    // &Standard::Algorithms::Trees::Tests::prefix_tree_performance_tests, 1);

    // tests.emplace_back("range_tree_tests", &Standard::Algorithms::Trees::Tests::range_tree_tests, 1);

    tests.emplace_back("one_based_index_t_tests", &Standard::Algorithms::Heaps::Tests::one_based_index_t_tests, 1);

    tests.emplace_back("treap_tests", &Standard::Algorithms::Heaps::Tests::treap_tests, 1);

    tests.emplace_back("deap_tests", &Standard::Algorithms::Heaps::Tests::deap_tests, 1);

    tests.emplace_back("min_max_heap_tests", &Standard::Algorithms::Heaps::Tests::min_max_heap_tests, 1);

    tests.emplace_back("van_emde_boas_tree_tests", &van_emde_boas_tree_tests, 1);

    tests.emplace_back("splay_tree_tests", &Standard::Algorithms::Trees::Tests::splay_tree_tests, 1);

    tests.emplace_back(
        "leftist_height_biased_tree_tests", &Standard::Algorithms::Trees::Tests::leftist_height_biased_tree_tests, 1);

    tests.emplace_back("b_tree_tests", &Standard::Algorithms::Trees::Tests::b_tree_tests, 1);

    tests.emplace_back("fibonacci_heap_tests", &Standard::Algorithms::Heaps::Tests::fibonacci_heap_tests, 1);

    tests.emplace_back("boruvka_mst_tests", &boruvka_mst_tests, 1);

    tests.emplace_back("binomial_heap_tests", &Standard::Algorithms::Heaps::Tests::binomial_heap_tests, 1);

    tests.emplace_back(
        "order_statistics_tree_tests", &Standard::Algorithms::Trees::Tests::order_statistics_tree_tests, 1);

    tests.emplace_back(
        "closest_select_update_tests", &Standard::Algorithms::Trees::Tests::closest_select_update_tests, 1);

    tests.emplace_back("trie_map_tests", Standard::Algorithms::Trees::Tests::trie_map_tests, 1);

    tests.emplace_back("min_cut_stoer_wagner_tests", &min_cut_stoer_wagner_tests, 3);

    tests.emplace_back("max_flow_tests", &max_flow_tests, 4);

    tests.emplace_back("persistent_trie_tests", &Standard::Algorithms::Trees::Tests::persistent_trie_tests, 1);

    tests.emplace_back(
        "persistent_segment_tree_tests", &Standard::Algorithms::Trees::Tests::persistent_segment_tree_tests, 2);

    tests.emplace_back(
        "segment_tree_add_bool_tests", &Standard::Algorithms::Trees::Tests::segment_tree_add_bool_tests, 1);

    tests.emplace_back("lca_euler_tour_tests", &Standard::Algorithms::Trees::Tests::lca_euler_tour_tests, 1);

    tests.emplace_back("light_heavy_tree_decomposition_tests",
        &Standard::Algorithms::Trees::Tests::light_heavy_tree_decomposition_tests, 1);

    tests.emplace_back("difference_array_range_update_in_o1_tests",
        &Standard::Algorithms::Trees::Tests::difference_array_range_update_in_o1_tests, 2);

    tests.emplace_back("k_d_tree_tests", &Standard::Algorithms::Trees::Tests::k_d_tree_tests, 1);

    tests.emplace_back("segment_tree_tests", &Standard::Algorithms::Trees::Tests::segment_tree_tests, 1);

    tests.emplace_back(long_time_running_prefix + "segm_tree_sum_invert_tests",
        &Standard::Algorithms::Trees::Tests::segm_tree_sum_invert_tests, 1);

    tests.emplace_back("min_value_in_path_tests", &Standard::Algorithms::Trees::Tests::min_value_in_path_tests, 1);

    tests.emplace_back(
        "centroid_decomposition_tests", &Standard::Algorithms::Trees::Tests::centroid_decomposition_tests, 1);

    tests.emplace_back("euler_tour_tests", &euler_tour_tests, 2);

    tests.emplace_back("tree_longest_path_tests", &Standard::Algorithms::Trees::Tests::tree_longest_path_tests, 1);

    tests.emplace_back("construct_from_distinct_numbers_tests",
        &Standard::Algorithms::Trees::Tests::construct_from_distinct_numbers_tests, 1);

    tests.emplace_back(
        "reverted_segment_tree_tests", &Standard::Algorithms::Trees::Tests::reverted_segment_tree_tests, 1);

    tests.emplace_back("segment_tree_lazy_tests", &Standard::Algorithms::Trees::Tests::segment_tree_lazy_tests, 2);

    tests.emplace_back("binary_indexed_tree_tests", &Standard::Algorithms::Trees::Tests::binary_indexed_tree_tests, 2);

    tests.emplace_back("prefix_codes_tests", &Standard::Algorithms::Trees::Tests::prefix_codes_tests, 2);

    tests.emplace_back(
        long_time_running_prefix + "prefix_tree_tests", &Standard::Algorithms::Trees::Tests::prefix_tree_tests, 1);

    tests.emplace_back(
        "binary_tree_serialize_tests", &Standard::Algorithms::Trees::Tests::binary_tree_serialize_tests, 2);

    tests.emplace_back(long_time_running_prefix + "reconstruct_tree_tests",
        &Standard::Algorithms::Trees::Tests::reconstruct_tree_tests, 1);

    tests.emplace_back(
        long_time_running_prefix + "red_black_tree_tests", &Standard::Algorithms::Trees::Tests::red_black_tree_tests, 1);

    tests.emplace_back("interval_tree_tests", &Standard::Algorithms::Trees::Tests::interval_tree_tests, 1);

    tests.emplace_back(
        long_time_running_prefix + "avl_tree_tests", &Standard::Algorithms::Trees::Tests::avl_tree_tests, 1);

    tests.emplace_back(long_time_running_prefix + "mst_performance_tests", &mst_performance_tests, 1);

    tests.emplace_back(long_time_running_prefix + "least_common_ancestor_tests",
        &Standard::Algorithms::Trees::Tests::least_common_ancestor_tests, 2);

    tests.emplace_back(
        "bst_to_doubly_linked_list_tests", &Standard::Algorithms::Trees::Tests::bst_to_doubly_linked_list_tests, 1);

    tests.emplace_back("free_tree_tests", &Standard::Algorithms::Trees::Tests::free_tree_tests, 1);

    tests.emplace_back("is_tree_symmetrical_tests", &Standard::Algorithms::Trees::Tests::is_tree_symmetrical_tests, 1);

    tests.emplace_back("largest_bst_size_tests", &Standard::Algorithms::Trees::Tests::largest_bst_size_tests, 1);

    tests.emplace_back("is_bst_tests", &Standard::Algorithms::Trees::Tests::is_bst_tests, 2);

    tests.emplace_back(
        "next_inorder_successor_tests", &Standard::Algorithms::Trees::Tests::next_inorder_successor_tests, 1);

    tests.emplace_back("suffix_tree_tests", &Standard::Algorithms::Trees::Tests::suffix_tree_tests, 1);

    tests.emplace_back(
        "in_order_threaded_node_tests", &Standard::Algorithms::Trees::Tests::in_order_threaded_node_tests, 1);

    tests.emplace_back(
        "lowest_common_ancestor_tests", &Standard::Algorithms::Trees::Tests::lowest_common_ancestor_tests, 3);

    tests.emplace_back(
        long_time_running_prefix + "binary_heap_tests", &Standard::Algorithms::Heaps::Tests::binary_heap_tests, 2);

    tests.emplace_back("skip_list_tests", &Standard::Algorithms::Trees::Tests::skip_list_tests, 1);

    tests.emplace_back("bellman_ford_shortest_paths_tests", &bellman_ford_shortest_paths_tests, 2);

    tests.emplace_back("max_weight_cycle_cover_tests", &max_weight_cycle_cover_tests, 1);

    tests.emplace_back("hungarian_matching_tests", &hungarian_matching_tests, 1);

    tests.emplace_back(
        "max_2c_matching_karzanov_hopcroft_karp_tests", &max_2c_matching_karzanov_hopcroft_karp_tests, 1);

    tests.emplace_back(
        long_time_running_prefix + "min_cost_perfect_matching_tests", &min_cost_perfect_matching_tests, 3);

    tests.emplace_back(long_time_running_prefix + "bridge_detector_tests", &bridge_detector_tests, 3);

    tests.emplace_back(long_time_running_prefix + "min_weight_hamiltonian_path_tests",
        &min_weight_hamiltonian_path_tests, five * 3 - 1);

    tests.emplace_back(long_time_running_prefix + "minimum_vertex_cover_tests", &minimum_vertex_cover_tests, 3);

    tests.emplace_back("minimum_set_cover_tests", &minimum_set_cover_tests, 2);

    tests.emplace_back("optimal_bst_tests", &Standard::Algorithms::Trees::Tests::optimal_bst_tests, 2);

    tests.emplace_back("detect_many_cycles_tests", &detect_many_cycles_tests, 2);

    tests.emplace_back("clique_maximum_tests", &clique_maximum_tests, 2);

    tests.emplace_back("is_graphic_degree_sequence_tests", &is_graphic_degree_sequence_tests, 1);

    tests.emplace_back("tarjan_scc_tests", &tarjan_scc_tests, 1);

    tests.emplace_back("work_critical_path_tests", &work_critical_path_tests, 3);

    tests.emplace_back("is_singly_connected_graph_tests", &is_singly_connected_graph_tests, 2);

    tests.emplace_back("topological_sort_tests", &topological_sort_tests, 2);

    tests.emplace_back("page_rank_tests", &page_rank_tests, 1);

    tests.emplace_back("digraph_cycle_detector_tests", &digraph_cycle_detector_tests, 1);

    tests.emplace_back("sparse_graph_tests", &sparse_graph_tests, 1);

    tests.emplace_back("minimal_spanning_tree_tests", &minimal_spanning_tree_tests, 2);

    tests.emplace_back("dag_longest_weighted_path_tests", &dag_longest_weighted_path_tests, 2);

    tests.emplace_back("stable_matching_tests", &stable_matching_tests, 1);

    tests.emplace_back("dijkstra_tests", &dijkstra_tests, five);

    tests.emplace_back("graph_girth_tests", &graph_girth_tests, 1);
}
