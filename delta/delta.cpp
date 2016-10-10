#include "control/ControlSystem.hpp"
#include "safety/DeltaSafetyProperties.hpp"
#include "sequencer/MoveBlockSequence.hpp"
#include "sequencer/MainSequence.hpp"
#include "sequencer/MainSequenceFaulhaber.hpp"
#include "sequencer/CalibrateSequence.hpp"
#include "socket/Client.hpp"

#include <eeros/hal/HAL.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include <eeros/logger/Logger.hpp>
#include <eeros/logger/StreamLogWriter.hpp>
#include <eeros/logger/SysLogWriter.hpp>
#include <eeros/sequencer/Sequencer.hpp>

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <math.h>

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
	
	StreamLogWriter w(std::cout);
//	SysLogWriter s("delta");
	w.show();
//	s.show();
	
	
// 	Logger<LogWriter>::setDefaultWriter(&s);
	
	
	std::cout << "Application eeduro-delta started..." << std::endl;
	
	// create control system
	ControlSystem controlSys;
	
	// initialize hardware
	controlSys.initBoard();
	
	controlSys.mouse.j.on_button([&controlSys](int x, bool value) {
		if (x == BTN_LEFT || x == BTN_RIGHT) {
			controlSys.board.power_out[0] = value;
			controlSys.board.power_out[1] = value;
		}
	});
	
	controlSys.joystick.on_button([&controlSys](int x, bool value) {
		if (x == 4 || x == 5) { // LB and RB
			controlSys.board.power_out[0] = value;
			controlSys.board.power_out[1] = value;
		}
	});
	
	// create safety system
	DeltaSafetyProperties safetyProperties(&controlSys);
	SafetySystem safetySys(safetyProperties, dt);
	
	Logger<LogWriter>::setDefaultWriter(&w);
	Logger<LogWriter> log('M');

	// create sequencer
	Sequencer sequencer;
	
	while(safetySys.getCurrentLevel().getId() != systemReady) {
		usleep(100000);
	}
	
	if (argc >= 2) {
		if (std::string("calibrate") == argv[1]) {
			std::cout << "here" << std::endl;
			CalibrateSequence calibrate(&sequencer, &controlSys, &safetySys);
			sequencer.start(&calibrate);
			sequencer.join();
		}
		else {
			log.fatal() << "unknown arguments";
		}
	}
	else {
		MainSequence mainSequence(&sequencer, &controlSys, &safetySys);  // TODO standard program
// 		MainSequenceFaulhaber mainSequence(&sequencer, &controlSys, &safetySys);
		sequencer.start(&mainSequence);
		
		while(running && sequencer.getState() != state::terminated) {
			usleep(1000000);
			/*log.trace() << controlSys.directKin.getOut().getSignal().getValue()[0] << "   "
						<< controlSys.directKin.getOut().getSignal().getValue()[1] << "   "
						<< controlSys.directKin.getOut().getSignal().getValue()[2] << "   "
						<< controlSys.directKin.getOut().getSignal().getValue()[3];*/
			
// 			eeros::math::Vector4 out = controlSys.vel2posInputs.getPositionOut().getSignal().getValue();
// 			log.info() << out(0) << "; " << out(1) << "; " << out(2) << "; " << out(3);
			
		}
	}
	
	log.info() << "Shuting down...";
	
	sequencer.shutdown();
	
	safetySys.triggerEvent(doParking);
	safetySys.shutdown();
	
	controlSys.disableAxis();
	controlSys.stop();
	
	sleep(1);
	
	return 0;
}
