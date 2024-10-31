#ifndef MTL_DEQUE_H
#define MTL_DEQUE_H

#include <initializer_list>
#include <mtl/list.h>

namespace mtl {
    template <typename T>
    class deque {
    private:
        static size_t cal_buf_size() {
            return sizeof(T) < 512 ? 512 / sizeof(T) : 1;
        }
    };
} // namespace mtl

#endif
