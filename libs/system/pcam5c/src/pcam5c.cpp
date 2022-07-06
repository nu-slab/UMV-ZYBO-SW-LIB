#include <slab/pcam5c.hpp>

namespace digilent
{
  pcam5c::pcam5c(const std::string& iic_devname, OV5640_cfg::mode_t mode, OV5640_cfg::awb_t  awb):
		width_ (OV5640_cfg::resolutions[mode].width ),
		height_(OV5640_cfg::resolutions[mode].height)
	{
		psgpio_ = std::make_unique<PS_GPIO>();
		psiic_  = std::make_unique<PS_IIC>(iic_devname, OV5640_cfg::OV5640_SLAVE_ADDR);
		reset();
		init();
		applyMode(mode); 
		applyAwb(awb);
	}

	pcam5c::~pcam5c() { shutdown(); }

	void pcam5c::init() const
	{
		uint8_t id_h, id_l;
		readReg(OV5640_cfg::REG_ID_H, id_h);
		readReg(OV5640_cfg::REG_ID_H, id_h);
		readReg(OV5640_cfg::REG_ID_L, id_l);
		if (id_h != OV5640_cfg::DEV_ID_H_ || id_l != OV5640_cfg::DEV_ID_L_) {
			char msg[100];
			snprintf(msg, sizeof(msg), "Got %02x %02x. Expected %02x %02x\r\n",
					id_h, id_l, OV5640_cfg::DEV_ID_H_, OV5640_cfg::DEV_ID_L_);
			throw std::runtime_error(std::string(msg));
		}

		writeReg(0x3103, 0x11);
		writeReg(0x3008, 0x82);
		usleep(10000); // usleep(1000000);

		const size_t n = sizeof(OV5640_cfg::cfg_init_) / sizeof(OV5640_cfg::cfg_init_[0]);
		for(size_t i = 0; i < n; ++i) 
			writeReg(OV5640_cfg::cfg_init_[i].addr, OV5640_cfg::cfg_init_[i].data);
	}

	void pcam5c::shutdown() const
	{
		// writeReg(0x3008, 0x82);
		psgpio_->turnOffPowerToCam();
		usleep(10000);
	}

	void pcam5c::reset() const
	{
		writeReg(0x3008, 0x82);
		psgpio_->turnOffPowerToCam();
		usleep(10000);
		psgpio_->turnOnPowerToCam();
		usleep(10000);
	}

	void pcam5c::readReg(uint16_t reg_addr, uint8_t& buf) const
	{
		buf = psiic_->iic_read(reg_addr);
		usleep(10000);
	}

	void pcam5c::writeReg(uint16_t reg_addr, uint8_t const reg_data) const
	{
		auto cnt = 10;
		while (true) {
			psiic_->iic_write(reg_addr, reg_data);
			usleep(10000);

			if (reg_addr == 0x3008) break;

			uint8_t buf;
			readReg(reg_addr, buf);
			if (buf == reg_data) 
				std::cout << "[PCam init : Status OK] ";
			else 
				std::cout << "[PCam init : Status NG] ";

			printf("addr : 0x%04X, write : 0x%02X, read : 0x%02X\n", reg_addr, (int)reg_data, (int)buf);
			if (buf == reg_data) break;

			cnt--;
			if (cnt == 0) 
				throw std::runtime_error("process that write to reg by using iic is failure");
		}
	}

	void pcam5c::applyMode(OV5640_cfg::mode_t mode) const
	{
		if (mode >= OV5640_cfg::mode_t::MODE_END) 
			throw std::runtime_error("OV5640 MODE setting is invalid");
		writeReg(0x3008, 0x42);
		const auto cfg_mode = &OV5640_cfg::modes[mode];
		writeConfig(cfg_mode->cfg, cfg_mode->cfg_size);
		writeReg(0x3008, 0x02);
	}

	void pcam5c::applyAwb(OV5640_cfg::awb_t awb) const
	{
		if (awb >= OV5640_cfg::awb_t::AWB_END) 
			throw std::runtime_error("OV5640 AWB setting is invalid");
		writeReg(0x3008, 0x42);
		auto cfg_mode = &OV5640_cfg::awbs[awb];
		writeConfig(cfg_mode->cfg, cfg_mode->cfg_size);
		writeReg(0x3008, 0x02);
	}

	void pcam5c::writeConfig(OV5640_cfg::config_word_t const* cfg, size_t cfg_size) const
	{
		for (size_t i = 0; i < cfg_size; ++i) 
			writeReg(cfg[i].addr, cfg[i].data);
	}

	bool pcam5c::set_mode(OV5640_cfg::mode_t& mode) const
	{
		if (mode >= OV5640_cfg::mode_t::MODE_END)
			return false;

		// [7]=0 Software reset; [6]=1 Software power down; Default=0x02
		writeReg(0x3008, 0x42);

		auto cfg_mode = &OV5640_cfg::modes[mode];
		writeConfig(cfg_mode->cfg, cfg_mode->cfg_size);

		// [7]=0 Software reset; [6]=0 Software power down; Default=0x02
		writeReg(0x3008, 0x02);
		return true;
	}

	bool pcam5c::set_awb(OV5640_cfg::awb_t awb) const
	{
		if (awb >= OV5640_cfg::awb_t::AWB_END)
			return false;
		// [7]=0 Software reset; [6]=1 Software power down; Default=0x02
		writeReg(0x3008, 0x42);

		auto cfg_mode = &OV5640_cfg::awbs[awb];
		writeConfig(cfg_mode->cfg, cfg_mode->cfg_size);

		// [7]=0 Software reset; [6]=0 Software power down; Default=0x02
		writeReg(0x3008, 0x02);
		return true;
	}

	void pcam5c::work(OV5640_cfg::mode_t mode) const
	{
		// Bring up input pipeline back-to-front
		std::cout << std::endl << "### Bring up input pipeline back-to-front ###" << std::endl;
		{
			std::cout << "stage 1" << std::endl;
			MIPI_CSI_2_RX_mWriteReg(XPAR_SYSTEM_PCAMINTERFACE_MIPI_CSI_2_RX_S_AXI_LITE_BASEADDR, CR_OFFSET, (CR_RESET_MASK & ~CR_ENABLE_MASK));
			MIPI_D_PHY_RX_mWriteReg(XPAR_SYSTEM_PCAMINTERFACE_MIPI_D_PHY_RX_S_AXI_LITE_BASEADDR, CR_OFFSET, (CR_RESET_MASK & ~CR_ENABLE_MASK));
			reset();
		}
		{
			std::cout << "stage 2" << std::endl;
			Xil_Out32(GAMMA_BASE_ADDR, 3); // Set Gamma correction factor to 1/1.8
			init();
		}
		{
			std::cout << "stage 3" << std::endl;
			MIPI_CSI_2_RX_mWriteReg(XPAR_SYSTEM_PCAMINTERFACE_MIPI_CSI_2_RX_S_AXI_LITE_BASEADDR, CR_OFFSET, CR_ENABLE_MASK);
			MIPI_D_PHY_RX_mWriteReg(XPAR_SYSTEM_PCAMINTERFACE_MIPI_D_PHY_RX_S_AXI_LITE_BASEADDR, CR_OFFSET, CR_ENABLE_MASK);
			set_mode(mode);
			set_awb(OV5640_cfg::awb_t::AWB_ADVANCED);
		}
		std::cout << std::endl << "OV5640 Initialized and working start !\n" << std::endl;
	}
}

