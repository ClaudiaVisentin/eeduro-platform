#include "MainSequenceFaulhaber_client.hpp"
#include "../../safety/client/DeltaSafetyProperties_client.hpp"
#include "../../control/types.hpp"
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

MainSequenceFaulhaber_client::MainSequenceFaulhaber_client(Sequencer* sequencer, ControlSystem_client* controlSys, SafetySystem* safetySys) :
	Sequence<void>("main", sequencer),
	moveBlock(sequencer, controlSys, safetySys),
	sort(sequencer, controlSys, safetySys),
	controlSys(controlSys),
	safetySys(safetySys) {
	// nothing to do
}

void MainSequenceFaulhaber_client::run() {
	const AxisVector start_position{ 0, 0, -0.015, 0 };
	controlSys->pathPlanner.setInitPos(start_position);
	
	log.trace() << "Sequencer '" << name << "': started.";
	yield();
	log.trace() << "Sequencer '" << name << "': waiting until robot is ready...";
	while(safetySys->getCurrentLevel().getId() != systemReady) {
		yield();
	}

	double time;
	bool standing; bool sorted = false;
	AxisVector enc, enc_prev, diff;
	
	int index = controlSys->inputSwitch.getCurrentInput(); // mouse output
	AxisVector speed; speed << 1, 1, 1, 5;
	AxisVector acc; acc << 10, 10, 10, 50;
	double speedup_factor = 1.0;
	
	while (true) {
		if (index == 1) {
			log.trace() << "Press the blue button switch to predifined path";
		} else if (index == 0 /*|| index == 2*/) {
			log.trace() << "Press the blue button switch to mouse input";
		} 
		
		usleep(200000);
		controlSys->board.button_latch[0].reset();
		
		if(index == 1) { // MOUSE
			// wait for button signal or timeout
			while(controlSys->board.button_latch[0].get() != true) {
				usleep(100000);
				
				// define if standing
				enc = controlSys->board.getPosOut().getSignal().getValue();
				for(int i = 0; i < enc.size(); i++){
					diff[i] = fabs(enc[i] - enc_prev[i]);
				}
				if(diff[0] < 0.01 && diff[1] < 0.01 && diff[2] < 0.01 && diff[3] < 2.0) standing = true;
				else standing = false;
				
				if(standing ) time = time + 0.1;
				else time = 0.0;
				
				// define if timed out
				if(time > 3.0) {
					time = 0.0;
					break;
				}
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
			index = 1;
		}
		
		if (index == 0) {
			log.trace() << "sort";
			controlSys->pathPlanner.setInitPos(controlSys->inputSwitch.getOut().getSignal().getValue());
// 			controlSys->pathPlanner.setMaxSpeed(speed * speedup_factor);
// 			controlSys->pathPlanner.setMaxAcc(acc * speedup_factor);
 			controlSys->inputSwitch.switchToInput(0);
			controlSys->pathPlanner.gotoPoint(start_position);
			while(!controlSys->pathPlanner.posReached()) {
				usleep(100000);
				yield();
			}
			sort();
		}
		if (index == 1) {
			log.trace() << "switching to mouse input";
			controlSys->pathPlanner.setInitPos(controlSys->inputSwitch.getOut().getSignal().getValue());
			controlSys->mouse.setInitPos(start_position); 
			controlSys->joystick.setInitPos(start_position); 
			controlSys->vel2posInputs.setInitPos(start_position);
			
			controlSys->inputSwitch.switchToInput(0);
			controlSys->pathPlanner.gotoPoint(start_position);
			while(!controlSys->pathPlanner.posReached()) {
				usleep(100000);
				yield();
			}
			controlSys->mouse.setInitPos(controlSys->inputSwitch.getOut().getSignal().getValue()); 
			controlSys->joystick.setInitPos(controlSys->inputSwitch.getOut().getSignal().getValue()); 
			controlSys->vel2posInputs.setInitPos(controlSys->inputSwitch.getOut().getSignal().getValue());
			controlSys->inputSwitch.switchToInput(1);
		}
	}
	
	log.trace() << "Sequencer '" << name << "': finished.";
}


