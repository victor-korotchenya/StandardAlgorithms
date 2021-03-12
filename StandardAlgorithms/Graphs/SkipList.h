#pragma once
#include "../Utilities/RandomGenerator.h"
#include "../disable_constructors.h"
#include "../Utilities/StreamUtilities.h"

namespace Privet::Algorithms::Random
{
    //SkipList is similar to BST, but uses randomization.
    template <typename TKey, typename TValue, int maxLevels = 10 >
    class SkipList final
    {
        enum class ValueKind final : char
        {
            Min = 0, Max = 1, Usual = 2
        };

        struct TNode final
        {
            TKey Key;
            TValue Value;

            TNode* Nexts[maxLevels];
        };

        const double Probability;

        size_t Count;

        //Can go from 0 up to (maxLevels - 1) inclusively.
        int MaxCurrentLevel;

        DoubleRandom _DoubleRandom;

        //Minus infinity.
        TNode Head;

        //Plus infinity.
        //It allows to avoid extra comparisons with null.
        TNode Tail;

        //Min and max key insertion require special handling - for now.
        bool HasMinMaxKey[2];

#ifdef _DEBUG
        TKey _minKey, _maxKey;
#endif
        DISABLE_ALL_CONSTRUCTORS(SkipList);

    public:

        using VisitFunction = bool(*)(const TKey& key, const TValue& value);

        SkipList(
            const TKey& minKey, const TKey& maxKey, double probability = 0.5);

        ~SkipList();

        size_t length() const noexcept
        {
            return Count;
        }

        bool empty() const noexcept
        {
            const bool result = 0 == Count;
            return result;
        }

        //TODO: p3. use shared_ptr<> to make it safe!
        void Add(const TKey& key, const TValue& value);
        TValue* Find(const TKey& key) const;
        bool Delete(const TKey& key);

        void Clear();

        void Visit(VisitFunction visitFunction) const;

    private:

        void AddUsual(const TKey& key, const TValue& value);
        TValue* FindUsual(const TKey& key) const;
        bool DeleteUsual(const TKey& key);
        void FixUselessLevels();

        void AddSpecial(const TValue& value, const ValueKind valueKind);
        TValue* FindSpecial(const ValueKind valueKind) const;
        bool DeleteSpecial(const ValueKind valueKind);

        TNode* FindClosest(const TKey& key, TNode* nexts[maxLevels]) const;

        int RandomLevel();

        void ResetHeadTail();

        ValueKind GetValueKind(const TKey& key) const;

        void static ThrowAddKey(const TKey& key, const TValue& oldValue, const TValue& newValue);

#ifdef _DEBUG
        void VerifyOrder(const char* const message) const;
        size_t VerifyLowestLevel(const char* const message) const;
#endif
    };

    template <typename TKey, typename TValue, int maxLevels>
    Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        SkipList(const TKey& minKey, const TKey& maxKey, double probability)
        :
        Probability(probability),
        Count(0), MaxCurrentLevel(0),
        _DoubleRandom(0.0, 1.0)
#ifdef _DEBUG
        , _minKey(minKey), _maxKey(maxKey)
#endif
    {
#pragma warning( push )
#pragma warning( disable : 4127)
        //TODO: p2. do it statically.
        {
            const int minLevel = 2;
            const int maxLevel = 1024 * 1024;
            if (!(minLevel <= maxLevels && maxLevels <= maxLevel))
            {
                std::ostringstream ss;
                ss << "The maxLevels (" << maxLevels
                    << ") must be in the range [" << minLevel << ", "
                    << maxLevel << "].";
                StreamUtilities::throw_exception(ss);
            }
        }
#pragma warning( pop )

        const bool isOk = 0.0 < probability && probability < 1.0;
        if (!isOk)
        {
            std::ostringstream ss;
            ss << "The probability (" << probability <<
                ") must be in range (0, 1).";
            StreamUtilities::throw_exception(ss);
        }

        HasMinMaxKey[0] = HasMinMaxKey[1] = false;

        Head.Key = minKey;
        Tail.Key = maxKey;

        ResetHeadTail();
    }

    template <typename TKey, typename TValue, int maxLevels >
    Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        ~SkipList()
    {
        Clear();
    }

    template <typename TKey, typename TValue, int maxLevels >
    void Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        Add(const TKey& key, const TValue& value)
    {
#ifdef _DEBUG
        VerifyOrder("Before add.");
#endif
        const ValueKind valueKind = GetValueKind(key);
        if (ValueKind::Usual == valueKind)
        {
            AddUsual(key, value);
        }
        else
        {
            AddSpecial(value, valueKind);
        }

#ifdef _DEBUG
        VerifyOrder("After add.");
#endif
    }

