//-----------------------------------------------------------------------------
// <pspl.hpp>
//  - Header of slab::PSPL class
//    - Declared of slab::PSPL class
//    - Declared of slab::mutex class
//-----------------------------------------------------------------------------
// Version 1.00 (Sep. 23, 2020)
//  - Added declaration of slab::PSPL class
//  - Added declaration of slab::mutex class
//-----------------------------------------------------------------------------
// (C) 2020-2021 Naofumi Yoshinaga, Taichi Saikai All rights reserved.
//-----------------------------------------------------------------------------

#ifndef PSPL_HPP
#define PSPL_HPP

#include <iostream>
#include <cstdlib>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <cstdint>

#define WRITE_ADDR   0x0
#define WRITE_VALUE  0x1
#define WRITE_ENABLE 0x2
#define READ_ADDR    0x3

namespace slab
{
  class mutex
  {
  private:
    /* 
     * true  : locked, 
     * false : unlocked 
     */
    bool mtx;
  protected:
  public:
    mutex();
    ~mutex();
    void lock();
    void unlock();
  };

  class PSPL
  {
  private:
    volatile uint32_t *reg_;
    int uiofd_;
    bool open_flag_;
    mutex mtx_w_;
    mutex mtx_r_;
  protected:
  public:
    PSPL();
    PSPL(const char*);
    PSPL(std::string);
    ~PSPL();
    bool open_device(const char*);
    bool close_device();
    int read(int addr);
    void write(int addr, int data);
  };
}

#endif // PSPL_HPP
