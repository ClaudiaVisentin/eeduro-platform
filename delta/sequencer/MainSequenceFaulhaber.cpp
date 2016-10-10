#include "MainSequenceFaulhaber.hpp"
#include "../safety/DeltaSafetyProperties.hpp"
#include "../control/types.hpp"
#include <unistd.h>
#include <iostream>

#include <queue>

using namespace eeduro::delta;
using namespace eeros::sequencer;
using namespace eeros::safety;

enum {
	idle,
	move_from_to,
	move_to
};

MainSequenceFaulhaber::MainSequenceFaulhaber(Sequencer* sequencer, ControlSystem* controlSys, SafetySystem* safetySys) :
	Sequence<void>("main", sequencer),
	moveBlock(sequencer, controlSys, safetySys),
	sort(sequencer, controlSys, safetySys),
	controlSys(controlSys),
	safetySys(safetySys) {
	// nothing to do
}

void MainSequenceFaulhaber::run() {
	const AxisVector start_position{ 0, 0, -0.015, 0 };
	controlSys->pathPlanner.setInitPos(start_position);
	
	log.trace() << "Sequencer '" << name << "': started.";
	yield();
	log.trace() << "Sequencer '" << name << "': waiting until robot is ready...";
	
	// Automatic init
	while(safetySys->getCurrentLevel().getId() < systemOn) {
		yield();
	}
	safetySys->triggerEvent(doPoweringUp);
	// Wait initialization complete
	while(safetySys->getCurrentLevel().getId() < systemReady) {
		yield();
	}
	
	double time;
	bool standing; bool sorted = false; bool mouse_standing; bool joyst_standing;
	AxisVector enc, enc_prev, enc_diff;
	
	int index = controlSys->inputSwitch.getCurrentInput(); // mouse output
	
	AxisVector speed; speed << 1, 1, 1, 5;
	AxisVector acc; acc << 10, 10, 10, 50;
	double speedup_factor = 1.0;

	int count_sort = 0;
	
	while (true) {
		usleep(200000);
		controlSys->board.button_latch[0].reset();
		
		if(index == 1) { // MOUSE or JOYSTICK
			// wait for blue button signal or timeout
			while(controlSys->board.button_latch[0].get() != true) {
				usleep(100000);
				
				// define if standing
				enc = controlSys->board.getPosOut().getSignal().getValue();
				for(int i = 0; i < enc.size(); i++){
					enc_diff[i] = fabs(enc[i] - enc_prev[i]);
				}
				if(enc_diff[0] < 0.01 && enc_diff[1] < 0.01 && enc_diff[2] < 0.01 && enc_diff[3] < 2.0) standing = true;
				else standing = false;
				
				if(standing ) time = time + 0.1;
				else time = 0.0;
				
				// define if timed out
				if(time > 5.0 /*3.0*/) {
					time = 0.0;
					break;
				}
				
				// set prev variables
				enc_prev = enc;
				
				yield();
			}
			// set index
			index = 0;
				
			speedup_factor = speedup_factor + 2.0;
			if(speedup_factor > 6.0)
				speedup_factor = 1.0;
		}
		else {
			// if finished with sorting, then go automatically to device mode (joystick or mouse) -> index = 1
			index = 1;
		}
		
		if (index == 0) {
			// go to ready position
			controlSys->pathPlanner.setInitPos(controlSys->inputSwitch.getOut().getSignal().getValue());
// 			controlSys->pathPlanner.setMaxSpeed(speed * speedup_factor);
// 			controlSys->pathPlanner.setMaxAcc(acc * speedup_factor);
 			controlSys->inputSwitch.switchToInput(0);
			controlSys->pathPlanner.gotoPoint(start_position);
			while(!controlSys->pathPlanner.posReached()) {
				usleep(100000);
				yield();
			}
			
			std::cout << "count: " << count_sort << std::endl;
			
			// check count sorting
			if(count_sort == 4){
				// initialize
				safetySys->triggerEvent(doParking);
				while(safetySys->getCurrentLevel().getId() != systemReady) {
					usleep(100000);
					yield();
				}
				count_sort = 0;
			}
			else{
				// sort
				sort();
				count_sort++;
			}
		}
		if (index == 1) {
			log.trace() << "switching to mouse input";
			controlSys->mouse.setInitPos(start_position); 
			controlSys->joystick.setInitPos(start_position); 
			controlSys->vel2posInputs.setInitPos(start_position); 

			controlSys->pathPlanner.setInitPos(controlSys->inputSwitch.getOut().getSignal().getValue());
 			
			controlSys->inputSwitch.switchToInput(0);
			controlSys->pathPlanner.gotoPoint(start_position);
			while(!controlSys->pathPlanner.posReached()) {
				usleep(100000);
				yield();
			}
			controlSys->mouse.setInitPos(controlSys->inputSwitch.getOut().getSignal().getValue()); 
			
			// TODO only Marketing & Robotic Robot
			controlSys->joystick.setInitPos(controlSys->inputSwitch.getOut().getSignal().getValue()); 
 			controlSys->vel2posInputs.setInitPos(controlSys->inputSwitch.getOut().getSignal().getValue()); 

			controlSys->inputSwitch.switchToInput(1);
		}
	}
	log.trace() << "Sequencer '" << name << "': finished.";
}