    template <typename TKey, typename TValue, int maxLevels >
    TValue* Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        Find(const TKey& key) const
    {
#ifdef _DEBUG
        VerifyOrder("Before find.");
#endif
        const ValueKind valueKind = GetValueKind(key);

        auto result = ValueKind::Usual == valueKind
            ? FindUsual(key)
            : FindSpecial(valueKind);

        return result;
    }

    template <typename TKey, typename TValue, int maxLevels >
    bool Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        Delete(const TKey& key)
    {
#ifdef _DEBUG
        VerifyOrder("Before delete.");
#endif
        const ValueKind valueKind = GetValueKind(key);

        auto result = ValueKind::Usual == valueKind
            ? DeleteUsual(key)
            : DeleteSpecial(valueKind);

#ifdef _DEBUG
        VerifyOrder("After delete.");
#endif
        return result;
    }

    template <typename TKey, typename TValue, int maxLevels >
    void Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        AddUsual(const TKey& key, const TValue& value)
    {
        TNode* nexts[maxLevels];
        TNode* current = FindClosest(key, nexts);

        if (key == current->Key)
        {
            if (value == current->Value)
            {
                return;
            }

            ThrowAddKey(key, current->Value, value);
            return;
        }

        const int newlevel = RandomLevel();
        if (newlevel > MaxCurrentLevel)
        {
            for (int level = MaxCurrentLevel + 1; level <= newlevel; ++level)
            {
                nexts[level] = &Head;
            }

            MaxCurrentLevel = newlevel;
        }

        TNode* newNode = new TNode;
        newNode->Key = key;
        newNode->Value = value;

        for (int level = 0; level <= MaxCurrentLevel; ++level)
        {
            newNode->Nexts[level] = nexts[level]->Nexts[level];
            nexts[level]->Nexts[level] = newNode;
        }

        ++Count;
    }

    template <typename TKey, typename TValue, int maxLevels >
    TValue* Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        FindUsual(const TKey& key) const
    {
        TNode* current = FindClosest(key, nullptr);

        auto result = key == current->Key ? &(current->Value) : nullptr;
        return result;
    }

    template <typename TKey, typename TValue, int maxLevels >
    bool Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        DeleteUsual(const TKey& key)
    {
        TNode* nexts[maxLevels];
        TNode* current = FindClosest(key, nexts);

        const bool result = key == current->Key;
        if (result)
        {
            for (int level = 0; level <= MaxCurrentLevel; ++level)
            {
                if (current != nexts[level]->Nexts[level])
                {
                    break;
                }

                nexts[level]->Nexts[level] = current->Nexts[level];
            }

            delete current;

            --Count;

            FixUselessLevels();
        }

        return result;
    }

    template <typename TKey, typename TValue, int maxLevels>
    void SkipList<TKey, TValue, maxLevels>::FixUselessLevels()
    {
        while (MaxCurrentLevel > 0 && (&Tail) == Head.Nexts[MaxCurrentLevel])
        {
            --MaxCurrentLevel;
        }
    }

    template <typename TKey, typename TValue, int maxLevels >
    void Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        AddSpecial(const TValue& value, const ValueKind valueKind)
    {
        const char index = static_cast<char>(valueKind);
        TNode& ref1 = ValueKind::Min == valueKind ? Head : Tail;

        if (HasMinMaxKey[index])
        {
            if (value == ref1.Value)
            {
                return;
            }

            ThrowAddKey(ref1.Key, ref1.Value, value);
            return;
        }

        ref1.Value = value;
        HasMinMaxKey[index] = true;
        ++Count;
    }

    template <typename TKey, typename TValue, int maxLevels >
    TValue* Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        FindSpecial(const ValueKind valueKind) const
    {
        const char index = static_cast<char>(valueKind);
        const TNode& ref1 = ValueKind::Min == valueKind ? Head : Tail;

        if (HasMinMaxKey[index])
        {
            return const_cast<TValue*>(&(ref1.Value));
        }

        return nullptr;
    }

    template <typename TKey, typename TValue, int maxLevels >
    bool Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        DeleteSpecial(const ValueKind valueKind)
    {
        const char index = static_cast<char>(valueKind);
        if (HasMinMaxKey[index])
        {
            HasMinMaxKey[index] = false;
            --Count;

            return true;
        }

        return false;
    }

    template <typename TKey, typename TValue, int maxLevels >
    typename SkipList<TKey, TValue, maxLevels>::TNode*
        Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        FindClosest(const TKey& key, TNode* nexts[maxLevels]) const
    {
        TNode* result = const_cast<TNode*>(&Head);

        for (int level = MaxCurrentLevel; 0 <= level; --level)
        {
            while (result->Nexts[level]->Key < key)
            {
                result = result->Nexts[level];
            }

            if (nexts)
            {
                nexts[level] = result;
            }
        }

        result = result->Nexts[0];

        return result;
    }

