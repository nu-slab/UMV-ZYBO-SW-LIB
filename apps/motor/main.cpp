#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <cstdio>
#include <cmath>
#include <slab/system.hpp>
#include "kbhit.hpp"
#include "live_plot.hpp"

#define ACCEL_MAX  127
#define ACCEL_MIN -127
#define ACCEL_INC    8
#define WAIT_MS     10
#define EMPH(str) (std::string("\033[1m") + (str) + std::string("\033[0m\033[32m"))

char    key = 0;  
int8_t  accel_l = 0, accel_r = 0;
int32_t left_rotation = 0, right_rotation = 0;
double  kp = 20.0, ki = 2.0, kd = 1.0, bias = 0;
bool    kill = false;

void info(const char key);
void loop(slab::System &zynq);

int main(void)
{
  slab::System zynq("/dev/uio0");

  FILE *fp = fopen("gain.log", "r");
  if (fp != nullptr) { // gain file found
    if (fscanf(fp, "%lf %lf %lf %lf\n", &kp, &ki, &kd, &bias) < 4) {
      std::cerr << "[ERROR] fscanf failed" << std::endl;
      exit(1);
    }
  }
  int32_t p = static_cast<int32_t>(std::round(kp * (1 << 16)));
  int32_t i = static_cast<int32_t>(std::round(ki * (1 << 16)));
  int32_t d = static_cast<int32_t>(std::round(kd * (1 << 16)));
  zynq.set_pid_gain(p, i, d, bias, true);
  zynq.set_pid_gain(p, i, d, bias, false);

  std::thread t(live_plot), s(loop, std::ref(zynq));
  info(key);
  while (true) {
    if (kbhit()) {
      key = getchar();
      switch (key) {
      case 'k': 
	accel_l = std::min(accel_l + ACCEL_INC, ACCEL_MAX);
	accel_r = std::min(accel_r + ACCEL_INC, ACCEL_MAX);
	zynq.send_accel(accel_l, accel_r); 
	break;
      case 'j': 
	accel_l = std::max(accel_l - ACCEL_INC, ACCEL_MIN);
	accel_r = std::max(accel_r - ACCEL_INC, ACCEL_MIN);
	zynq.send_accel(accel_l, accel_r); 
	break;
      case 'l':
	accel_l = std::min(accel_l + ACCEL_INC, ACCEL_MAX);
	accel_r = std::max(accel_r - ACCEL_INC, ACCEL_MIN);
	zynq.send_accel(accel_l, accel_r); 
	break;
      case 'h':
	accel_l = std::max(accel_l - ACCEL_INC, ACCEL_MIN);
	accel_r = std::min(accel_r + ACCEL_INC, ACCEL_MAX);
	zynq.send_accel(accel_l, accel_r); 
	break;
      case 'b':
	while (accel_l != 0 || accel_r != 0) {
	  if      (accel_l > 0) accel_l--;
	  else if (accel_l < 0) accel_l++;
	  if      (accel_r > 0) accel_r--;
	  else if (accel_r < 0) accel_r++;
	  zynq.send_accel(accel_l, accel_r); 
	  std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_MS));
	}
	break;
      case 'm': 
	accel_l = accel_r = 100; // ACCEL_MAX;
	zynq.send_accel(accel_l, accel_r); 
	break;
      case 'n': {
	const int ave = (accel_l + accel_r) / 2;
	while (accel_l != ave || accel_r != ave) {
	  if      (accel_l < ave) accel_l++;
	  else if (accel_l > ave) accel_l--;
	  if      (accel_r < ave) accel_r++;
	  else if (accel_r > ave) accel_r--;
	  zynq.send_accel(accel_l, accel_r); 
	  std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_MS));
	}
	break;
      }
      case 'q':
	while (accel_l != 0 || accel_r != 0) {
	  if      (accel_l > 0) accel_l--;
	  else if (accel_l < 0) accel_l++;
	  if      (accel_r > 0) accel_r--;
	  else if (accel_r < 0) accel_r++;
	  zynq.send_accel(accel_l, accel_r); 
	  std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_MS));
	}
	break;
      case 'p':
	kp = kp + 0.1;
	p = static_cast<int32_t>(std::roundf(kp * (1 << 16)));
	i = static_cast<int32_t>(std::roundf(ki * (1 << 16)));
	d = static_cast<int32_t>(std::roundf(kd * (1 << 16)));
	zynq.set_pid_gain(p, i, d, bias,  true);
	zynq.set_pid_gain(p, i, d, bias, false);
	break;
      case 'P':
	kp = std::max(kp - 0.1, 0.0);
	p = static_cast<int32_t>(std::roundf(kp * (1 << 16)));
	i = static_cast<int32_t>(std::roundf(ki * (1 << 16)));
	d = static_cast<int32_t>(std::roundf(kd * (1 << 16)));
	zynq.set_pid_gain(p, i, d, bias,  true);
	zynq.set_pid_gain(p, i, d, bias, false);
	break;
      case 'i':
	ki = ki + 0.1;
	p = static_cast<int32_t>(std::roundf(kp * (1 << 16)));
	i = static_cast<int32_t>(std::roundf(ki * (1 << 16)));
	d = static_cast<int32_t>(std::roundf(kd * (1 << 16)));
	zynq.set_pid_gain(p, i, d, bias,  true);
	zynq.set_pid_gain(p, i, d, bias, false);
	break;
      case 'I':
	ki = std::max(ki - 0.1, 0.0);
	p = static_cast<int32_t>(std::roundf(kp * (1 << 16)));
	i = static_cast<int32_t>(std::roundf(ki * (1 << 16)));
	d = static_cast<int32_t>(std::roundf(kd * (1 << 16)));
	zynq.set_pid_gain(p, i, d, bias,  true);
	zynq.set_pid_gain(p, i, d, bias, false);
	break;
      case 'd':
	kd = kd + 0.1;
	p = static_cast<int32_t>(std::roundf(kp * (1 << 16)));
	i = static_cast<int32_t>(std::roundf(ki * (1 << 16)));
	d = static_cast<int32_t>(std::roundf(kd * (1 << 16)));
	zynq.set_pid_gain(p, i, d, bias,  true);
	zynq.set_pid_gain(p, i, d, bias, false);
	break;
      case 'D':
	kd = std::max(kd - 0.1, 0.0);
	p = static_cast<int32_t>(std::roundf(kp * (1 << 16)));
	i = static_cast<int32_t>(std::roundf(ki * (1 << 16)));
	d = static_cast<int32_t>(std::roundf(kd * (1 << 16)));
	zynq.set_pid_gain(p, i, d, bias,  true);
	zynq.set_pid_gain(p, i, d, bias, false);
	break;
      }
      info(key);
      if (key == 'q') break;
    }
  }
  std::cout << "\033[0m\033[7B" << std::endl;
  kill = true;
  t.join(); s.join();

  do {
    std::cout << "Save PID gains? [y/n]: ";
    while (!kbhit());
    key = getchar();
    std::cout << key << std::endl;
  } while (key != 'y' && key != 'n');
  
  if (key == 'y') {
    fp = fopen("gain.log", "w");
    if (fp == nullptr) {
      std::cerr << "[ERROR] file open failed: gain.log" << std::endl;
      exit(1);
    } else {
      fprintf(fp, "%lf %lf %lf %lf\n", kp, ki, kd, bias);
      fclose(fp);
    }  
  }
  
  return 0;
}

