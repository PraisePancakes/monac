#ifndef FSM_HPP
#define FSM_HPP
#include <expected>
#include <span>
#include <type_traits>

#include "../utils/traits.hpp"
namespace monac::semantic {

template <std::ranges::contiguous_range T_>
class worm_machine {
    using value_type = T_::value_type;
    using size_type = T_::size_type;
    using const_pointer = T_::const_pointer;
    using storage_view = std::span<const value_type>;
    using iterator = storage_view::iterator;

    storage_view _m_span;
    size_type _m_start{0};
    size_type _m_end{0};

    // specialization of strings being passed to match span of chars

    template <std::size_t I_ = 0, utils::traits::tuple_like Tup_>
        requires(I_ >= 0 && std::convertible_to<std::tuple_element_t<0, Tup_>, std::string_view> && utils::traits::char_like<value_type>)
    bool _match_helper(Tup_& tup) {
        if constexpr (I_ == std::tuple_size_v<Tup_>) {
            return true;
        } else {
            auto head_string = std::string(std::get<I_>(tup));
            for (const auto c : head_string)
                if (!match("discardable", c)) return false;
            auto tail = _match_helper<I_ + 1, Tup_>(tup);
            if (!tail) return false;
            return true;
        }
    };

    // generalized
    template <std::size_t I_ = 0, utils::traits::tuple_like Tup_>
        requires(I_ >= 0 && !std::convertible_to<std::tuple_element_t<0, Tup_>, std::string_view> && !utils::traits::char_like<value_type>)
    bool _match_helper(Tup_& tup) {
        if constexpr (I_ == std::tuple_size_v<Tup_>) {
            return true;
        } else {
            auto head = std::get<I_>(tup);
            if (!match("discardable", head)) return false;
            auto tail = _match_helper<I_ + 1, Tup_>(tup);
            if (!tail) return false;
            return true;
        }
    }

   public:
    worm_machine() = delete;
    worm_machine(const T_& sequence_to_view) : _m_span{sequence_to_view} {};
    worm_machine(T_&& t) = delete;
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

    std::expected<value_type, std::string> match(const std::string& error, const value_type& v) {
        if (is_end()) return std::unexpected(error);
        if (v != peek()) return std::unexpected(error);
        return advance();
    };

    // generalized specialization

    template <typename... Args_>
        requires(sizeof...(Args_) > 1)
    std::expected<std::tuple<Args_...>, std::string> match(const std::string& error, Args_&&... args) {
        const auto start_before_sequence = _m_end;
        auto tup = std::forward_as_tuple(std::forward<Args_>(args)...);
        bool val = _match_helper(tup);
        if (val) return tup;
        _m_end = start_before_sequence;
        return std::unexpected(error);
    };

    virtual ~worm_machine() = default;
};

};  // namespace monac::semantic

#endif