#include "next_inorder_sucessor_tests.h"
#include "BinarySearchTreeUtilities.h"
#include "../Utilities/VectorUtilities.h"
#include "NodeUtilities.h"
#include "../Utilities/StreamUtilities.h"
#include "../Assert.h"
#include "BinaryTreeNodeWithParent.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

using TBinaryNode = BinaryTreeNodeWithParent<long int>;
using NumberNext = TBinaryNode::value_type;

void CreateTree(vector<TBinaryNode*>& nodes)
{
    NodeUtilities::SetLeftWithParent(nodes, 10, 5);
    NodeUtilities::SetRightWithParent(nodes, 10, 17);

    NodeUtilities::SetLeftWithParent(nodes, 5, 3);
    NodeUtilities::SetRightWithParent(nodes, 5, 7);

    NodeUtilities::SetLeftWithParent(nodes, 17, 14);
    NodeUtilities::SetRightWithParent(nodes, 17, 20);

    NodeUtilities::SetLeftWithParent(nodes, 3, 1);
    NodeUtilities::SetRightWithParent(nodes, 3, 4);

    NodeUtilities::SetLeftWithParent(nodes, 7, 6);
    NodeUtilities::SetRightWithParent(nodes, 7, 8);

    NodeUtilities::SetLeftWithParent(nodes, 14, 12);
    NodeUtilities::SetRightWithParent(nodes, 14, 15);

    NodeUtilities::SetLeftWithParent(nodes, 20, 19);
    NodeUtilities::SetRightWithParent(nodes, 20, 23);
    //                        10
    //            5                            17
    //      3           7            14                20
    //  1     4       6   8       12    15         19       23
    //0   2                 9   11  13    16     18       22  24
    //                                                  21
    NodeUtilities::SetLeftWithParent(nodes, 1, 0);
    NodeUtilities::SetRightWithParent(nodes, 1, 2);

    NodeUtilities::SetRightWithParent(nodes, 8, 9);

    NodeUtilities::SetLeftWithParent(nodes, 12, 11);
    NodeUtilities::SetRightWithParent(nodes, 12, 13);

    NodeUtilities::SetRightWithParent(nodes, 15, 16);

    NodeUtilities::SetLeftWithParent(nodes, 19, 18);

    NodeUtilities::SetLeftWithParent(nodes, 23, 22);
    NodeUtilities::SetRightWithParent(nodes, 23, 24);

    NodeUtilities::SetLeftWithParent(nodes, 22, 21);
}

NumberNext ExpectedSuccessor(const NumberNext value, const NumberNext lastNodeValue)
{
    const NumberNext result = value < lastNodeValue ? 1 + value : 0 - (NumberNext(1));
    return result;
}

void CheckSuccessors(vector< TBinaryNode*>& nodes)
{
    const size_t nodesCount = nodes.size();
    Assert::Greater(nodesCount, 1, "nodesCount");

    const NumberNext lastNodeValue = NumberNext(nodesCount) - 1;

    for (int i = 0; i < nodesCount; ++i)
    {
        const NumberNext expected = ExpectedSuccessor(i, lastNodeValue);

        TBinaryNode* node = nodes[i];

        const TBinaryNode* const sucessor =
            BinarySearchTreeUtilities< NumberNext, TBinaryNode >::NextInOrderSucessor(node);
        if (nullptr == sucessor)
        {
            if (lastNodeValue != i)
            {
                ostringstream ss;

                ss << "Error in next_inorder_sucessor_tests:"
                    << " null returned for node=" << i << ".";

                StreamUtilities::throw_exception(ss);
            }
        }
        else
        {
            const NumberNext actual = sucessor->Value;
            if (expected != actual)
            {
                ostringstream ss;
                ss << "Error in next_inorder_sucessor_tests:"
                    << " expected (" << expected << ") != actual (" << actual << ")"
                    << " for node=" << i << ".";

                StreamUtilities::throw_exception(ss);
            }
        }
    }
}

void Privet::Algorithms::Trees::Tests::next_inorder_sucessor_tests()
{
    vector< TBinaryNode* > nodes;

    try
    {
        const int nodesCount = 25;
        NodeUtilities::CreateNodes(nodes, nodesCount);
        CreateTree(nodes);
        CheckSuccessors(nodes);
    }
    catch (const exception&)
    {
        VectorUtilities::FreeAndClear(nodes);

        throw;
    }
}