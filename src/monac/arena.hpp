#ifndef ARENA_HPP
#define ARENA_HPP
#include <iostream>
namespace monac::memory {

template <typename Alloc = std::allocator<std::byte>>
class arena {
    using alloc_traits = std::allocator_traits<Alloc>;

    Alloc m_allocator;
    std::byte* m_region{nullptr};

   public:
    arena() = default;
    arena(const std::size_t size) : m_region{new std::byte[size]} {};
};

};  // namespace monac::memory

#endif