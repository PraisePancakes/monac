#ifndef ARENA_HPP
#define ARENA_HPP
#include <iostream>
namespace monac::memory {

template <typename Alloc = std::allocator<std::byte>>
class arena {
    using alloc_traits = std::allocator_traits<Alloc>;

    Alloc _m_allocator;
    std::byte* _m_region{nullptr};

   public:
    arena() = default;
    arena(const std::size_t size) : _m_region{new std::byte[size]} {};
};

};  // namespace monac::memory

#endif