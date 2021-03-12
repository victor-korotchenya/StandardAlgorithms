# Discrete mathematics algorithms in C++

# Graphs

1. Adelson Velsky Landis tree is a balanced O(log(N)) BST where subtree heights can differ by max 1; only 4 rotations unlike 6 in the red-black tree.

1. Binary heap (priority queue) can be constructed in O(n); good to find min k-th for small k. The C++ STL std::set might be used as an alternative to push_heap, pop_heap.

1. Binary search + bit masks -> min vertex cover in O(2**n * n * log(n)).
   1. Binary search + some function can be fast enough for optimization, tip.

1. Binomial heap can merge in O(log(n)) using power of 2 size trees.

1. B-tree has O(log(n)) height, and can be used to implement a block file storage.

1. Create _random_ array, string, graph (directed, undirected, weighted), tree (simple, weighted) for testing.

1. Cycle find in directed graph using 3 colors, stack.

1. Cycle find in undirected graph using DFS, stack, depth array.

1. Double-ended priority queue DEPQ lets min and max finding and extraction.
   1. Min-max heap stores min(max) on even(odd) levels.
   1. Deap holds min(max) heap in its left(right) subtree. Any left node i has a corresponding right j (may be parent), and (i.item <= j.item).

1. Fibonacci heap to quickly add a node, merge, or decrease the key. 

1. Gale Shapley stable matching alg.

1. Graph girth (shortest cycle length) by BFS for each vertex.

1. Hamiltonian cycle of min cost (Karp TSP, NP-complete) using masks, start from 0, end with u, in O(2**n * n * n).
   1. Oftentimes an NP-complete problem requires masks, tip.

1. Hamiltonian path count using an ending vertex and vertex masks in O(2**n * n * n).

1. Hamiltonian path find using masks for outgoing vertices, cycle on vertex subset size, in O(2**n * n).
   1. For small n, use uint64_t as a bitset to reduce time complexity by n, tip.

1. Havel Hakimi to build a graph from a degree sequence in O(n*n).

1. Hierholzer to find Euler tour in Euler graph (has 0 or 2 odd degree vertices) using 2 stacks.

1. Is a graph complete, connected, simple, a tree?

1. LCA of a tree using sparse table in O(log(n)).
   1. If requests are given, offline Tarjan can find LCAs using DSU with set parent in O(1) per query.

1. Leftist height or weight tree.
   1. The height version can decrease key in log(n).

1. Maximum flow
   1. Dinic using BFS to find many shortest paths, node levels, DFS to add a flow.
   1. Max flow Edmonds Karp converges slowly.
   1. Min cost max flow; adj matrix min cost perfect matching using masks.

1. Meet in the middle, or bidirectional search can be faster than, for example, plain BFS, tip.

1. MST
   1. Boruvka parallel find a min-cost edge for each tree.
   1. Jarnik Prim by adding min cost edge to a final set.
   1. Kruskal go through sorted edges. DSU is used as in Boruvka.

1. Pair heap is expected to be fast in practice.
   1. A node stores its children via the first child, and then the next/prev.
   1. Merge every second sibling when finding min to minimize the tree height. 

1. Persistent segment tree, persistent trie - a new root for a change.

1. Segment/interval tree, where zi is sum/xor/gcd/lcm/min/max
   1. Fenwick tree in O(log(N)): add to a cell, save self + left subtree sum, range sum, no extra space.
   1. Segment tree to add/set 1, zi over a range, 2x space.
   1. Segment tree to add/set a range, zi at 1, 2x space.
   1. Segment tree to add/set/zi over a range using arrays of x and 2x size.

1. Shortest path single source:
   1. Dijkstra in O(n * n * log(n)) for a sparse graph, in O(n*n) for a dense graph.
   1. Bellman Ford (negative cost edges) in O(v*e).

1. Shortest paths all vertices: Floyd Warshall in O(n**3).

1. Simple cycle count using masks, min first node in a cycle.

1. Simple path count using masks.

1. Skip list is similar to BST, but uses randomization, deeper node has more links.

1. Splay tree is BST, placing the last found nodes closer to the root.
   1. Good for repetitive searches.
   2. Ugly for real-time, read-only storage, multi-threading.

1. Tarjan detect bridge edges (cut-edges, or cut arcs), and articulation points (cut-nodes, or cut vertices) in an undirected graph by 1 DFS.

1. Tarjan strongly connected components to solve 2-SAT in 2-CNF; impossible when both x, !x are in same SCC.

1. Threaded binary tree: 1 char to speed up the in-order tree traversal by pointing to the next node when right node = null.

1. Topological sort for DAG by queuing 0 in-degree vertices.
   1. Given a DAG of project activities, find: the earliest, latest completion times, a critical path of 0 slack edges.

1. Treap is an unbalanced BST, and a heap, where each node has random priority.