    template <typename TKey, typename TValue, int maxLevels >
    int Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        RandomLevel()
    {
        int level = 1;

        while (level < maxLevels
            //It does not make sense to go too far.
            && level <= (MaxCurrentLevel + 1)
            && _DoubleRandom() < Probability)
        {
            level++;
        }

        --level;
        return level;
    }

    template <typename TKey, typename TValue, int maxLevels >
    void Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        Visit(VisitFunction visitFunction) const
    {
        if (nullptr == visitFunction)
        {
            throw std::runtime_error("The 'visitFunction' must be not null.");
        }

        bool shallContinue = true;
        if (HasMinMaxKey[0])
        {
            shallContinue = visitFunction(Head.Key, Head.Value);
        }

        TNode* current = Head.Nexts[0];

        while (shallContinue && (&Tail) != current)
        {
            shallContinue = visitFunction(current->Key, current->Value);
            current = current->Nexts[0];
        }

        if (shallContinue && HasMinMaxKey[1])
        {
            visitFunction(Tail.Key, Tail.Value);
        }
    }

    template <typename TKey, typename TValue, int maxLevels >
    void Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        Clear()
    {
        HasMinMaxKey[0] = HasMinMaxKey[1] = false;

        TNode* current = Head.Nexts[0];
        while (&Tail != current)
        {
            TNode* tempNode = current;
            current = current->Nexts[0];
            delete tempNode;
        }

        Count = 0;
        MaxCurrentLevel = 0;

        ResetHeadTail();
    }

    template <typename TKey, typename TValue, int maxLevels >
    void Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        ResetHeadTail()
    {
        for (int level = 0; level < maxLevels; ++level)
        {
            Head.Nexts[level] = &Tail;

            //Must never get there.
            Tail.Nexts[level] = nullptr;
        }
    }

    template <typename TKey, typename TValue, int maxLevels >
    typename Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::ValueKind
        Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        GetValueKind(const TKey& key) const
    {
        if (key == Head.Key)
        {
            return ValueKind::Min;
        }

        if (key == Tail.Key)
        {
            return ValueKind::Max;
        }

        return ValueKind::Usual;
    }

    template <typename TKey, typename TValue, int maxLevels >
    void Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        ThrowAddKey(const TKey& key, const TValue& oldValue, const TValue& newValue)
    {
        std::ostringstream ss;
        ss << "The key (" << key
            << ") has different value (" << oldValue
            << ") when inserting new value(" << newValue << ").";
        StreamUtilities::throw_exception(ss);
    }

#ifdef _DEBUG
    template <typename TKey, typename TValue, int maxLevels >
    void Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        VerifyOrder(const char* const message) const
    {
        if (_minKey != Head.Key)
        {
            std::ostringstream ss;
            ss << "Error: (" << _minKey <<
                ") _minKey != Head.Key(" << Head.Key << ").";

            if (message)
            {
                ss << " " << message;
            }

            StreamUtilities::throw_exception(ss);
        }

        if (_maxKey != Tail.Key)
        {
            std::ostringstream ss;
            ss << "Error: (" << _maxKey <<
                ") _maxKey != Tail.Key(" << Tail.Key << ").";

            if (message)
            {
                ss << " " << message;
            }

            StreamUtilities::throw_exception(ss);
        }

        size_t index = VerifyLowestLevel(message);

        if (HasMinMaxKey[0])
        {
            ++index;
        }
        if (HasMinMaxKey[1])
        {
            ++index;
        }

        if (Count != index)
        {
            std::ostringstream ss;
            ss << "Error: (" << Count
                << ") Count != index(" << index << ").";

            if (message)
            {
                ss << " " << message;
            }

            StreamUtilities::throw_exception(ss);
        }
    }

    template <typename TKey, typename TValue, int maxLevels >
    size_t Privet::Algorithms::Random::SkipList< TKey, TValue, maxLevels >::
        VerifyLowestLevel(const char* const message) const
    {
        size_t result = 0;

        TKey previousKey = Head.Key;
        TNode* current = Head.Nexts[0];

        while (&Tail != current)
        {
            if (current->Key <= previousKey)
            {
                std::ostringstream ss;
                ss << "Error: current->Key (" << current->Key
                    << ") <= previousKey (" << previousKey
                    << ") at index=" << result << ".";

                if (message)
                {
                    ss << " " << message;
                }

                StreamUtilities::throw_exception(ss);
            }

            previousKey = current->Key;
            ++result;

            current = current->Nexts[0];
        }

        return result;
    }
#endif
}