
#include "../src/monac/arena.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <type_traits>
#include <vector>

#include "doctest.h"

// ------------------------------------------------------------
// Test types
// ------------------------------------------------------------

struct aligned_16 {
    alignas(16) std::byte data[16];
};

struct aligned_64 {
    alignas(64) std::byte data[64];
};

struct aligned_256 {
    alignas(256) std::byte data[256];
};

struct large_type {
    char data[16 * 1024];
};

struct test_object {
    int value;
    explicit test_object(int v) : value(v) {}
};

struct counted_object {
    static inline int constructions = 0;
    static inline int destructions = 0;

    counted_object() { ++constructions; }
    ~counted_object() { ++destructions; }
};

// ------------------------------------------------------------
// Basic allocation
// ------------------------------------------------------------

TEST_CASE("arena: basic allocation") {
    monac::memory::arena_list arenas;

    auto* a = arenas.make<int>(42);
    auto* b = arenas.make<int>(100);

    REQUIRE(a != nullptr);
    REQUIRE(b != nullptr);

    CHECK(*a == 42);
    CHECK(*b == 100);
    CHECK(a != b);
}

TEST_CASE("arena: multiple allocations retain their values") {
    monac::memory::arena_list arenas;

    std::vector<int*> pointers;

    for (int i = 0; i < 5000; ++i) {
        auto* ptr = arenas.make<int>(i);

        REQUIRE(ptr != nullptr);
        pointers.push_back(ptr);
    }

    for (int i = 0; i < 5000; ++i) {
        REQUIRE(pointers[i] != nullptr);
        CHECK(*pointers[i] == i);
    }
}

// ------------------------------------------------------------
// Alignment
// ------------------------------------------------------------

TEST_CASE("arena: default alignment") {
    monac::memory::arena_list arenas;

    auto* ptr = arenas.make<std::max_align_t>();

    REQUIRE(ptr != nullptr);

    CHECK(
        reinterpret_cast<std::uintptr_t>(ptr) % alignof(std::max_align_t) == 0);
}

TEST_CASE("arena: 16 byte alignment") {
    monac::memory::arena_list arenas;

    auto* ptr = arenas.make<aligned_16>();

    REQUIRE(ptr != nullptr);

    CHECK(reinterpret_cast<std::uintptr_t>(ptr) % 16 == 0);
}

TEST_CASE("arena: 64 byte alignment") {
    monac::memory::arena_list arenas;

    auto* ptr = arenas.make<aligned_64>();

    REQUIRE(ptr != nullptr);

    CHECK(reinterpret_cast<std::uintptr_t>(ptr) % 64 == 0);
}

TEST_CASE("arena: 256 byte alignment") {
    monac::memory::arena_list arenas;

    auto* ptr = arenas.make<aligned_256>();

    REQUIRE(ptr != nullptr);

    CHECK(reinterpret_cast<std::uintptr_t>(ptr) % 256 == 0);
}

TEST_CASE("arena: mixed alignment allocations") {
    monac::memory::arena_list arenas;

    auto* a = arenas.make<int>(10);
    auto* b = arenas.make<aligned_64>();
    auto* c = arenas.make<int>(20);
    auto* d = arenas.make<aligned_256>();

    REQUIRE(a != nullptr);
    REQUIRE(b != nullptr);
    REQUIRE(c != nullptr);
    REQUIRE(d != nullptr);

    CHECK(reinterpret_cast<std::uintptr_t>(b) % 64 == 0);
    CHECK(reinterpret_cast<std::uintptr_t>(d) % 256 == 0);

    CHECK(*a == 10);
    CHECK(*c == 20);
}

// ------------------------------------------------------------
// Large allocations and region growth
// ------------------------------------------------------------

TEST_CASE("arena: allocation larger than default region") {
    monac::memory::arena_list arenas;

    auto* ptr = arenas.make<large_type>();

    REQUIRE(ptr != nullptr);

    ptr->data[0] = 'A';
    ptr->data[sizeof(ptr->data) - 1] = 'Z';

    CHECK(ptr->data[0] == 'A');
    CHECK(ptr->data[sizeof(ptr->data) - 1] == 'Z');
}

