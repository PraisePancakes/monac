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
    using iterator = T::iterator;
    using const_iterator = T::const_iterator;
    using reverse_iterator = T::reverse_iterator;
    using const_reverse_iterator = T::const_reverse_iterator;
    using storage_view = std::span<T>;
    storage_view m_span;
    size_type m_start{0};
    size_type m_end{0};

   public:
    worm_machine() = delete;
    worm_machine(const storage_view& view) : m_span{view} {};
    worm_machine(const worm_machine&) = default;
    worm_machine(worm_machine&&) = default;
    void operator=(const worm_machine&) = default;
    void operator=(worm_machine&&) = default;

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

    iterator begin() {
        return m_span.begin() + m_start;
    };

    iterator end() {
        return m_span.begin() + m_end;
    };

    const_iterator begin() const {
        return this->begin();
    };

    const_iterator end() const {
        return this->end();
    };

    value_type advance() noexcept {
        if (is_end()) {
            return peek_prev();
        }
        return m_span[m_end++];
    };

    void finish_unit() noexcept {
        this->m_start = m_end;
    };
    virtual ~worm_machine() = default;
};

};  // namespace monac::semantic

#endif