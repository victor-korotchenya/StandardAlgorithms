#pragma once

#include <iomanip> // setprecision
#include <ostream>
#include <utility>
#include "../Utilities/StreamUtilities.h"

namespace Privet
{
    namespace Algorithms
    {
        template <typename TVertex, typename TWeight>
        struct WeightedVertex final
        {
            TVertex Vertex;
            TWeight Weight;

            WeightedVertex(const TVertex vertex = {}, const TWeight weight = {})
                : Vertex(vertex), Weight(weight)
            {
            }

            std::pair<int, int64_t> to_pair() const
            {
                return { Vertex, Weight };
            }

            bool operator ==(
                const WeightedVertex<TVertex, TWeight>& b) const
            {
                const bool result = Vertex == b.Vertex && Weight == b.Weight;
                return result;
            }

            //Implements less or equal by weight.
            bool operator()(
                const WeightedVertex<TVertex, TWeight>& a,
                const WeightedVertex<TVertex, TWeight>& b) const
            {
                const bool result = a.Weight <= b.Weight;
                return result;
            }

            explicit operator TVertex() const
            {
                return Vertex;
            }

            friend std::ostream& operator <<
                (std::ostream& str, WeightedVertex const& edge)
            {
                str << "Vertex=" << edge.Vertex
                    << ", Weight=";

#pragma warning(push)
#pragma warning(disable : 4127)
                if (std::is_same<TWeight, double>::value)
                {
                    const auto prec = StreamUtilities::GetMaxDoublePrecision();
                    str << std::setprecision(prec) << edge.Weight;
                }
                else
                {
                    str << edge.Weight;
                }
#pragma warning(pop)

                return str;
            }
        };

        template <typename TVertex = int, typename TWeight = int>
        struct WeightedVertex_WeightGreater final
        {
            //Maximum weight.
            bool operator()(
                const WeightedVertex<TVertex, TWeight>& a,
                const WeightedVertex<TVertex, TWeight>& b) const
            {
                const auto result = b.Weight < a.Weight;
                return result;
            }
        };

        //template <typename TVertex = int, typename TWeight = int>
        //struct WeightedVertex_WeightEqual final
        //{
        //  //Implements equality by weight.
        //  bool inline operator()(
        //    const WeightedVertex<TVertex, TWeight> &a,
        //    const WeightedVertex<TVertex, TWeight> &b) const
        //  {
        //    const bool result = a.Weight == b.Weight;
        //    return result;
        //  }
        //};
    }
}
