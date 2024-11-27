#ifndef MTL_TYPES_H
#define MTL_TYPES_H

#include <cstddef>
#include <exception>

namespace mtl {
using std::size_t;
using difference_t = std::ptrdiff_t;

struct NullIterator : public std::exception {
    const char* msg_;

    NullIterator() : msg_("This iterator is null.") {}
    explicit NullIterator(const char* msg) : msg_(msg) {}
    ~NullIterator() noexcept override = default;
    const char* what() {
        return msg_;
    }
};

struct EmptyContainer : public std::exception {
    const char* msg_;

    EmptyContainer() : msg_("This container is already empty.") {}
    explicit EmptyContainer(const char* msg) : msg_(msg) {}
    ~EmptyContainer() noexcept override = default;
    [[nodiscard]] const char* what() {
        return msg_;
    }
};
} // namespace mtl

#endif
