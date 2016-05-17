#include <eeros/hal/HAL.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include <eeros/sequencer/Sequencer.hpp>
#include <eeros/logger/Logger.hpp>
#include <eeros/logger/StreamLogWriter.hpp>
#include <eeros/logger/SysLogWriter.hpp>

#include "control/server/ControlSystem_server.hpp"
#include "safety/server/DeltaSafetyProperties_server.hpp"
#include "sequencer/server/MainSequenceFaulhaber_server.hpp"
#include "socket/Server.hpp"

#include <iostream>
#include <unistd.h>
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
	
	if(argc < 2) {
		fprintf(stderr, "usage: %s portno \n", argv[0]);
		exit(0);
	}
	
	// Logger
	StreamLogWriter w(std::cout);
	w.show();
	Logger<LogWriter>::setDefaultWriter(&w);
	Logger<LogWriter> log('M');
	
	log.trace() << "Application eeduro-delta started...";
	
	// Create server thread (Activate only if working with another robot)
	Server *serverThread = new Server();
// 	while(!serverThread->isRunning()) usleep(100000);
	
	// Create control system
	ControlSystem_server controlSystem(serverThread); 
	
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
	DeltaSafetyProperties_server safetyProperties(&controlSystem);
	SafetySystem safetySys(safetyProperties, dt);
	
	// Create sequencer
	Sequencer sequencer;
	
	MainSequenceFaulhaber_server mainSequence(&sequencer, &controlSystem, &safetySys);
	sequencer.start(&mainSequence);
	
	while(running && sequencer.getState() != state::terminated) {
		usleep(1000000);
	}
	
	std::cout << "Stopping threads" << std::endl;
	controlSystem.stop();
	serverThread->stop();
	
	return 0;

}
