#pragma once

/** ==========================================================================
* 2010 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================
*
* Example of a normal std::queue protected by a mutex for operations,
* making it safe for thread communication, using std::mutex from C++0x with
* the help from the std::thread library from JustSoftwareSolutions
* ref: http://www.stdthread.co.uk/doc/headers/mutex.html
*
* This exampel  was totally inspired by Anthony Williams lock-based data structures in
* Ref: "C++ Concurrency In Action" http://www.manning.com/williams */

#ifndef SHARED_QUEUE
#define SHARED_QUEUE

#include <queue>
#include <mutex>
#include <exception>
#include <condition_variable>

/** Multiple producer, multiple consumer thread safe queue
* Since 'return by reference' is used this queue won't throw */
template<typename T>
class shared_queue
{
  std::queue<T> queue_;
  mutable std::mutex m_;
  std::condition_variable data_cond_;

  shared_queue& operator=(const shared_queue&); // c++11 feature not yet in vs2010 = delete;
  shared_queue(const shared_queue& other); // c++11 feature not yet in vs2010 = delete;

public:
  shared_queue(){}

  void push(T item){
    std::lock_guard<std::mutex> lock(m_);
    queue_.push(item);
    data_cond_.notify_one();
  }

  /// \return immediately, with true if successful retrieval
  bool try_and_pop(T& popped_item){
    std::lock_guard<std::mutex> lock(m_);
    if(queue_.empty()){
      return false;
    }
    popped_item=std::move(queue_.front());
    queue_.pop();
    return true;
  }

  /// Try to retrieve, if no items, wait till an item is available and try again
  void wait_and_pop(T& popped_item){
    std::unique_lock<std::mutex> lock(m_); // note: unique_lock is needed for std::condition_variable::wait
    while(queue_.empty())
    { //                       The 'while' loop below is equal to
      data_cond_.wait(lock);  //data_cond_.wait(lock, [](bool result){return !queue_.empty();});
    }
    popped_item=std::move(queue_.front());
    queue_.pop();
  }

  bool empty() const{
    std::lock_guard<std::mutex> lock(m_);
    return queue_.empty();
  }

  unsigned size() const{
    std::lock_guard<std::mutex> lock(m_);
    return queue_.size();
  }
};

#endif