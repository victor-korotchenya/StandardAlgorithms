#pragma once
#include <vector>
#include "../Utilities/ExceptionUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        namespace Graphs
        {
            //It is edge effective, but vertex defective -(.
            template<typename number_t>
            class SparseGraph final
            {
                std::vector<number_t> _values, _previous, _lasts;
                number_t _counter;

            public:

                SparseGraph(number_t vertex_count, number_t edge_count)
                    : _values(RequirePositive(edge_count, "Edge count") + 1ll),
                    _previous(edge_count + 1ll),
                    _lasts(RequirePositive(vertex_count, "Vertex count")),
                    _counter(0)
                {
                }

                void add_edge(number_t a, number_t b);
            };

            template <typename number_t>
            void SparseGraph<number_t>::add_edge(number_t a, number_t b)
            {
                if (_values.size() <= _counter + 1)
                {
                    std::ostringstream ss;
                    ss << "Cannot add edges as the counter has maximum value("
                        << _counter << ").";
                    throw StreamUtilities::throw_exception<std::out_of_range>(ss);
                }

                if (_lasts.size() <= a)
                {
                    std::ostringstream ss;
                    ss << "Cannot add the edge (" << a << ", " << b
                        << ") as the first vertex is out of range.";
                    throw StreamUtilities::throw_exception<std::out_of_range>(ss);
                }
                if (_lasts.size() <= b)
                {
                    std::ostringstream ss;
                    ss << "Cannot add the edge (" << a << ", " << b
                        << ") as the second vertex is out of range.";
                    throw StreamUtilities::throw_exception<std::out_of_range>(ss);
                }

                ++_counter;

                _values[_counter] = b;
                _previous[_counter] = _lasts[a];
                _lasts[a] = _counter;
            }
        }
    }
}