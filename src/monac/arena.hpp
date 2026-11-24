#ifndef ARENA_HPP
#define ARENA_HPP
#include <iostream>
#include <vector>

#ifndef ARENA_REGION_DEFAULT_CAPACITY
#define ARENA_REGION_DEFAULT_CAPACITY (8 * 1024)
#endif

namespace monac::memory {

namespace _internal_memory_resource_ {
template <typename Alloc_ = std::allocator<std::byte>>
class arena final {
    using alloc_traits = std::allocator_traits<Alloc_>;

    Alloc_ _m_allocator;
    std::byte* _m_buffer{nullptr};
    std::size_t _m_offset{0};
    std::size_t _m_capacity{0};

   public:
    arena() : _m_buffer{alloc_traits::allocate(_m_allocator, ARENA_REGION_DEFAULT_CAPACITY)}, _m_offset{0}, _m_capacity{ARENA_REGION_DEFAULT_CAPACITY} {};
    arena(const std::size_t size) : _m_buffer{alloc_traits::allocate(_m_allocator, size)}, _m_offset{0}, _m_capacity{size} {};
    arena(Alloc_ alloc, const std::size_t size) : _m_allocator{alloc}, _m_buffer{alloc_traits::allocate(alloc, size)}, _m_offset{0}, _m_capacity{size} {};
    arena(const arena&) = delete;
    arena& operator=(const arena&) = delete;
    arena(arena&& other) : _m_allocator(std::move(other._m_allocator)) {
        using std::swap;
        swap(_m_buffer, other._m_buffer);
        swap(_m_offset, other._m_offset);
        swap(_m_capacity, other._m_capacity);
    }

    arena& operator=(arena&& other) = delete;
    ~arena() {
        if (_m_buffer)
            alloc_traits::deallocate(_m_allocator, _m_buffer, _m_capacity);
    };

    void reset() { _m_offset = 0; };

    void* allocate(const std::size_t size, std::size_t align = alignof(std::max_align_t)) {
        auto current = reinterpret_cast<std::uintptr_t>(_m_buffer + _m_offset);
        std::uintptr_t aligned = (current + align - 1) & ~(align - 1);
        std::size_t padding = static_cast<std::size_t>(aligned - current);
        if (size > _m_capacity || _m_offset > _m_capacity - size || padding > _m_capacity - _m_offset - size) return nullptr;
        _m_offset += padding + size;
        return reinterpret_cast<void*>(aligned);
    };

    template <typename T_, typename... Args_>
    T_* make(Args_&&... args) {
        auto mem = allocate(sizeof(T_), alignof(T_));
        if (!mem) return nullptr;
        return new (mem) T_(std::forward<Args_>(args)...);
    };
};
}  // namespace _internal_memory_resource_

class arena_list final {
    std::vector<_internal_memory_resource_::arena<>> _m_list;
    std::size_t _m_current{0};

   public:
    arena_list() = default;
    ~arena_list() = default;

    template <typename T_, typename... Args_>
    T_* make(Args_&&... args) {
        if (_m_list.empty()) {
            _m_list.push_back(_internal_memory_resource_::arena<>{});
            if (auto* ptr = _m_list[_m_current].make<T_>(std::forward<Args_>(args)...)) return ptr;
        }
        constexpr std::size_t required = sizeof(T_) + alignof(T_) - 1;
        constexpr std::size_t capacity = required > ARENA_REGION_DEFAULT_CAPACITY ? required : ARENA_REGION_DEFAULT_CAPACITY;
        _m_list.emplace_back(capacity);
        _m_current = _m_list.size() - 1;
        return _m_list.back().make<T_>(std::forward<Args_>(args)...);
    };
};

};  // namespace monac::memory

#endif