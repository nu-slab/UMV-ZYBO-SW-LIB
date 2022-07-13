// necessary
#include<thread>
#include<iostream>
#include<iomanip>
#include<slab/system.hpp>

// unnecessary
#include"mod/kbhit.hpp"

// exit flag
bool exit_flag = false;

void pid_feedback(slab::System& zynq){
	std::string buf;
	
	while (!exit_flag) {
		std::cout << "pid-feedback";
		std::cout << std::setw(4) << zynq.recv_rotation(true);
		std::cout << std::setw(4) << zynq.recv_rotation(false) << "\n" << std::flush;
		usleep(100000);
	}
}

int main(void){

	slab::System pl_connection("/dev/uio0");
	std::thread t(pid_feedback, std::ref(pl_connection));

	std::string buf;
	
	while (true) {
		if (kbhit()) {
			std::cout << std::endl;
			break;
		}
	}
	exit_flag = true;
	t.join();

	return 0;
}
