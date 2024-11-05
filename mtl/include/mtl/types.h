#ifndef MTL_TYPES_H
#define MTL_TYPES_H

#include <exception>

namespace mtl {
using size_t = unsigned long long;

class NullIterator : public std::exception {
private:
    const char* msg_;

public:
    NullIterator() : msg_("This iterator is null.") {}
    NullIterator(const char* msg) : msg_(msg) {}
    virtual ~NullIterator() noexcept = default;
    const char* what() {
        return msg_;
    }
};

class EmptyContainer : public std::exception {
private:
    const char* msg_;

public:
    EmptyContainer() : msg_("This container is already empty.") {}
    EmptyContainer(const char* msg) : msg_(msg) {}
    virtual ~EmptyContainer() noexcept = default;
    const char* what() {
        return msg_;
    }
};
} // namespace mtl

#endif
