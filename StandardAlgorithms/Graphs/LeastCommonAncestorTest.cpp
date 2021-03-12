#include <vector>
#include "BinaryTreeNode.h"
#include "BinaryTreeUtilities.h"
#include "../Utilities/VectorUtilities.h"
#include "NodeUtilities.h"
#include "LeastCommonAncestorTest.h"
#include "../Numbers/Arithmetic.h"
#include "../Utilities/StreamUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

namespace
{
    using TNodeAncestor = Privet::Algorithms::Trees
        ::BinaryTreeNode<long long int>;

    using AncestorNumber = TNodeAncestor::value_type;

    static void CreateTree(std::vector< TNodeAncestor*>& nodes);

    static void CheckAncestors(const std::vector< TNodeAncestor*>& nodes);

    //Zero based numbering.
    static size_t CommonAncestorForCompleteTreeSlow(size_t node1, size_t node2);

    //One base numbering - extra 1 allows easier calculations.
    static size_t CommonAncestorForCompleteTreeFast(size_t node1, size_t node2);

    void CreateTree(
        vector< TNodeAncestor*>& nodes)
    {
        const size_t nodesCount = nodes.size();
        const size_t maxParentIndex = NodeUtilities::ParentNode(nodesCount - 1);

        for (size_t parentIndex = 0; parentIndex <= maxParentIndex; ++parentIndex)
        {
            const size_t child1 = NodeUtilities::LeftChild(parentIndex);
            NodeUtilities::SetLeft(nodes, parentIndex, child1);

            const size_t child2 = NodeUtilities::RightChild(parentIndex);
            NodeUtilities::SetRight(nodes, parentIndex, child2);
        }
    }

    void CheckAncestors(
        const vector< TNodeAncestor*>& nodes)
    {
        const size_t nodesCount = nodes.size();

        const TNodeAncestor* const root = const_cast<const TNodeAncestor* const>(nodes[0]);

        for (size_t index1 = 0; index1 < nodesCount; ++index1)
        {
            for (size_t index2 = 0; index2 < nodesCount; ++index2)
            {
                const TNodeAncestor* const node1 = const_cast<const TNodeAncestor* const>(nodes[index1]);
                const TNodeAncestor* const node2 = const_cast<const TNodeAncestor* const>(nodes[index2]);

                const TNodeAncestor* const actual =
                    BinaryTreeUtilities< AncestorNumber >::LeastCommonAncestorWithoutParent(
                        root, node1, node2);

                if (nullptr == actual)
                {
                    ostringstream ss;
                    ss << "Error in LeastCommonAncestorTest:"
                        << " null returned for index1=" << index1
                        << ", index2=" << index2 << ".";
                    const string s = ss.str();
                    throw runtime_error(s.c_str());
                }

                const size_t expectedIndex = CommonAncestorForCompleteTreeSlow(index1, index2);
                const size_t expectedIndex2 = CommonAncestorForCompleteTreeFast(1 + index1, 1 + index2);
                if (expectedIndex + 1 != expectedIndex2)
                {
                    ostringstream ss;
                    ss << "Error in CommonAncestorForCompleteTreeFast:"
                        << " expectedIndex (" << expectedIndex
                        << ") != expectedIndex2 (" << expectedIndex2 - 1
                        << ") when index1=" << index1
                        << ", index2=" << index2 << ".";
                    const string s = ss.str();
                    throw runtime_error(s.c_str());
                }

                const TNodeAncestor* const expected = nodes[expectedIndex];
                if (expected != actual)
                {
                    ostringstream ss;
                    ss << "Error in LeastCommonAncestorTest:"
                        << " expect " << expectedIndex
                        << " but got " << actual->Value
                        << " when index1=" << index1
                        << ", index2=" << index2 << ".";
                    const string s = ss.str();
                    throw runtime_error(s.c_str());
                }
            }
        }
    }

    //Zero base numbering.
    size_t CommonAncestorForCompleteTreeSlow(
        size_t node1, size_t node2)
    {
        for (;;)
        {
            //if (0 == node1 || 0 == node2)
            //{
            //  return 0;
            //}

            if (node1 == node2)
            {
                return node1;
            }

            if (node2 < node1)
            {
                node1 = NodeUtilities::ParentNode(node1);
            }
            else
            {
                node2 = NodeUtilities::ParentNode(node2);
            }
        }
    }

    //One base numbering - extra 1 allows easier calculations.
    size_t CommonAncestorForCompleteTreeFast(
        size_t node1, size_t node2)
    {
#ifdef _DEBUG
        if (0 == node1 || 0 == node2)
        {
            ostringstream ss;
            ss << "The nodes (" << node1 << ", " << node2
                << ") must be positive (1 based).";
            const string s = ss.str();
            throw runtime_error(s.c_str());
        }
#endif
        //It is rare in a big tree.
        //if (1 == node1 || 1 == node2)
        //{
        //  return 1;
        //}

        const int level1 = MostSignificantBit64(node1);
        const int level2 = MostSignificantBit64(node2);
        const auto levelDelta = abs(level1 - level2);

        //Equate the same level.
        if (level1 < level2)
        {
            node2 = node2 >> levelDelta;
        }
        else if (level2 < level1)
        {
            node1 = node1 >> levelDelta;
        }

        if (node1 == node2)
        {
            return node1;
        }

        const auto columnDistance = MostSignificantBit64(node1 ^ node2);
        const unsigned long long int result = node1 >> (1 + columnDistance);
#ifdef _DEBUG
        if (0 == result)
        {
            ostringstream ss;
            ss << "Error in CommonAncestorForCompleteTreeFast:"
                << " got zero result. node1=" << node1 << ", node2=" << node2
                << ".";

            StreamUtilities::throw_exception(ss);
        }
#endif
        return result;
    }
}

void Privet::Algorithms::Trees::Tests::LeastCommonAncestorTest()
{
    vector< TNodeAncestor* > nodes;

    try
    {
        const int nodesCount = (1 << 3) - 1;
        NodeUtilities::CreateNodes(nodes, nodesCount);
        CreateTree(nodes);
        CheckAncestors(nodes);
    }
    catch (const exception&)
    {
        VectorUtilities::FreeAndClear(nodes);

        throw;
    }
}