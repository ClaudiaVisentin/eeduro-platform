#include "CalibrateSequence_client.hpp"
#include "../../safety/client/DeltaSafetyProperties_client.hpp"
#include <eeros/core/EEROSException.hpp>
#include <unistd.h>

using namespace eeduro::delta;
using namespace eeros::sequencer;
using namespace eeros::safety;


CalibrateSequence_client::CalibrateSequence_client(Sequencer* sequencer, ControlSystem_client* controlSys, SafetySystem* safetySys) :
	Sequence<void>("main", sequencer),
	controlSys(controlSys),
	safetySys(safetySys)
{
	
}

void CalibrateSequence_client::waitUntilReady() {
	while(safetySys->getCurrentLevel().getId() != systemReady) {
		usleep(100000);
		yield();
	}
}

void CalibrateSequence_client::waitForButton(std::vector<int> buttons) {
	for (auto i: buttons) {
		if (i < 0 || i > 3)
			throw EEROSException("index out of range");
	}
	usleep(200000);
	for (auto i: buttons) controlSys->board.button_latch[i].reset();
	while(true) {
		for (auto i: buttons) {
			if (controlSys->board.button_latch[i].get())
				return;
		}
		usleep(100000);
		yield();
	}
}

void CalibrateSequence_client::waitForGreenButton()	 { waitForButton({2}); }
void CalibrateSequence_client::waitForRedButton()		 { waitForButton({1}); }
void CalibrateSequence_client::waitForBlueButton()		 { waitForButton({0}); }
void CalibrateSequence_client::waitForBlueOrRedButton() { waitForButton({ 0, 1 }); }

void CalibrateSequence_client::run() {
	waitUntilReady();
	
	usleep(500000);
	
	//safetySys->triggerEvent(events::doEmergency);
	controlSys->disableAxis();
	calibration.loadDefaults();
	
	const char *block[] = { "[no block]", "[block 1]", "[block 2]", "[block 3]" };

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			log.trace() << block[i] << " move TCP to position " << j << " and press the blue button";
			waitForBlueButton();
			
			auto p = controlSys->directKin.getOut().getSignal().getValue();
			calibration.position[j].zblock[i] = p[2];
			if (i == 3) {
				calibration.position[j].x = p[0];
				calibration.position[j].y = p[1];
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		calibration.position[i].level12 = (calibration.position[i].zblock[1] + calibration.position[i].zblock[2]) / 2.0;
		calibration.position[i].level23 = (calibration.position[i].zblock[2] + calibration.position[i].zblock[3]) / 2.0;
		calibration.position[i].level30 = (calibration.position[i].zblock[3] + calibration.position[i].zblock[0]) / 2.0;
	}

	bool good = true;
	for (int i = 0; i < 4; i++) {
		double z0 = calibration.position[i].zblock[0];
		double z1 = calibration.position[i].zblock[1];
		double z2 = calibration.position[i].zblock[2];
		double z3 = calibration.position[i].zblock[3];
		
		if (z0 >= z3) {
			log.error() << "[position " << i << "] invalid calibration zblock0 >= zblock3";
			good = false;
		}
		if (z3 >= z2) {
			log.error() << "[position " << i << "] invalid calibration zblock3 >= zblock2";
			good = false;
		}
		if (z2 >= z1) {
			log.error() << "[position " << i << "] invalid calibration zblock2 >= zblock1";
			good = false;
		}
		if (z1 <= calibration.position[i].level12) {
			log.error() << "[position " << i << "] invalid calibration zblock1 <= level12";
			good = false;
		}
		if (z2 >= calibration.position[i].level12) {
			log.error() << "[position " << i << "] invalid calibration zblock2 >= level12";
			good = false;
		}
		if (z2 <= calibration.position[i].level23) {
			log.error() << "[position " << i << "] invalid calibration zblock2 <= level23";
			good = false;
		}
		if (z3 >= calibration.position[i].level23) {
			log.error() << "[position " << i << "] invalid calibration zblock3 >= level23";
			good = false;
		}
		if (z3 <= calibration.position[i].level30) {
			log.error() << "[position " << i << "] invalid calibration zblock3 <= level30";
			good = false;
		}
		if (z0 >= calibration.position[i].level30) {
			log.error() << "[position " << i << "] invalid calibration zblock0 >= level30";
			good = false;
		}
	}
	
	if (!good) {
		return;
	}
	if (calibration.save()) {
		log.info() << "calibration saved";
	}
	else {
		log.error() << "calibration could not be saved";
	}
}
