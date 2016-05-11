#include <eeros/hal/HAL.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include <eeros/logger/Logger.hpp>
#include <eeros/logger/StreamLogWriter.hpp>
#include <eeros/logger/SysLogWriter.hpp>
#include <eeros/sequencer/Sequencer.hpp>

#include "control/ControlSystem_server.hpp"
#include "safety/DeltaSafetyProperties.hpp"
#include "sequencer/MoveBlockSequence.hpp"
#include "sequencer/MainSequenceFaulhaber.hpp"
#include "socket/Server.hpp"
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
	
	if(argc < 2) {
		fprintf(stderr, "usage: %s portno \n", argv[0]);
		exit(0);
	}
	
	// Create server thread
	Server *serverThread = new Server();
	while(!serverThread->isRunning()) usleep(100000);
	
	// Create control system
	ControlSystem_server controlSystem(serverThread); 
	
	std::cout << "starting control system" << std::endl;
	controlSystem.start();
	
	while (running) {
		std::cout << "msg received: " << controlSystem.getAxisPos() << std::endl;
		std::cout << "msg sent: "     << controlSystem.getTcpPos() << std::endl;
		usleep(100000);
	}
	
	std::cout << "stopping threads" << std::endl;
	controlSystem.stop();
	serverThread->stop();
	
	return 0;

}
