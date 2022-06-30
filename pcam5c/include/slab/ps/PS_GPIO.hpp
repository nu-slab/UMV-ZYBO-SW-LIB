#ifndef _PS_GPIO_H
#define _PS_GPIO_H

#include <iostream>
#include <utility>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

namespace digilent
{
  class PS_GPIO {
  private:
    const uint32_t CAM_EN_PIN = 375;
  protected:
  public:
    PS_GPIO();
    ~PS_GPIO();
    void turnOnPowerToCam() const;
    void turnOffPowerToCam() const;
  };
}

#endif
