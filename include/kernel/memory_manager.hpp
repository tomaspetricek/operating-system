#ifndef KERNEL_MEMORY_MANAGER_HPP
#define KERNEL_MEMORY_MANAGER_HPP

#include <cstddef>
#include <cstdint>

#include <common/stdlib.h>

namespace tp
{
    using memory_t = void *;
    using address_t = uint32_t;

    class heap_allocator
    {
        struct heap_segment
        {
            struct heap_segment *next{nullptr};
            struct heap_segment *prev{nullptr};
            bool is_allocated{false};
            uint32_t segment_size{0}; // includes this header
        };

        heap_segment *list_head_{nullptr};

    public:
        explicit heap_allocator(address_t heap_start,
                                std::size_t heap_size) noexcept
        {
            list_head_ = (heap_segment *)heap_start;
            bzero(list_head_, sizeof(heap_segment));
            list_head_->segment_size = heap_size;
        }

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