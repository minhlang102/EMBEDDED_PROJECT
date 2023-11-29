"""Implementation of software timer"""

import extern
from threading import Timer

TIMER_CYCLE = 1000	#ms

def setTimer0(duration):
	"""Timer used for main fsm"""
	extern.timer0_counter = duration/TIMER_CYCLE
	extern.timer0_flag = 0

def setTimer1(duration):
	"""Timer used for ack timeout of sensor data"""
	extern.timer1_counter = duration/TIMER_CYCLE
	extern.timer1_flag = 0

def setTimer2(duration):
	"""Timer used for ack timeout of button 1"""
	extern.timer2_counter = duration/TIMER_CYCLE
	extern.timer2_flag = 0

def setTimer3(duration):
	"""Timer used for ack timeout of button 2"""
	extern.timer3_counter = duration/TIMER_CYCLE
	extern.timer3_flag = 0

def setTimer4(duration):
	extern.timer4_counter = duration/TIMER_CYCLE
	extern.timer4_flag = 0

def timer_run():
	if extern.timer0_counter > 0:
		extern.timer0_counter -= 1
		if extern.timer0_counter == 0: extern.timer0_flag = 1

	if extern.timer1_counter > 0:
		extern.timer1_counter -= 1
		if extern.timer1_counter == 0: extern.timer1_flag = 1
	
	if extern.timer2_counter > 0:
		extern.timer2_counter -= 1
		if extern.timer2_counter == 0: extern.timer2_flag = 1

	if extern.timer3_counter > 0:
		extern.timer3_counter -= 1
		if extern.timer3_counter == 0: extern.timer3_flag = 1

	if extern.timer4_counter > 0:
		extern.timer4_counter -= 1
		if extern.timer4_counter == 0: extern.timer4_flag = 1
