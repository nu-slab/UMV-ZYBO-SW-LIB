#ifndef DMA_H
#define DMA_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdint.h>

#define FATAL                                                \
  do {                                                       \
    fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
            __LINE__, __FILE__, errno, strerror(errno));     \
    exit(1);                                                 \
  } while (0)
#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

namespace slab
{
  class dma
  {
  private:
    int   fd_;
    off_t target_;
    volatile void *map_base_, *virt_addr_; 
    void open_memory(uint32_t addr);
    void close_memory();
  protected:
  public:
    dma();
    ~dma();
    uint64_t read(uint32_t addr, uint8_t access_bytes);
    uint64_t write(uint32_t addr, uint64_t data, uint8_t access_bytes);
  };
}

#endif // DMA_H