1. Tree decomposition
   1. Centroid node has max size of a subtree <= floor(n/2). Build centroid tree in O(n*log(n)).  Centroid tree to find closest marked node.
   1. Heavy edge goes to a child with max descendant count. Build O(n*log(n)*log(n))). Select min in path, update edge cost.

1. Trie (prefix tree, auto-complete), compressed or not, as a map for strings, or to find max XOR of 2 items.

1. Van Emde Boas tree recursively splits into blocks of size sqrt(n) to achieve log(log(n)) time for min, max, prev, next.




# Numbers

1. Bit operations: lowest bit, set bit, remove bit, pop count, next greater/smaller with same bit count, bit array/set.

1. Boyer Moore majority element, appearing > n/2 times in O(n).

1. Count valid brace strings given a pattern "[[??][??" using {index, balance} buffer.
   1. Some seemingly NP-hard difficult problems might be solved in polynomial time, tip.

1. Count ways: every man selects a pet of his preference list, chosen pets are unique. Each masked man takes a free pet or not.
   1. Uniqueness can be guaranteed via masks or a method argument, de/in-creasing with each recursive call, tip. 

1. Cuckoo hash table to search, erase in O(1) time.

1. Dependency reorganization can reduce space/time, tip.

1. Difference array: many range update operations {left, right, value to add} each in O(1) by setting ar[left] += val, ar[right+1] -= val; build the resulting array in O(n).

1. Disjoint set union-find data structure, or DSU, using partial path compression.

1. Dutch flag partition into {less, equal, greater} to be used in Quick select/sort.

1. Enumerate all subsets of a mask using binary AND (&).

1. Eratosthenes sieve using primes, min factor in O(n).

1. Enumerate valid Catalan/brace strings.

1. Factoring/decomposition into prime factors:
   1. Using precomputed min factor in O(log(n)).
   1. Simple cycle on odd numbers in O(sqrt(n)).

1. Fibonacci calculator using Linear Algebra in O(log(n)).
   1. Mathematics can solve some problems in about O(1) if you know or can derive a formula, tip.

1. Find item in 2D array sorted by each column, each row in O(m+n).

1. Hierarchical clustering in O(n**3) merging 2 closest nodes into a cluster.

1. Inversion count using merge sort in O(n*log(n)).

1. Knapsack max value, subset sum in pseudo-polynomial O(n*max(weight)).

1. Longest alternative subsequence (up, down, up...) in O(n).

1. Longest increasing subsequence in O(n*log(n)).

1. Lower bound in s sorted vectors in (n + log(s)) time via fractional cascading (appending every 2nd).

1. Matrix multiplication in O(n**3); fast power/exponentiation.

1. Max in each sub-array of size m <= n in O(n).

1. Min -1 or +1 adds to make an array increasing by using a priority queue/heap.

1. Mo algorithm to count the number of the distinct items in a range in O(n**1.5).

1. Multiply, add, subtract for signed, unsigned with overflow check.

1. N choose k modulo m using factorials and their modulo inverses in O(1) per query by preliminary computation.

1. Perfect read-only hash table to search in O(1) given distinct keys.

1. Sparse table min/max query in O(1) because min(a..d) = min(min(a..c), min(b..d)), but init in O(n*log(n)), has log(n) levels to store a power of 2 predecessor.

1. Sparse table in 2D to find min/max in a rectangle.

1. Sorted array 2-sum in O(n).

1. Sorted array 3-sum in O(n*n).

1. Subsequence O(n*(2**n)) and substring O(n*log(n)), both of min positive sum.




## Geometry

1. Angle between 3 distinct points in 2D plane.

1. Compress coordinates to reduce 1e9 to 1e5, tip.

1. Convex hull Graham scan in O(n*log(n)): find min point, sort by angle or min distance to it, process points 1-by-1 removing not convex penultimate.

1. Dot product, cross product of 2 points.




## Strings

1. Aho Corasick simultaneously find all preprocessed words in a text t in O(|t| + number of found words).

1. Rabin-Karp string matching. Count distinct substrings of given length using rolling hashes.

1. Edit distance, longest common subsequence of 2 strings in time and space O(m*n).
   1. If the matching subsequence is not needed, the previous and current buffers might reduce the space requirement to min(m, n), tip.
   1. Binary search on length + Rolling hash might run in O(m*log(n)).
   1. Quadratic complexity -> an algorithm not usable for big data.
   1. Hunt Szymanski improvement.

1. Enumerate all (Catalan number) 2n length valid brace strings.

1. Find a substring p in a string t.
   1. DFSA build in O(|p|), find in O(|t|).
   1. Knuth Morris Pratt same complexity as above.

1. Wildcard match in O(n**2) using either 2d array, or NDFSA and a set of reachable states for every letter.


# To build the project, set the env variables:

GSL_INCLUDE=C:\Projects\Microsoft\GSL\include
