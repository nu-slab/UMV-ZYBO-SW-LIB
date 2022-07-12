#include<thread>
#include<iostream>
#include<iomanip>
#include<slab/system.hpp>

bool kill_flag = false;

void pid_feedback(slab::System& zynq){
	std::string buf;
	
	std::cout << std::setw(4) << "r" << std::setw(4) << "l" << "\n";
	while (!kill_flag) {
		std::cout << std::setw(4) << zynq.recv_rotation(true);
		std::cout << std::setw(4) << zynq.recv_rotation(false) << "\r" << std::flush;
	}
}

int main(void){

	slab::System pl_connection("/dev/uio0");
	std::thread t(pid_feedback, std::ref(pl_connection));

	std::string buf;
	
	while (true) {
		std::cin >> buf;
		if (buf == "exit" || buf == "x" | std::cin.fail()) break;
	}
	kill_flag = true;
	t.join();

	return 0;
}
