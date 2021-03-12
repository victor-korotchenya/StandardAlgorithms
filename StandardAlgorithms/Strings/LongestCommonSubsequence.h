#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace
{
    template<typename TContainer, typename TSubsequence>
    size_t CalculateMatix(
        const TContainer& a,
        const TContainer& b,
        std::vector<std::vector<size_t>>& tempArray)
    {
        size_t result = 0;
        const auto a_size = a.size(), b_size = b.size();
        for (size_t i = 1; i <= a_size; ++i)
        {
            for (size_t j = 1; j <= b_size; ++j)
            {
                tempArray[i][j] = a[i - 1] == b[j - 1]
                    ? tempArray[i - 1][j - 1] + 1
                    : std::max(tempArray[i - 1][j], tempArray[i][j - 1]);

                result = std::max(result, tempArray[i][j]);
            }
        }

        return result;
    }

    template<typename TContainer, typename TSubsequence>
    void BacktrackResult(
        const TContainer& a,
        const TContainer& b,
        const std::vector<std::vector<size_t>>& tempArray,
        TSubsequence& subsequence)
    {
        auto x = a.size(), y = b.size();
        auto lengthLeft = tempArray[x][y];
        if (0 == lengthLeft)
        {
            return;
        }

        subsequence.resize(lengthLeft);
        do
        {
            while (lengthLeft == tempArray[x - 1][y - 1])
            {//Go by diagonal until a smaller length is encountered.
                --x; --y;
            }

            while (lengthLeft == tempArray[x][y - 1])
            {//Go left.
                --y;
            }

            while (lengthLeft == tempArray[x - 1][y])
            {//Go up.
                --x;
            }

#ifdef _DEBUG
            if (x <= 0 || y <= 0 || a[x - 1] != b[y - 1])
            {
                throw std::runtime_error("a[x - 1] != b[y - 1] in BacktrackResult.");
            }
#endif

            subsequence[--lengthLeft] = a[x - 1];
        } while (lengthLeft);
    }
}

namespace Privet
{
    namespace Algorithms
    {
        namespace Strings
        {
            //This is not the fastest way.
            template<typename TContainer, typename TSubsequence = TContainer>
            size_t LongestCommonSubsequence(
                const TContainer& a, const TContainer& b,
                TSubsequence& subsequence)
            {
                subsequence.clear();

                const auto a_size = a.size(), b_size = b.size();
                if (0 == a_size || 0 == b_size)
                {
                    return 0;
                }

                //TODO: p3. can be implemented using O(n) size - use crossing idea.
                std::vector<std::vector<size_t>> tempArray(a_size + 1,
                    std::vector<size_t>(b_size + 1, 0));

                const auto result = CalculateMatix<TContainer, TSubsequence>(a, b, tempArray);
                BacktrackResult<TContainer, TSubsequence>(a, b, tempArray, subsequence);
                return result;
            }
        }
    }
}