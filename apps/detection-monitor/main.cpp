// necessary
#include<thread>
#include<iostream>
#include<math.h>
#include<slab/system.hpp>
#include"fixed_queue.hpp"

// unnecessary
#include"kbhit.hpp"

#define CLASS_NUM  4
#define BUF_SIZE  20
#define DETECT_TH 10
#define WAIT_TIME 500000

bool exit_flag = false;
bool detect_flag[CLASS_NUM];

void detection_feedback(slab::System& zynq){

	FixedQueue<bool> detection[CLASS_NUM];

	uint8_t i, j, result;

	for(i=0;i<CLASS_NUM;i++) {
		detection[i].fixed_size(BUF_SIZE);
	}

	while (!exit_flag) {
		result = zynq.recv_detection();
	
		for(i=0; i < CLASS_NUM; i++){
			detection[i].push_back((result & uint8_t(pow(2,i))));
		}
				
		for(i=0; i < CLASS_NUM; i++){
			if(detection[i].count > DETECT_TH){ detect_flag[i] = true; }
			else { detect_flag[i] = false; }
			std::cout << detect_flag[i];
		}
		std::cout << "\r";

		usleep(WAIT_TIME);
	}
}

int main(void){

	slab::System pl_connection("/dev/uio0");
	std::thread t(detection_feedback, std::ref(pl_connection));


	while (true) {
		if (kbhit()) {
			break;
		}
	}
	exit_flag = true;
	t.join();

	return 0;
}


