#ifndef FSM_HPP
#define FSM_HPP
#include <iostream>
#include <span>
#include <type_traits>
namespace monac::semantic {
template <std::ranges::random_access_range T_>
class worm_machine {
    using value_type = T_::value_type;
    using size_type = T_::size_type;
    using const_pointer = T_::const_pointer;
    using storage_view = std::span<const value_type>;
    using iterator = storage_view::iterator;

    storage_view _m_span;
    size_type _m_start{0};
    size_type _m_end{0};

   public:
    worm_machine() = delete;
    worm_machine(const T_& sequence_to_view) : _m_span{sequence_to_view} {};
    worm_machine(const worm_machine&) = default;
    worm_machine(worm_machine&&) = default;
    worm_machine& operator=(const worm_machine&) = default;
    worm_machine& operator=(worm_machine&&) = default;

    [[nodiscard]] bool is_end() const noexcept {
        return _m_end >= _m_span.size();
    };

    [[nodiscard]] value_type peek() const noexcept {
        return _m_span[_m_end];
    };

    [[nodiscard]] value_type peek_prev() const noexcept {
        return _m_span[_m_end - 1];
    }

    [[nodiscard]] value_type peek_next() const noexcept {
        return _m_span[_m_end + 1];
    }

    [[nodiscard]] iterator begin() noexcept {
        return _m_span.begin() + _m_start;
    };

    [[nodiscard]] iterator end() noexcept {
        return _m_span.begin() + _m_end;
    };

    value_type advance() noexcept {
        return _m_span[_m_end++];
    };

    void finish_unit() noexcept {
        this->_m_start = _m_end;
    };

    virtual ~worm_machine() = default;
};

};  // namespace monac::semantic

#endif