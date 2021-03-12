#include <string>
#include <vector>
#include "ConvertUtilities.h"

using namespace std;

void Privet::Algorithms::ConvertUtilities::StringToBooleanVector(
    const string& data,
    vector< bool >& result)
{
    const size_t size = data.size();

    result.clear();
    result.resize(size, false);

    for (size_t i = 0; i < size; ++i)
    {
        const char& c = data[i];
        result[i] = '0' != c;
    }
}