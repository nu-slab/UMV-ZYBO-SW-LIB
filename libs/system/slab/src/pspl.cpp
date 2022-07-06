//-----------------------------------------------------------------------------
// <pspl.cpp>
//  - Defined functions of slab::PSPL class and slab::mutex class
//-----------------------------------------------------------------------------
// Version 1.00 (Sep. 23, 2020)
//  - Added definition for functions of slab::PSPL class
//  - Added definition for functions of slab::mutex class
//-----------------------------------------------------------------------------
// (C) 2020-2021 Naofumi Yoshinaga, Taichi Saikai All rights reserved.
//-----------------------------------------------------------------------------

#include <slab/pspl.hpp>

namespace slab
{
	/* mutex */
	mutex::mutex() { mtx = false; }
	mutex::~mutex() { mtx = false; }
	void mutex::lock() { while (mtx); mtx = true; }
	void mutex::unlock() { mtx = false; }

	/* constructor / destructor */
	PSPL::PSPL() { open_flag_ = false; }
	PSPL::PSPL(const char *dev)
	{
		open_flag_ = false;
		if (!open_flag_) {
			open_device(dev);
		}
	}
	PSPL::PSPL(std::string dev)
	{
		open_flag_ = false;
		if (!open_flag_) {
			open_device(dev.c_str());
		}
	}
	PSPL::~PSPL()
	{
		if (open_flag_) {
			close_device();
		}
	}

	/* device */
	bool PSPL::open_device(const char* dev)
	{
		if (!open_flag_) {
			/* open device */
			if ((uiofd_ = open(dev, O_RDWR | O_SYNC)) < 0) {
				std::cerr << "ERROR: device open failed: " << dev << std::endl;
				return false;
			}
			/* mmap register */
			reg_ = (uint32_t *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, uiofd_, 0);
			if (reg_ == MAP_FAILED) {
				std::cerr << "ERROR: mmap failed: reg_" << std::endl;
				close(uiofd_);
				return false;
			}
			/* change flag */
			open_flag_ = true;
		}
		return true;
	}
	bool PSPL::close_device()
	{
		if (open_flag_) {
			munmap((void*)reg_, 0x1000);
			close(uiofd_);
			open_flag_ = false;
		}
		return true;
	}

	/* resister R/W */
	int PSPL::read(int addr) {
		int data;
		mtx_r_.lock();
		data = reg_[addr];
		mtx_r_.unlock();
		return data;
	}
	void PSPL::write(int addr, int data) {
		mtx_w_.lock();
		reg_[addr] = data;
		mtx_w_.unlock();
	}

} // namespace slab
