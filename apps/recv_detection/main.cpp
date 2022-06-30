#include <iostream>
#include <chrono>
#include <thread>
#include <slab/system.hpp>
#include "kbhit.hpp"

int main(void)
{
  slab::System zynq("/dev/uio0");
  std::cout << "** Press any key to exit. **" << std::endl;
  std::string message = "";
  while (true) {
    int detection = zynq.recv_detection();
    switch (detection) {
    case 0:  message = "doll [x] traffic_light [x]"; break;
    case 1:  message = "doll [o] traffic_light [x]"; break;
    case 2:  message = "doll [x] traffic_light [o]"; break;
    case 3:  message = "doll [o] traffic_light [o]"; break;
    default: message = "doll [?] traffic_light [?]"; break;
    }
    std::cout << '\r' << message << std::flush;
    if (kbhit()) {
      std::cout << std::endl;
      break;
    }
  }  
  return 0;
}
