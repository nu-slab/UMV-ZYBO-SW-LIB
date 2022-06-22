#!/bin/python3

import slab

# keyboard hit
import kbhit
kbhit.atexit.register(kbhit.set_normal_term)
kbhit.set_curses_term()

fpga = slab.System("/dev/uio0")

ACCEL_MAX :int=  127 
ACCEL_MIN :int= -127 
ACCEL_INC :int=   16 
STEER_MAX :int=    7
STEER_MIN :int=   -7
STEER_INC :int=    1

accel :int= 0
steer :int= 0

loop  :bool= True

while loop:
    if kbhit.kbhit():
        key = kbhit.getch()

        if   key == 'h':
            steer = STEER_MIN if (steer < STEER_MIN + STEER_INC) else steer - STEER_INC
        elif key == 'j':
            accel = ACCEL_MIN if (accel < ACCEL_MIN + ACCEL_INC) else accel - ACCEL_INC
        elif key == 'k':
            accel = ACCEL_MAX if (accel > ACCEL_MAX - ACCEL_INC) else accel + ACCEL_INC
        elif key == 'l':
            steer = STEER_MAX if (steer > STEER_MAX - STEER_INC) else steer + STEER_INC
        elif key == 'r':
            accel = 0
            steer = 0

        elif key == 'q':
            accel = 0
            steer = 0
            loop :bool= False

        fpga.send_accel(accel)
        fpga.send_steer(steer)

    print ('\rAccel: %d, Steer: %d, Rotation_f: %d, Rotation_r: %d   ' %(accel, steer, fpga.recv_rotation(True), fpga.recv_rotation(False)), end='')


