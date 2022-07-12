#include <iostream>
#include <chrono>
#include <thread>
#include <slab/system.hpp>
#include "kbhit.hpp"

#define NC "\e[0m"
#define RED "\e[0;31m"
#define YLW "\e[0;33m"
#define BLE "\e[0;34m"
#define PNK "\e[0;35m"
#define REDB "\e[41m"

int main(void)
{
	slab::System zynq("/dev/uio0");
	std::cout << "** Press any key to exit. **" << std::endl;
	
	std::string msg = "";
	
	while (true) {
		int detection = zynq.recv_detection();
		switch (detection & 1) {
			case 1:  msg += "\e[0;34m blue [o] \e[0m"; break;
			case 0:  msg += " blue [x] "; break;
			default: msg += " blue [?] "; break;
		}
		switch (detection & 2) {
			case 2:  msg += "\e[0;33m yellow [o] \e[0m"; break;
			case 0:  msg += " yellow [x] "; break;
			default: msg += " yellow [?] "; break;
		}
		switch (detection & 4) {
			case 4:  msg += "\e[0;31m red [o] \e[0m"; break;
			case 0:  msg += " red [x] "; break;
			default: msg += " red [?] "; break;
		}
		switch (detection & 8) {
			case 8:  msg += "\e[0;35m doll [o] \e[0m"; break;
			case 0:  msg += " doll [x]"; break;
			default: msg += " doll [?]"; break;
		}

		std::cout << '\r' << msg << std::flush;
		msg = "";
		if (kbhit()) {
			std::cout << std::endl;
			break;
		}
	}  
	return 0;
}
