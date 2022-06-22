#ifndef _PS_IIC_H_
#define _PS_IIC_H_

#include <iostream>
#include <utility>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

namespace digilent
{
  class PS_IIC {
  private:
    int iic_fd_;
  protected:
  public:
    PS_IIC(const std::string& iic_devname, uint8_t slave_addr);
    ~PS_IIC();
    uint8_t iic_read(uint16_t addr) const;
    void iic_write(uint16_t addr, uint8_t data) const;
  };
}

#endif
