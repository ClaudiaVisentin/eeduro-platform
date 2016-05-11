#include <eeros/hal/HAL.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include <eeros/logger/Logger.hpp>
#include <eeros/logger/StreamLogWriter.hpp>
#include <eeros/logger/SysLogWriter.hpp>
#include <eeros/sequencer/Sequencer.hpp>

#include "control/ControlSystem_client.hpp"
#include "safety/DeltaSafetyProperties.hpp"
#include "sequencer/MoveBlockSequence.hpp"
#include "sequencer/MainSequenceFaulhaber.hpp"
#include "socket/Client.hpp"
#include <iostream>

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

	if(argc < 3) {
		fprintf(stderr, "usage: %s hostname portno \n", argv[0]);
		exit(0);
	}
	
	// Create client thread
	Client *clientThread = new Client(argv[0], argv[1]);
	while(!clientThread->isRunning()) usleep(100000);
	
	// Create control system
	ControlSystem_client controlSystem(clientThread); 
	
	std::cout << "Starting control system" << std::endl;
	controlSystem.start();
	
	while (running) {
		std::cout << "msg received: " << controlSystem.getAxisPos() << std::endl;
		std::cout << "msg sent: "     << controlSystem.getTcpPos() << std::endl;
		usleep(100000);
	}
	
	std::cout << "Stopping threads" << std::endl;
	controlSystem.stop();
	clientThread->stop();
	
	return 0;
}