TEST_CASE("arena: creates multiple regions") {
    monac::memory::arena_list arenas;

    std::vector<int*> pointers;

    for (int i = 0; i < 5000; ++i) {
        auto* ptr = arenas.make<int>(i);

        REQUIRE(ptr != nullptr);
        pointers.push_back(ptr);
    }

    for (int i = 0; i < 5000; ++i)
        CHECK(*pointers[i] == i);
}

TEST_CASE("arena: alternating large and small allocations") {
    monac::memory::arena_list arenas;

    auto* first = arenas.make<int>(1);
    auto* large = arenas.make<large_type>();
    auto* second = arenas.make<int>(2);

    REQUIRE(first != nullptr);
    REQUIRE(large != nullptr);
    REQUIRE(second != nullptr);

    CHECK(*first == 1);
    CHECK(*second == 2);
}

// ------------------------------------------------------------
// Pointer stability
// ------------------------------------------------------------

TEST_CASE("arena: pointers remain valid after region growth") {
    monac::memory::arena_list arenas;

    auto* first = arenas.make<int>(12345);

    REQUIRE(first != nullptr);

    for (int i = 0; i < 5000; ++i) {
        auto* ptr = arenas.make<int>(i);
        REQUIRE(ptr != nullptr);
    }

    CHECK(*first == 12345);
}

// ------------------------------------------------------------
// Construction
// ------------------------------------------------------------

TEST_CASE("arena: constructs objects with constructor arguments") {
    monac::memory::arena_list arenas;

    auto* ptr = arenas.make<test_object>(42);

    REQUIRE(ptr != nullptr);
    CHECK(ptr->value == 42);
}

TEST_CASE("arena: constructs multiple different object types") {
    monac::memory::arena_list arenas;

    auto* integer = arenas.make<int>(42);
    auto* object = arenas.make<test_object>(100);
    auto* character = arenas.make<char>('A');

    REQUIRE(integer != nullptr);
    REQUIRE(object != nullptr);
    REQUIRE(character != nullptr);

    CHECK(*integer == 42);
    CHECK(object->value == 100);
    CHECK(*character == 'A');
}

// ------------------------------------------------------------
// Repeated allocations and boundary behavior
// ------------------------------------------------------------

TEST_CASE("arena: many small allocations") {
    monac::memory::arena_list arenas;

    constexpr int count = 100000;

    std::vector<int*> pointers;
    pointers.reserve(count);

    for (int i = 0; i < count; ++i) {
        auto* ptr = arenas.make<int>(i);

        REQUIRE(ptr != nullptr);
        pointers.push_back(ptr);
    }

    for (int i = 0; i < count; ++i)
        CHECK(*pointers[i] == i);
}

TEST_CASE("arena: zero-sized allocation type") {
    struct empty_type {};

    monac::memory::arena_list arenas;

    auto* a = arenas.make<empty_type>();
    auto* b = arenas.make<empty_type>();

    REQUIRE(a != nullptr);
    REQUIRE(b != nullptr);
}

// ------------------------------------------------------------
// Move construction
// ------------------------------------------------------------

TEST_CASE("arena: move construction preserves allocated objects") {
    using arena_type = monac::memory::_internal_memory_resource_::arena<>;

    arena_type original;

    auto* ptr = original.make<int>(42);

    REQUIRE(ptr != nullptr);

    arena_type moved(std::move(original));

    CHECK(*ptr == 42);

    auto* next = moved.make<int>(100);

    REQUIRE(next != nullptr);
    CHECK(*next == 100);
}

// ------------------------------------------------------------
// Compile-time properties
// ------------------------------------------------------------

TEST_CASE("arena: type properties") {
    using arena_type = monac::memory::_internal_memory_resource_::arena<>;

    CHECK_FALSE(std::is_copy_constructible_v<arena_type>);
    CHECK_FALSE(std::is_copy_assignable_v<arena_type>);
    CHECK(std::is_move_constructible_v<arena_type>);
}
