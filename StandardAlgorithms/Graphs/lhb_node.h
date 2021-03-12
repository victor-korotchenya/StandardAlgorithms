#pragma once
#include<algorithm>
#include<cassert>
#include<cstdint>
#include<limits>

namespace Standard::Algorithms::Trees
{
    //    todo(p2): del copy-paste in class_member_field.
    //    template<bool cond, class t> struct class_member_field { t v; };
    //    template<class t> struct class_member_field<false, t> {};
    //
    //    template<class key_t, bool has_parent = false>
    //    struct poor_sample
    //    {
    //        class_member_field<has_parent, lhb_node*> parent;
    //    };

    template<class key_t1, bool has_parent1 = false>
    struct lhb_node final
    {
        static constexpr bool has_parent = has_parent1;
        using key_t = key_t1;

#if _DEBUG
        std::int32_t height = 1;
        key_t key{};
        lhb_node *left{};
        lhb_node *right{};
#else
        lhb_node *left{};
        lhb_node *right{};
        key_t key{};
        std::int32_t height = 1;
#endif
    };

    template<class key_t1>
    struct lhb_node<key_t1, true> final
    {
        static constexpr bool has_parent = true;
        using key_t = key_t1;

#if _DEBUG
        std::int32_t height = 1;
        key_t key{};
        lhb_node *parent{};
        lhb_node *left{};
        lhb_node *right{};
#else
        lhb_node *parent{};
        lhb_node *left{};
        lhb_node *right{};
        key_t key{};
        std::int32_t height = 1;
#endif
    };
} // namespace Standard::Algorithms::Trees
