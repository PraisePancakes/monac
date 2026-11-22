#ifndef FSM_HPP
#define FSM_HPP
#include <iostream>
#include <span>
#include <type_traits>
namespace monac::semantic {
template <std::ranges::random_access_range T>
class worm_machine {
    using value_type = T::value_type;
    using size_type = T::size_type;
    using difference_type = T::difference_type;
    using reference = T::reference;
    using const_reference = T::const_reference;
    using pointer = T::pointer;
    using const_pointer = T::const_pointer;

    using storage_view = std::span<const value_type>;
    using iterator = storage_view::iterator;
    using const_iterator = storage_view::const_iterator;
    using reverse_iterator = storage_view::reverse_iterator;
    using const_reverse_iterator = storage_view::const_reverse_iterator;
    storage_view m_span;
    size_type m_start{0};
    size_type m_end{0};

   public:
    worm_machine() = delete;
    worm_machine(const T& sequence_to_view) : m_span{sequence_to_view} {};
    worm_machine(const worm_machine&) = default;
    worm_machine(worm_machine&&) = default;
    worm_machine& operator=(const worm_machine&) = default;
    worm_machine& operator=(worm_machine&&) = default;

    [[nodiscard]] bool is_end() const noexcept {
        return m_end >= m_span.size();
    };

    [[nodiscard]] value_type peek() const noexcept {
        return m_span[m_end];
    };

    [[nodiscard]] value_type peek_prev() const noexcept {
        return m_span[m_end - 1];
    }

    [[nodiscard]] value_type peek_next() const noexcept {
        return m_span[m_end + 1];
    }

    [[nodiscard]] iterator begin() noexcept {
        return m_span.begin() + m_start;
    };

    [[nodiscard]] iterator end() noexcept {
        return m_span.begin() + m_end;
    };

    [[nodiscard]] const_iterator begin() const noexcept {
        return this->begin();
    };

    [[nodiscard]] const_iterator end() const noexcept {
        return this->end();
    };

    value_type advance() noexcept {
        return m_span[m_end++];
    };

    void finish_unit() noexcept {
        this->m_start = m_end;
    };

    virtual ~worm_machine() = default;
};

};  // namespace monac::semantic

#endif