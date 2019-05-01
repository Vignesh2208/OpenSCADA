#ifndef __PC_EMULATOR_INCLUDE_SYNC_QUEUE_H__
#define __PC_EMULATOR_INCLUDE_SYNC_QUEUE_H__

#include <iostream>
#include <queue>
#include <condition_variable>

using namespace std;

template <typename T> class SynchronizedQueue
{
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable condvar_;

    typedef std::lock_guard<std::mutex> lock;
    typedef std::unique_lock<std::mutex> ulock;

public:
    void push(T const &val)
    {
        lock l(mutex_); // prevents multiple pushes corrupting queue_
        bool wake = queue_.empty(); // we may need to wake consumer
        queue_.push(val);
        if (wake) condvar_.notify_one();
    }

    T pop()
    {
        ulock u(mutex_);
        while (queue_.empty())
            condvar_.wait(u);
        // now queue_ is non-empty and we still have the lock
        T retval = queue_.front();
        queue_.pop();
        return retval;
    }

    bool IsEmpty() {
        return queue_.empty();
    }
};

#endif