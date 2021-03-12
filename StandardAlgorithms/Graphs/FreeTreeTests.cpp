#include <exception>
#include "BinaryTreeUtilities.h"
#include "FreeTreeTests.h"
#include "../Assert.h"
#include "../Utilities/ExceptionUtilities.h"
#include "../Utilities/Random.h"
#include "../Utilities/StreamUtilities.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

namespace
{
    using Number = unsigned;

    struct Node final
    {
        size_t* DeleteCounter;

        Number Value;

        Node* Left, * Right;

        explicit Node(size_t* deleteCounter = nullptr,
            const Number& value = Number(0),
            Node* left = nullptr,
            Node* right = nullptr)
            :
            DeleteCounter(deleteCounter), Value(value), Left(left), Right(right)
        {
        }

        ~Node()
        {
            if (DeleteCounter)
            {
                *DeleteCounter += Value;
            }
        }
    };

    size_t SumNodes(const Node* const root)
    {
        size_t result = 0;

        BinaryTreeUtilities<Number, Node>::PreOrder(
            root,
            [&](const Node* const node) -> void
            {
                ThrowIfNull(node, "node");

                result += size_t(node->Value);
            });

        return result;
    }

    void DeleteStep(
        const string& name,
        const size_t expectedCounter,
        const size_t* const deleteCounter,
        Node** pRoot)
    {
        ThrowIfNull(deleteCounter, "deleteCounter pointer." + name);
        ThrowIfNull(pRoot, "pRoot." + name);
        ThrowIfNull(*pRoot, "(*(pRoot))." + name);

        Assert::AreEqual(size_t(0), *deleteCounter,
            "deleteCounter before delete. " + name);

        FreeTree<Node>(pRoot);
        Assert::AreEqual(expectedCounter, *deleteCounter, "deleteCounter after delete. " + name);

        if (nullptr != *pRoot)
        {
            ostringstream ss;
            ss << "After delete, the root is not null. " << name;
            StreamUtilities::throw_exception(ss);
        }
    }

    void OneNode()
    {
        const Number data = UnsignedIntRandom(3, 5003)();

        size_t deleteCounter = 0;

        auto root = new Node(&deleteCounter);
        root->Value = data;

        const string name = "Single node";

        const auto sum = SumNodes(root);
        Assert::AreEqual(size_t(data), sum, "SumNodes for " + name);

        DeleteStep(name, data, &deleteCounter, &root);
    }

    void ManyNodes()
    {
        const Number minSize = 1;
        const Number maxSize = 20;
        const Number size = UnsignedIntRandom(minSize, maxSize)();

        Node* root = nullptr;
        size_t deleteCounter = 0;

        const bool isUniqueOnly = true;

        BinaryTreeUtilities<Number, Node>::BuildRandomTree(
            size, &root, size << 4, isUniqueOnly,
            [&]() -> Node*

            {
                Node* result = new Node(&deleteCounter);
                return result;
            });

        const size_t sum = SumNodes(root);

        const string name = to_string(size) + " random nodes tree";
        DeleteStep(name, sum, &deleteCounter, &root);
    }
}

void Privet::Algorithms::Trees::Tests::FreeTreeTests()
{
    OneNode();
    ManyNodes();
}