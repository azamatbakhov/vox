#pragma once

#include <mutex>
#include <shared_mutex>

#define lock_(Mutex) \
    ::std::lock_guard ANONYMOUS_VAR(lock_mutex_) {Mutex}

#define shared_lock_(Mutex) \
    ::std::shared_lock ANONYMOUS_VAR(lock_mutex_) {Mutex}
