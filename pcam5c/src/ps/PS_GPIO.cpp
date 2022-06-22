#include <slab/ps/PS_GPIO.hpp>

namespace digilent
{
	PS_GPIO::PS_GPIO()
	{
		int fd;
		char fname[128];

		/* open CAM_GPIO */
		if (!(fd = open("/sys/class/gpio/export", O_WRONLY))) {
			std::cerr << "could not open CAM_GPIO" << std::endl;
		}
		if(!write(fd, std::to_string(CAM_EN_PIN).c_str(), 3)) {
			std::cerr << "could not enable CAM_GPIO" << std::endl;
		}
		close(fd);

		/* setting direction of port */
		sprintf(fname, "/sys/class/gpio/gpio%d/direction", CAM_EN_PIN);
		if (!(fd = open(fname, O_WRONLY))) {
			std::cerr << "could not open file of CAM_GPIO direction" << std::endl;
		}
		if(!write(fd, "out", 3)) {
			std::cerr << "could not set CAM_GPIO direction" << std::endl;
		}
		close(fd);
	}

	PS_GPIO::~PS_GPIO()
	{
		/* unable port */
		int fd = open("/sys/class/gpio/unexport", O_WRONLY);
		close(fd);
	}

	void PS_GPIO::turnOnPowerToCam() const
	{
		char fname[128];

		sprintf(fname, "/sys/class/gpio/gpio%d/value", CAM_EN_PIN);
		int cam_en_fd = open(fname, O_WRONLY);

		if(!write(cam_en_fd, "1", 1)) {
			std::cerr << "could not turn on to CAM" << std::endl;
		}

		close(cam_en_fd);
	}

	void PS_GPIO::turnOffPowerToCam() const
	{
		char fname[128];

		sprintf(fname, "/sys/class/gpio/gpio%d/value", CAM_EN_PIN);
		int cam_en_fd = open(fname, O_WRONLY);

		if(!write(cam_en_fd, "0", 1)) {
			std::cerr << "could not turn off CAM" << std::endl;
		}

		close(cam_en_fd);
	}
}
