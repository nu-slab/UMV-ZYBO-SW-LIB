#include <slab/dma.hpp>

namespace slab
{
	dma::dma() {}
	dma::~dma() {}

	void dma::open_memory(uint32_t addr)
	{
		target_ = addr;
		if ((fd_ = open("/dev/mem", O_RDWR | O_SYNC)) == -1) FATAL;
		map_base_ = (volatile void *)mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, target_ & ~MAP_MASK);
		if (map_base_ == (void *)-1) FATAL;
		virt_addr_ = map_base_ + (target_ & MAP_MASK); // warning: op with (void *)
	}

	void dma::close_memory()
	{
		if (munmap((void *)map_base_, MAP_SIZE) == -1) FATAL;
		close(fd_);
	}

	uint64_t dma::read(uint32_t addr, uint8_t access_bytes)
	{
		uint64_t data;
		open_memory(addr);
		switch (access_bytes) {
			case 1:
				data = (uint64_t)*((uint8_t  *) virt_addr_);
				break;
			case 2:
				data = (uint64_t)*((uint16_t *) virt_addr_);
				break;
			case 4:
				data = (uint64_t)*((uint32_t *) virt_addr_);
				break;
			case 8:
				data = (uint64_t)*((uint64_t *) virt_addr_);
				break;
		}
		close_memory();
		return data;
	}

	uint64_t dma::write(uint32_t addr, uint64_t data, uint8_t access_bytes)
	{
		uint64_t read_data;
		open_memory(addr);
		switch (access_bytes) {
			case 1:
				*((uint8_t  *) virt_addr_)  = (uint8_t)data;
				read_data = (uint64_t)*((uint8_t  *) virt_addr_);
				break;
			case 2:
				*((uint16_t *) virt_addr_) = (uint16_t)data;
				read_data = (uint64_t)*((uint16_t *) virt_addr_);
				break;
			case 4:
				*((uint32_t *) virt_addr_) = (uint32_t)data;
				read_data = (uint64_t)*((uint32_t *) virt_addr_);
				break;
			case 8:
				*((uint64_t *) virt_addr_) = (uint64_t)data;
				read_data = (uint64_t)*((uint64_t *) virt_addr_);
				break;
		}
		close_memory();
		return read_data;
	}
}
