#include <eeros/hal/HAL.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include <eeros/logger/Logger.hpp>
#include <eeros/logger/StreamLogWriter.hpp>
#include <eeros/logger/SysLogWriter.hpp>
#include <eeros/sequencer/Sequencer.hpp>

#include "control/client/ControlSystem_client.hpp"
#include "safety/client/DeltaSafetyProperties_client.hpp"
#include "sequencer/client/MainSequence_client.hpp"
#include "socket/Client.hpp"
#include <iostream>
#include <signal.h>

using namespace eeduro;
using namespace eeduro::delta;
using namespace eeros;
using namespace eeros::logger;
using namespace eeros::hal;
using namespace eeros::safety;
using namespace eeros::sequencer;

volatile bool running = true;

void signalHandler(int signum) {
	running = false;
}

int main(int argc, char* argv[]) {
	signal(SIGINT, signalHandler);
	
	if(argc < 3) {
		fprintf(stderr, "usage: %s hostname portno \n", argv[0]);
		exit(0);
	}
	
	// Logger
	StreamLogWriter w(std::cout);
	w.show();
	Logger<LogWriter>::setDefaultWriter(&w);
	Logger<LogWriter> log('M');
	
	log.trace() << "Application eeduro-delta started...";
	
	// Create client thread
	Client *clientThread = new Client(argv[0], argv[1]);
// 	while(!clientThread->isRunning()) usleep(100000);
	
	// Create control system
	ControlSystem_client controlSystem(clientThread); 
	
	// initialize hardware
	controlSystem.initBoard();
	
	controlSystem.mouse.j.on_button([&controlSystem](int x, bool value) {
		if (x == BTN_LEFT || x == BTN_RIGHT) {
			controlSystem.board.power_out[0] = value;
			controlSystem.board.power_out[1] = value;
		}
	});
	
	controlSystem.joystick.on_button([&controlSystem](int x, bool value) {
		if (x == 4 || x == 5) { // LB and RB
			controlSystem.board.power_out[0] = value;
			controlSystem.board.power_out[1] = value;
		}
	});
	
	// Create safety system
	DeltaSafetyProperties_client safetyProperties(&controlSystem);
	SafetySystem safetySys(safetyProperties, dt);
	
	// Create sequencer
	Sequencer sequencer;
	
	MainSequence_client mainSequence(&sequencer, &controlSystem, &safetySys);
	sequencer.start(&mainSequence);
	
	while(running && sequencer.getState() != state::terminated) {
		usleep(1000000);
	}
	
	std::cout << "Stopping threads" << std::endl;
	controlSystem.stop();
	clientThread->stop();
	
	return 0;
}