#include "soy/weak.h"

_weak_pool::pool_t _weak_pool::pool_;
_weak_pool::idx_t  _weak_pool::head_ = 0;
size_t             _weak_pool::alloced_ = 0;
