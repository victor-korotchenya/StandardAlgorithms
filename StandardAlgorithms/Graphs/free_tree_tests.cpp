#include"free_tree_tests.h"
#include"../Utilities/random.h"
#include"../Utilities/test_utilities.h"
#include"binary_tree_utilities.h"
#include<gsl/gsl>
#include<memory>
#include<optional>
#include<vector>

namespace
{
    using int_t = std::uint32_t;

    struct moda_t final
    {
        constexpr moda_t() = default;
        constexpr moda_t(const moda_t &) = default;
        constexpr auto operator= (const moda_t &) & -> moda_t & = default;
        constexpr moda_t(moda_t &&) noexcept = default;
        constexpr auto operator= (moda_t &&) &noexcept -> moda_t & = default;

        constexpr ~moda_t() noexcept
        {
            if (delete_counter != nullptr)
            {
                *delete_counter += value;
            }
        }

        // NOLINTNEXTLINE
        std::uint64_t *delete_counter{};
        // NOLINTNEXTLINE
        int_t value{};
        // NOLINTNEXTLINE
        moda_t *left{};
        // NOLINTNEXTLINE
        moda_t *right{};
    };

    [[nodiscard]] constexpr auto sum_nodes(const moda_t &root) noexcept(false) -> std::uint64_t
    {
        std::uint64_t result{};

        Standard::Algorithms::Trees::binary_tree_utilities<int_t, moda_t>::pre_order(&root,
            [&result](const moda_t *const node) -> void
            {
                assert(node != nullptr);
                result += node->value;
            });

        return result;
    }

    constexpr void delete_step(const std::string &name, const std::optional<std::uint64_t> expected_counter,
        gsl::owner<moda_t *> root, const std::uint64_t &delete_counter)
    {
        const auto good = expected_counter.has_value();

        // In a test, a memory leak is better than double deletion, however neither is expected here.
        Standard::Algorithms::throw_if_null("root node", root);

        if (good)
        {
            ::Standard::Algorithms::ert::are_equal(0U, delete_counter, "Delete counter before deletion. " + name);
        }

        auto *double_ptr = &root;
        Standard::Algorithms::Trees::free_tree_33<moda_t>(double_ptr);

        if (nullptr != *double_ptr)
        {
            auto str = ::Standard::Algorithms::Utilities::w_stream();
            str << "After deletion, the root pointer is not null. " << name;
            Standard::Algorithms::throw_exception(str);
        }

        if (good)
        {
            ::Standard::Algorithms::ert::are_equal(
                expected_counter.value(), delete_counter, "Delete counter after deletion. " + name);
        }
    }

    void one_node()
    {
        auto uniq = std::make_unique<moda_t>();
        auto *root = Standard::Algorithms::throw_if_null("root", uniq.get());

        std::uint64_t delete_counter{};
        root->delete_counter = &delete_counter;

        constexpr auto min_value = 3;
        constexpr auto max_value = 5003;
        const auto data = Standard::Algorithms::Utilities::random_t<std::uint32_t>(min_value, max_value)();
        root->value = data;

        const std::string name = "Single node";
        {
            const auto sum = sum_nodes(*root);
            ::Standard::Algorithms::ert::are_equal(data, sum, "Sum nodes for " + name);
        }

        const std::optional<std::uint64_t> expected_counter{ data };

        // NOLINTNEXTLINE
        gsl::owner<moda_t *> owner = uniq.release();

        delete_step(name, expected_counter, owner, delete_counter);
    }

    void many_nodes()
    {
        constexpr int_t min_size = 1;
        constexpr int_t max_size = 20;

        const auto size = Standard::Algorithms::require_positive(
            Standard::Algorithms::Utilities::random_t<std::uint32_t>(min_size, max_size)(), "node count");
        const std::string name = std::to_string(size) + " random nodes tree";

        gsl::owner<moda_t *> root{};
        std::uint64_t delete_counter{};
        std::optional<std::uint64_t> expected_counter = std::nullopt;
        std::string error = "Error creating a tree. ";

        try
        {
            constexpr auto is_unique_only = true;

            Standard::Algorithms::Trees::binary_tree_utilities<int_t, moda_t>::build_random_tree(size, &root, size << 4U,
                is_unique_only,
                [&delete_counter]() -> moda_t *
                {
                    auto uniq = std::make_unique<moda_t>();
                    {
                        auto *node = Standard::Algorithms::throw_if_null("new node", uniq.get());
                        node->delete_counter = &delete_counter;
                    }

                    return uniq.release();
                });

            Standard::Algorithms::throw_if_null("created tree root", root);

            const auto sum = sum_nodes(*root);
            expected_counter = sum;

            error.clear();
        }
        catch (const std::exception &exc)
        {
            try
            {
                error += exc.what();
            }
            catch (...)
            {// Already got some error text.
            }
        }
        catch (...)
        {// A tree must be freed - do nothing here.
        }

        if (!error.empty() && root == nullptr)
        {
            throw std::runtime_error(error);
        }

        delete_step(name, expected_counter, root, delete_counter);

        if (!error.empty())
        {
            throw std::runtime_error(error);
        }
    }
} // namespace

void Standard::Algorithms::Trees::Tests::free_tree_tests()
{
    one_node();
    many_nodes();
}
