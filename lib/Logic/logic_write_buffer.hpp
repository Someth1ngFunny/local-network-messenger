#include <iostream>
#include <string>
#include <atomic>
#include <queue>

#include "logic.hpp"

// TODO make better version of WriteBufferAsync
class WriteBufferAsync {
private:
  std::ostream& output_;
  std::mutex& mutex_write_buff_;
  std::atomic_bool is_buff_cout_ = false;
  std::queue<std::string> write_buffer_;

  void inline __free_write_buff() 
  {
    std::lock_guard lock(mutex_write_buff_);
    while (!write_buffer_.empty())
    {
      output_ << write_buffer_.front();
      write_buffer_.pop();
    }
  }

  void inline __add_to_buff(const std::string to_output_)
  {
    std::lock_guard lock(mutex_write_buff_);
    write_buffer_.push(to_output_);
  }

public:

  WriteBufferAsync(std::mutex& mutex_write_buff, std::ostream& output)
    : mutex_write_buff_(mutex_write_buff),
      output_(output)
  {
  }

  void inline buf_write(const std::string& str) {
    if (is_buff_cout_) {
      __add_to_buff(str);
    } else {
      std::lock_guard lock(mutex_write_buff_);
      output_ << str;
    }
  }

  void inline close_write() 
  {
    if (!is_buff_cout_) 
    {
      is_buff_cout_ = true;
    }
  }

  void inline open_write() 
  {
    if (is_buff_cout_) 
    {
      __free_write_buff();
      is_buff_cout_ = false;
    }
  }

};

/* class WriteBuffer {
private:
  std::ostream& output_;
  std::mutex mutex_write_buff_;
  std::atomic_bool is_buff_cout_ = false;
  std::queue<std::any> write_buffer_;

  void inline __free_write_buff() 
  {
    while (!write_buffer_.empty())
    {
      output_ << write_buffer_.front();
      write_buffer_.pop();
    }
  }

  void inline __add_to_buff(const std::any to_output_)
  {
    write_buffer_.push(to_output_);
  }

  class write_handler
  {
  private:
    WriteBuffer& self;
  public:
    write_handler(WriteBuffer& self) 
      : self(self) 
    {
    }
    template <class T>
    write_handler& operator<<(const T &v)
    {
      const std::lock_guard lock(self.mutex_write_buff_);
      if (self.is_buff_cout_) {
        self.__add_to_buff(v);
      } else {
        self.output_ << v;
      }
      return *this;
    }
    write_handler& operator<<(std::ostream& (*manip)(std::ostream&)) {
      self.output_ << manip;
      return *this;
    }
  };

public:

  WriteBuffer(std::ostream& output_)
    : output_(output_)
  {
  }

  write_handler inline buf_cout() {
    return write_handler(*this);
  }

  void buff_write() 
  {
    if (!is_buff_cout_) 
    {
      is_buff_cout_ = true;
    }
  }

  void free_write() 
  {
    if (!is_buff_cout_) 
    {
      __free_write_buff();
      is_buff_cout_ = false;
    }
  }

}; */