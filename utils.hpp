#pragma once

#include "discord.hpp"

#include <nlohmann/json.hpp>
#include <mutex>
#include <condition_variable>
#include <cpprest/ws_client.h>
#include <cpprest/http_client.h>

class semaphore {
  private:
    std::mutex mutex_;
    std::condition_variable condition_;
    unsigned long count_; // Initialized as locked.
    unsigned long max_;

  public:
    semaphore(unsigned long max) : count_{max}, max_{max} {;}

    void notify() {
      std::lock_guard<decltype(mutex_)> lock(mutex_);
      ++count_;
      condition_.notify_one();
    }

    void wait() {
      std::unique_lock<decltype(mutex_)> lock(mutex_);
      while(!count_) // Handle spurious wake-ups.
        condition_.wait(lock);
      --count_;
    }

    bool try_wait() {
      std::lock_guard<decltype(mutex_)> lock(mutex_);
      if(count_) {
        --count_;
        return true;
      }
      return false;
    }

    void reset() {
      std::lock_guard<decltype(mutex_)> lock(mutex_);
      count_ = max_;
    }
};

namespace backend {
  discord::user parse_user(const nlohmann::json &);
  discord::role parse_role(const nlohmann::json &);
  discord::channel parse_channel(const nlohmann::json &);
}
