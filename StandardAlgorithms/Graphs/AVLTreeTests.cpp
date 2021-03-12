#include <exception>
#include <vector>
#include <memory> // unique_ptr
#include "../Utilities/Random.h"
#include "../Utilities/VectorUtilities.h"
#include "BinaryTreeUtilities.h"
#include "AVLTreeTests.h"
#include "AVLTree.h"
#include "..\Assert.h"

using namespace std;
using namespace Privet::Algorithms::Trees;
using namespace Privet::Algorithms;

namespace
{
    typedef long int Number;
    typedef AVLTreeNode< Number > Node;
    typedef AVLTree< Number > Tree;

    void Add(const Number number, const bool expected, Tree& root)
    {
        const auto sizeBefore = root.size();
        const bool actual = root.Add(number);
        Assert::AreEqual(expected, actual, "Add result.");

        const auto sizeAfter = root.size();
        Assert::AreEqual(sizeBefore + (expected ? 1 : 0),
            sizeAfter, "size after add.");
    }

    void Delete(const Number number, const bool expected, Tree& root)
    {
        const auto sizeBefore = root.size();
        const bool actual = root.Delete(number);
        const auto sizeAfter = root.size();

        Assert::AreEqual(expected, actual, "Delete result");
        Assert::AreEqual(expected ? sizeBefore - 1 : sizeBefore,
            sizeAfter, "size after delete");
    }

    void Find(const string& name, const Number number, const bool expected, const Tree& root)
    {
        const bool actual = root.Find(number);
        Assert::AreEqual(expected, actual, name);
    }
}

void Privet::Algorithms::Trees::Tests::AVLTreeTests()
{
    const size_t size = 20;
    vector< Number > values;

    const bool isUniqueOnly = true;
    FillRandom(values, size + 1, Number(0), isUniqueOnly);
    Assert::AreEqual(size + 1, values.size(), "values.size");

    unique_ptr< Tree > uniqueTree(new Tree());
    if (!uniqueTree)
    {
        throw exception("Cannot create tree.");
    }

    Tree* pTree = uniqueTree.get();
    if (!pTree)
    {
        throw exception("The created tree is null.");
    }

    Tree& root = *pTree;

    for (size_t i = 0; i < size; ++i)
    {
        const Number& value = values[i];

        Find("Find before add", value, false, root);

        Add(value, true, root);
        Find("Find after add", value, true, root);

        Add(value, false, root);
        Find("Find after second add", value, true, root);
    }
    Assert::AreEqual(size, root.size(), "root.size after adding");

    vector< size_t > indexes;
    VectorUtilities::Fill(size, indexes);
    ShuffleArray(indexes);
    //Assert::AreEqual(size, indexes.size(), "indexes.size");
    //
    //for (size_t i = 0; i < size; ++i)
    //{
    //  const Number& value = values[indexes[i]];
    //
    //  Find("Find before delete", value, true, root);
    //
    //  Delete(value, true, root);
    //  Find("Find after delete", value, false, root);
    //
    //  Delete(value, false, root);
    //  Find("Find after second delete", value, false, root);
    //}
    //Assert::AreEqual(size_t(0), root.size(), "root.size after deleting");

    root.clear();
    Assert::AreEqual(size_t(0), root.size(), "root.size after clearing");
}