#pragma once
#include <stack>
#include <stdexcept>
#include <tuple>
#include "BinaryTreeUtilities.h"

namespace
{
    template <typename note_t>
    note_t* find_first_fork(note_t* node)
    {
        for (;;)
        {
            if (node->Left)
            {
                if (node->Right)
                {
                    return node;
                }

                node = node->Left;
            }
            else if (node->Right)
            {
                node = node->Right;
            }
            else
            {//There is no or just one value.
                return nullptr;
            }
        }
    }

    template<typename value_t>
    value_t shift_add_bit(value_t sum, value_t bit)
    {
        const auto result = static_cast<value_t>((sum << 1) | bit);
        return result;
    }
}

namespace Privet::Algorithms::Trees
{
    //Find maximum XOR for 2 elements.
    template <typename value_t>
    class BinaryTrie final
    {
        using bit_t = unsigned char;

        struct note_t final
        {
            bit_t Value;
            note_t* Left, * Right;

            explicit note_t(bit_t value = {},
                note_t* left = {}, note_t* right = {})
                : Value(value), Left(left), Right(right)
            {
            }
        };

        using tuple_t = std::tuple<note_t*, note_t*, value_t, value_t>;

        note_t* _Root;

        BinaryTrie(const BinaryTrie&) = delete;
        BinaryTrie& operator = (const BinaryTrie&) = delete;
        BinaryTrie(BinaryTrie&&) = delete;
        BinaryTrie& operator =(BinaryTrie&&) = delete;

    public:
        BinaryTrie()
            : _Root(new note_t())
        {
        }

        ~BinaryTrie()
        {
            FreeTree<note_t>(_Root);
        }

        //Add a value.
        void add(value_t value)
        {
            constexpr auto bits = static_cast<unsigned>(sizeof(value_t) << 3);

            auto node = _Root;
            auto i = bits - 1;
            do
            {
                const auto b = static_cast<bit_t>((value >> i) & 1);

                auto ptr = 0 == b ? &(node->Left) : &(node->Right);
                if (nullptr == *ptr)
                    *ptr = new note_t(b);

                node = *ptr;
            } while (i--);
        }

        //Maximum XOR of 2 values, not necessarily distinct.
        //Return 0 when no values added.
        value_t max_xor() const
        {
            auto node = find_first_fork(_Root);
            if (!node)
                return{};

            value_t result{};

            std::stack<tuple_t> st;
            push_tuple(st, node->Left, node->Right, 0, 0);

            do
            {
                const auto& top = st.top();
                note_t* const left = std::get<0>(top);
                note_t* const right = std::get<1>(top);
                const auto sum_left = std::get<2>(top);
                const auto sum_right = std::get<3>(top);
                st.pop();
#ifdef _DEBUG
                const auto is_leaf_left = nullptr == left->Left && nullptr == left->Right;
                const auto is_leaf_right = nullptr == right->Left && nullptr == right->Right;
                if (is_leaf_left != is_leaf_right)
                {
                    throw std::runtime_error(
                        "Inner error: Left node has no sub-nodes, but the right one has, or vice versa.");
                }
#endif
                if (left->Left || left->Right)
                {
                    process_intermediate_nodes(st, left, right, sum_left, sum_right);
                }
                else
                {
                    const auto temp = static_cast<value_t>(sum_left ^ sum_right);
                    if (result < temp)
                    {
                        result = temp;
                    }
                }
            } while (!st.empty());

            return result;
        }

    private:
        static void process_intermediate_nodes(
            std::stack<tuple_t>& st,
            note_t* left, note_t* right,
            const value_t sum_left, const value_t sum_right)
        {
            if (nullptr == left->Left)
            {//Left has only Right
                push_tuple(st, left->Right,
                    nullptr == right->Left ? right->Right : right->Left,
                    sum_left, sum_right);
            }
            else if (nullptr == left->Right)
            {//Left has only Left.
                push_tuple(st, left->Left,
                    nullptr == right->Right ? right->Left : right->Right,
                    sum_left, sum_right);
            }
            //Left has both.
            else if (nullptr == right->Left)
            {//Right has only Right.
                push_tuple(st, left->Left, right->Right, sum_left, sum_right);
            }
            else if (nullptr == right->Right)
            {//Right has only Left.
                push_tuple(st, left->Right, right->Left, sum_left, sum_right);
            }
            else
            {//Both exist.
                push_tuple(st, left->Right, right->Left, sum_left, sum_right);
                push_tuple(st, left->Left, right->Right, sum_left, sum_right);
            }
        }

        static void push_tuple(
            std::stack<tuple_t>& st,
            note_t* left, note_t* right,
            value_t sum_left, value_t sum_right)
        {
            st.push(
                tuple_t(left, right,
                    shift_add_bit<value_t>(sum_left, left->Value),
                    shift_add_bit<value_t>(sum_right, right->Value)));
        }
    };
}