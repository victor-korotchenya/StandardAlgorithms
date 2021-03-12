#include "IteratorUtilitiesTests.h"
using namespace Privet::Algorithms;

IteratorUtilitiesTests::SomeStruct::SomeStruct(SomeStruct::IdType id)
    : Id(id)
{
}

IteratorUtilitiesTests::SomeStruct::IdType IteratorUtilitiesTests::GetValue(
    const SomeStruct& instance)
{
    return instance.Id;
}

void IteratorUtilitiesTests::Test()
{
    TestInner< short >("Short");

    TestInner<
        SomeStruct,
        SomeStruct::IdType,
        SomeStruct::IdType(*)(const SomeStruct&)
    >
        ("SomeStruct", GetValue);
}