void info(const char key)
{
  const std::string k = (key == 'k')? EMPH('k'): "k";
  const std::string j = (key == 'j')? EMPH('j'): "j";
  const std::string h = (key == 'h')? EMPH('h'): "h";
  const std::string l = (key == 'l')? EMPH('l'): "l";
  const std::string m = (key == 'm')? EMPH('m'): "m";
  const std::string n = (key == 'n')? EMPH('n'): "n";
  const std::string b = (key == 'b')? EMPH('b'): "b";
  const std::string q = (key == 'q')? EMPH('q'): "q";
  const std::string p = (key == 'p')? EMPH('p'): "p";
  const std::string i = (key == 'i')? EMPH('i'): "i";
  const std::string d = (key == 'd')? EMPH('d'): "d";
  const std::string P = (key == 'P')? EMPH('P'): "P";
  const std::string I = (key == 'I')? EMPH('I'): "I";
  const std::string D = (key == 'D')? EMPH('D'): "D";
  printf("\r\033[32m"); // set color green 
  printf("drive:                                             \n");
  printf("           [%s]                                    \n", k.c_str());
  printf("            |         set PID gains:               \n");
  printf("       [%s]--+--[%s]      Kp [%s/%s] value: %.1lf  \n", h.c_str(), l.c_str(), p.c_str(), P.c_str(), kp);
  printf("            |           Ki [%s/%s] value: %.1lf    \n", i.c_str(), I.c_str(), ki);
  printf("           [%s]          Kd [%s/%s] value: %.1lf   \n", j.c_str(), d.c_str(), D.c_str(), kd);
  printf("max: [%s]  nuetral: [%s]  brake: [%s]  quit: [%s]  \n", m.c_str(), n.c_str(), b.c_str(), q.c_str());
  printf("\r\033[7A"); // back console cursor
  fflush(stdout);
}

void loop(slab::System& zynq)
{
  while (!kill) {
    right_rotation = zynq.recv_rotation(true);
    left_rotation  = zynq.recv_rotation(false);
  }
}
