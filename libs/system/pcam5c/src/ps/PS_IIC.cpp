#include <slab/ps/PS_IIC.hpp>

namespace digilent
{
  PS_IIC::PS_IIC(const std::string& iic_devname, uint8_t slave_addr)
  {
    /* open device */
    if (!(iic_fd_= open((std::string("/dev/") + iic_devname).c_str(), O_RDWR))) {
      std::cerr << "could not open device of iic" << std::endl;
    }

    /* set of slave_addr */
    if(ioctl(iic_fd_, I2C_SLAVE, (slave_addr >> 1)) < 0) {
      std::cerr << "could not set iic slave_addr" << std::endl;
    }
  }

  PS_IIC::~PS_IIC() { close(iic_fd_); }

  uint8_t PS_IIC::iic_read(uint16_t addr) const
  {
    uint8_t write_buf[2] = {(uint8_t)((addr & 0xFF00) >> 8), (uint8_t)(addr & 0x00FF)};
    uint8_t read_buf[1];

    /* write addr to IIC */
    if (!write(iic_fd_, write_buf, 2)) {
      std::cerr << "could not write addr to iic device" << std::endl;
    }

    /* read data from IIC */
    if (!read(iic_fd_, read_buf, 1)) {
      std::cerr << "could not read data from iic device" << std::endl;
    }

    return read_buf[0];
  }

  void PS_IIC::iic_write(uint16_t addr, uint8_t data) const
  {
    uint8_t write_buf[3] = {(uint8_t)((addr & 0xFF00) >> 8), (uint8_t)(addr & 0x00FF), data};

    /* write addr and data to IIC */
    if (!write(iic_fd_, write_buf, 3)) {
      std::cerr << "could not write addr and data to iic device" << std::endl;
    }
  }
}
