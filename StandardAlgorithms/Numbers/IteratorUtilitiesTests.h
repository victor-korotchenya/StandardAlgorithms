#pragma once
#include <vector>
#include "../Utilities/VectorUtilities.h"
#include "../Utilities/IteratorUtilities.h"
#include "../Assert.h"

// todo: p1. Remove 'test'/'tests' folders.
namespace Privet::Algorithms
{
    class IteratorUtilitiesTests final
    {
        IteratorUtilitiesTests() = delete;

    public:
        static void Test();

    private:

        template <typename FullType,
            typename Type = FullType,
            typename GetValueFunc = const Type& (*)(const FullType&)
        >
            static void TestInner(
                const std::string& name,
                GetValueFunc getValueFunc = [](const FullType& a) -> const Type&
                {
                    return a;
                });

        struct SomeStruct final
        {
            using IdType = unsigned int;

            IdType Id;

            explicit SomeStruct(IdType id = IdType(0));

            //Hash.
            inline size_t operator()(const SomeStruct& b) const
            {
                return static_cast<size_t>(b.Id);
            }
        };

        static SomeStruct::IdType GetValue(const SomeStruct& instance);
    };

    template <typename FullType,
        typename Type,
        typename GetValueFunc >
        void IteratorUtilitiesTests::TestInner(
            const std::string& name,
            GetValueFunc getValueFunc)
    {
        const size_t size = 10;

        std::vector< FullType > data;
        Privet::Algorithms::VectorUtilities::Fill< FullType >(size, data);

        const bool throwOnException = false;

        size_t firstRepeatIndex = size << 1;
        {
            using Iterator = typename std::vector< FullType >::const_iterator;

            bool (*alg)
                (const Iterator, const Iterator, size_t&,
                    const std::string&, const bool, GetValueFunc)
                = IteratorUtilities< Type, Iterator, GetValueFunc >::
                FindFirstRepetition;

            const bool actual = alg(
                data.cbegin(), data.cend(), firstRepeatIndex,
                name, throwOnException, getValueFunc);

            Assert::AreEqual(false, actual, name + " none");
        }

        const size_t itemIndex = 3;
        data.push_back(data[itemIndex]);
        const size_t expectedIndex = data.size() - 1;

        Privet::Algorithms::VectorUtilities::Fill< FullType >(7, data, 305);
        {
            using Iterator = const FullType*;
            Iterator dataPtr = data.data();

            bool (*alg)
                (const Iterator, const Iterator, size_t&,
                    const std::string&, const bool, GetValueFunc)
                = IteratorUtilities< Type, Iterator, GetValueFunc >::FindFirstRepetition;

            const bool actual = alg(
                dataPtr, dataPtr + data.size(), firstRepeatIndex,
                name, throwOnException, getValueFunc);

            Assert::AreEqual(true, actual, name + " repeat result");
            Assert::AreEqual(expectedIndex, firstRepeatIndex, name + " repeat index");
        }
    }
}