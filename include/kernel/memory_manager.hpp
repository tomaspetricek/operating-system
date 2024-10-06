#ifndef KERNEL_MEMORY_MANAGER_HPP
#define KERNEL_MEMORY_MANAGER_HPP

#include <cstddef>
#include <cstdint>
#include <limits>
#include <cassert>

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
            std::size_t segment_size{0}; // includes this header
        };

        heap_segment *list_head_{nullptr};

        std::size_t minimum_segment_size(std::size_t bytes)
        {
            bytes += sizeof(heap_segment);
            bytes += bytes % 16 ? 16 - (bytes % 16) : 0; // what does it do?
            return bytes;
        }

        heap_segment *find_best_segment(std::size_t min_segment_size) const noexcept
        {
            heap_segment *curr, *best = nullptr;
            int32_t diff, best_diff = std::numeric_limits<int32_t>::max();

            // find allocation that is the closests in size to this request
            for (curr = list_head_; curr != nullptr; curr = curr->next)
            {
                diff = curr->segment_size - min_segment_size;

                if (!curr->is_allocated && diff < best_diff && diff >= 0)
                {
                    best = curr;
                    best_diff = diff;
                }
            }
            return best;
        }

        void try_splitting_segment(heap_segment *best, std::size_t min_segment_size) noexcept
        {
            const auto best_diff = best->segment_size - min_segment_size;

            // if the best difference we could come up was large, split up the segement
            // into two since our segment headers are rather large, the criterion for
            // splitting the segment is that when split, the segment not being requested
            // should be twice a header size
            if (best_diff > (2 * sizeof(heap_segment)))
            {
                bzero(((void *)(best)) + min_segment_size, sizeof(heap_segment));
                auto temp = best->next;
                best->next = static_cast<heap_segment *>(((void *)(best)) + min_segment_size);
                best->next->next = temp;
                best->next->prev = best;
                best->next->segment_size = best->segment_size - min_segment_size;
                best->segment_size = min_segment_size;
            }
        }

    public:
        explicit heap_allocator(address_t heap_start,
                                std::size_t heap_size) noexcept
        {
            assert(heap_size > sizeof(heap_segment));
            list_head_ = (heap_segment *)heap_start;
            bzero(list_head_, sizeof(heap_segment));
            list_head_->segment_size = heap_size;
        }

        memory_t allocate(std::size_t bytes) noexcept
        {
            const auto min_size = minimum_segment_size(bytes);
            auto best = find_best_segment(min_size);

            // there is no free memery now
            if (best == nullptr)
            {
                return nullptr;
            }
            try_splitting_segment(best, min_size);
            best->is_allocated = true;
            return best + 1; // move to the actaul memory
        }

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