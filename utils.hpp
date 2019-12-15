#pragma once
#include "discord.hpp"

#include <nlohmann/json.hpp>
#include <mutex>
#include <condition_variable>

//class CCasablancaClientErrorCode
//{
//  public:
//    CCasablancaClientErrorCode(int errCode) : _errCode(errCode) {};
//    /**
//      Check and translate to POSIX-conforming system error code.
//
//      For HTTP connection problems following codes are used by Casablanca:
//      std::errc::host_unreachable, std::errc::timed_out, std::errc::connection_aborted
//      */
//    bool IsStdSystemError(std::errc& stdErrorCode) const
//    {
//#ifdef _WINDOWS
//      const std::error_condition ec = utility::details::windows_category().default_error_condition(_errCode);
//#else
//      const std::error_condition ec = std::system_category().default_error_condition(_errCode);
//#endif
//      if (ec.category().name() != genericCategoryStrg)
//      {
//        return false;
//      }
//      else
//      {
//        stdErrorCode = std::errc(ec.value());
//        return true;
//      }
//    }
//
//    /**
//      Check if the reported error comes from SSL.
//
//      There is only one, generic server SLL certificate error at the moment!
//      */
//    bool IsSslError() const
//    {
//#ifdef _WINDOWS
//      const std::error_condition ec = utility::details::windows_category().default_error_condition(_errCode);
//
//      if (ec.category() == utility::details::windows_category())
//      {
//        return _errCode == ERROR_WINHTTP_SECURE_FAILURE;
//      }
//      else
//      {
//        return false;
//      }
//#else
//      const std::error_condition ec = std::system_category().default_error_condition(_errCode);
//
//      if (ec.category().name() == genericCategoryStrg)
//      {
//        return false;
//      }
//      else
//      {
//        // ??? OPEN TODO:::: must test, but it will depend on the SSL version !!!
//        // - patch Casablanca's ASIO code?
//        // return _errCode == 336458004; //== 0x140DF114  // OpenSSL error code  ??OR?? 335544539 == 0x140000DB
//        return true; // should be SSL
//      }
//#endif
//    }
//  private:
//    int _errCode;
//};
//

class semaphore
{
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
