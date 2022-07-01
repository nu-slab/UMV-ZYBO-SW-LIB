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
			case 1:  msg += BLE + "blue [o] " + NC ; break;
			case 0:  msg +=       "blue [x] "      ; break;
			default: msg +=       "blue [?] "      ; break;
		}
		switch (detection & 2) {
			case 2:  msg += YLW + "yellow [o] " + NC ; break;
			case 0:  msg +=       "yellow [x] "      ; break;
			default: msg +=       "yellow [?] "      ; break;
		}
		switch (detection & 4) {
			case 4:  msg += RED + "red [o] " + NC ; break;
			case 0:  msg +=       "red [x] "      ; break;
			default: msg +=       "red [?] "      ; break;
		}
		switch (detection & 8) {
			case 8:  msg += PNK + "doll [o]" + NC ; break;
			case 0:  msg +=       "doll [x]"      ; break;
			default: msg +=       "doll [?]"      ; break;
		}
		
		std::cout << '\r' << msg << std::flush;
		
		if (kbhit()) {
			std::cout << std::endl;
			break;
		}
	}  
	return 0;
}
