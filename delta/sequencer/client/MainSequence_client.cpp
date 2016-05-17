#include "MainSequence_client.hpp"
#include "../../safety/client/DeltaSafetyProperties_client.hpp"
#include <unistd.h>

#include <queue>

using namespace eeduro::delta;
using namespace eeros::sequencer;
using namespace eeros::safety;

enum {
	idle,
	move_from_to,
	move_to
};

MainSequence_client::MainSequence_client(Sequencer* sequencer, ControlSystem_client* controlSys, SafetySystem* safetySys) :
	Sequence<void>("main", sequencer),
	moveBlock(sequencer, controlSys, safetySys),
	sort(sequencer, controlSys, safetySys),
	controlSys(controlSys),
	safetySys(safetySys) {
	// nothing to do
}

void MainSequence_client::run() {
	const AxisVector start_position{ 0, 0, -0.015, 0 };
	
	controlSys->pathPlanner.setInitPos(start_position);
	
	log.trace() << "Sequencer '" << name << "': started.";
	yield();
	log.trace() << "Sequencer '" << name << "': waiting until robot is ready...";
	while(safetySys->getCurrentLevel().getId() != systemReady) {
		yield();
	}
	
	while (true) {
		int index = controlSys->inputSwitch.getCurrentInput();
		if (index == 0) {
			log.trace() << "Press the blue button switch to mouse input";
		} else if (index == 1) {
			log.trace() << "Press the blue button switch to joystick input";
		} else if (index == 2) {
			log.trace() << "Press the blue button switch to predifined path";
		}

		usleep(200000);
		controlSys->board.button_latch[0].reset();
		while(controlSys->board.button_latch[0].get() != true) {
			usleep(100000);
			yield();
		}
		
		index++;
		if (index > 2) index = 0;
// 		index++;
// 		if(index > 1) index = 0;
		
		if (index == 0) {
			log.trace() << "switching to predifined path";
			
			controlSys->pathPlanner.setInitPos(controlSys->inputSwitch.getOut().getSignal().getValue());
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
			controlSys->inputSwitch.switchToInput(0);
			controlSys->pathPlanner.gotoPoint(start_position);
			while(!controlSys->pathPlanner.posReached()) {
				usleep(100000);
				yield();
			}
// 			controlSys->mouse.setInitPos(start_position);
			controlSys->inputSwitch.switchToInput(1);
			}
		if (index == 2) {
			log.trace() << "switching to joystick input";
			controlSys->pathPlanner.setInitPos(controlSys->inputSwitch.getOut().getSignal().getValue());
			controlSys->inputSwitch.switchToInput(0);
			controlSys->pathPlanner.gotoPoint(start_position);
			while(!controlSys->pathPlanner.posReached()) {
				usleep(100000);
				yield();
			}
// 			controlSys->joystick.setInitPos(start_position);
			controlSys->inputSwitch.switchToInput(2);
		}
	}
	
	log.trace() << "Sequencer '" << name << "': finished.";
}


