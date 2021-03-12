#pragma once
//#include <unordered_map>
#include <queue>
#include "../Graphs/trie_map.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Numbers
        {
            // Count min number of copy-paste operations to sort a permutation using 2 waves meeting, or meet in the middle, or bidirectional search.
            // Time about O(n!*n**4), space O(n!).
            template<class string_t, class char_t = typename string_t::value_type,
                // Could be made 3x faster when using 'simple_trie_map_node_t'.
                class map_t = Privet::Algorithms::Trees::trie_map<char_t,
                // distance+1, is_target
                std::pair<int, bool>, Privet::Algorithms::Trees::simple_trie_map_node_t<char_t, std::pair<int, bool>, '0', '9'>>>
                std::pair<int, size_t> min_copy_paste_to_sort_permutation(const string_t& source)
            {
                const auto size = static_cast<int>(source.size());
                if (size <= 1)
                    return {};

                auto ordered = source;
                // todo: p3. Counting sort could suit here.
                std::sort(ordered.begin(), ordered.end());

                auto start = 0;
                while (start < size && source[start] == ordered[start])
                    ++start;

                auto size1 = size;
                while (start < size1 && source[size1 - 1] == ordered[size1 - 1])
                    --size1;

                if (start == size1)
                    return {};

                if (start + 2 == size1)
                    return { 1,0 };

                assert(size1 >= 3);

                map_t str_infos;
                std::queue<std::pair<string_t, std::pair<int, bool>>> q;

                {
                    // Use 1 as non-zero value.
                    const auto si = std::make_pair(1, false);
                    if (!start && size == size1)
                    {
                        str_infos[source] = si;
                        q.push({ source, si });
                    }
                    else
                    {
                        size1 -= start;
                        assert(size1 >= 3);

                        string_t s2 = source.substr(start, size1);
                        ordered = ordered.substr(start, size1);

                        str_infos[s2] = si;
                        q.push({ std::move(s2),si });
                    }
                }

                {
                    const auto ti = std::make_pair(1, true);
                    str_infos[ordered] = ti;
                    q.push({ ordered, ti });
                }

                size_t count = 2;
                do
                {
                    const auto str = std::move(q.front().first);
                    const auto info = std::make_pair(q.front().second.first + 1, q.front().second.second);
                    assert(info.first > 0);
                    q.pop();

                    for (auto len = 1; len < size1; ++len)
                    {
                        const auto rs = size1 - len;
                        for (auto from = 0; from <= rs; ++from)
                        {
                            // aBcd -> Bacd, aBcd (skip), acBd, acdB.
                            const string_t cut = str.substr(from, len),
                                rest = str.substr(0, from) + str.substr(from + len);

                            for (int x = from == 0; x <= rs; ++x)
                            {
                                auto cand = rest.substr(0, x) + cut + rest.substr(x);

                                auto& info2 = str_infos[cand];
                                if (!info2.first)
                                {
                                    info2 = info;
                                    q.push({ std::move(cand), info });
                                    ++count;
                                }
                                else if (info.second != info2.second)
                                {
                                    return { info.first + info2.first -
                                        // Initially added two 1s.
                                        2,
                                        count };
                                }
                            }
                        }
                    }
                } while (q.size());

                assert(0);
                throw std::runtime_error("min_copy_paste_to_sort_permutation inner error.");
            }

            // Slow time O(n!*n**4), space O(n!).
            template<class string_t, class char_t = typename string_t::value_type,
                //class map_t = std::unordered_map<string_t, int>>
                // Could be made faster when using 'simple_trie_map_node_t'.
                class map_t = Privet::Algorithms::Trees::trie_map<char_t,
                // distance+1
                int, Privet::Algorithms::Trees::simple_trie_map_node_t<char_t, int, '0', '9'>>>
                std::pair<int, size_t> min_copy_paste_to_sort_permutation_slow(const string_t& source)
            {
                const auto size = static_cast<int>(source.size());
                if (size <= 1)
                    return {};

                auto ordered = source;
                // todo: p3. Counting sort could suit here.
                std::sort(ordered.begin(), ordered.end());

                if (source == ordered)
                    return {};

                assert(size >= 2);
                map_t str_distance;
                std::queue<std::pair<string_t, int>> q;

                str_distance[source] = 1;
                q.push({ source, 1 });

                str_distance[ordered] = -1;
                //string_t buf(size, 0); // it is 2.5 times slower
                do
                {
                    const auto str = std::move(q.front().first);
                    const auto dist1 = q.front().second + 1;
                    assert(dist1 > 0);
                    q.pop();

                    for (auto len = 1; len < size; ++len)
                    {
                        const auto rs = size - len;
                        for (auto from = 0; from <= rs; ++from)
                        {
                            // aBcd -> Bacd, aBcd (skip), acBd, acdB.
                            const auto cut = str.substr(from, len);
                            const string_t rest = str.substr(0, from) + str.substr(from + len);

                            for (int x = from == 0; x <= rs; ++x)
                            {
                                const auto buf = rest.substr(0, x) + cut + rest.substr(x);
                                //std::copy(rest.begin(), rest.begin() + x, buf.begin());
                                //std::copy(str.begin() + from, str.begin() + from + len, buf.begin() + x);
                                ////std::copy(cut.begin(), cut.end(), buf.begin() + x);
                                //std::copy(rest.begin() + x, rest.end(), buf.begin() + x + len);

                                auto& d = str_distance[buf];
                                if (!d)
                                {
                                    d = dist1;
                                    q.push({ buf, dist1 });
                                }
                                else if (d < 0)
                                {
                                    assert(ordered == buf);
                                    return { dist1 - 1, str_distance.size() };
                                }
                            }
                        }
                    }
                } while (q.size());

                assert(0);
                throw std::runtime_error("min_copy_paste_to_sort_permutation inner error.");
            }
        }
    }
}