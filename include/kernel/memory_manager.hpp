#ifndef KERNEL_MEMORY_MANAGER_HPP
#define KERNEL_MEMORY_MANAGER_HPP

#include <cstdint>
#include <cstddef>

namespace tp
{
    using memory_t = void *;

    class heap_allocator
    {
        memory_t allocate(std::size_t amount) noexcept {}

        void deallocate(memory_t memory) noexcept {}
    };

    class page_allocator
    {
        memory_t allocate() noexcept {}

        void deallocate(memory_t memory) noexcept {}
    };

    class memory_manager
    {
        memory_t allocate_page() noexcept {}

        void deallocate_page(memory_t memory) noexcept {}

        memory_t allocate(std::size_t amount) noexcept {}

        void deallocate(memory_t memory) noexcept {}
    };
} // namespace tp

#endif // KERNEL_MEMORY_MANAGER_HPP