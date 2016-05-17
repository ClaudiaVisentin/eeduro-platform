#include "MoveBlockSequence_server.hpp"
#include <unistd.h>

using namespace eeduro::delta;
using namespace eeros::sequencer;
using namespace eeros::safety;


MoveBlockSequence_server::MoveBlockSequence_server(Sequencer* sequencer, ControlSystem_server* controlSys, SafetySystem* safetySys) :
	Sequence<void, int, int>("moveBlock", sequencer),
	controlSys(controlSys),
	safetySys(safetySys),
	position(0)
{
	calibration.loadDefaults();
	if (!calibration.load()) {
		log.warn() << "could not load calibration";
	}
}

void MoveBlockSequence_server::run(int from, int to) {
	up();
	move(from);
	down();
	grab();
	run(to);
}

void MoveBlockSequence_server::run(int to) {
	up();
	move(to);
	down();
	release();
	up();
}

void MoveBlockSequence_server::up() {
	log.trace() << "move to transportation height";
	
	eeros::math::Vector<4> torqueLimit{ q012gearTorqueLimit, q012gearTorqueLimit, q012gearTorqueLimit, q3gearTorqueLimit };
	controlSys->torqueLimitation.setLimit(-torqueLimit, torqueLimit);
	auto p = controlSys->pathPlanner.getLastPoint();
	
	p[2] = calibration.transportation_height;
	controlSys->pathPlanner.gotoPoint(p);
	waitUntilPointReached();
}

void MoveBlockSequence_server::down() {
	log.trace() << "move down";
	
// 	double down = calibration.position[position].zblock[1] + 0.001;
// 	double touch = calibration.position[position].zblock[3];
	double down = calibration.position[position].zblock[1] + 0.001;
	double touch = calibration.position[position].zblock[3] - 0.001;
	
	eeros::math::Vector<4> torqueLimit{ q012gearTorqueLimit, q012gearTorqueLimit, q012gearTorqueLimit, q3gearTorqueLimit };
// 	eeros::math::Vector<4> torqueLimitDown = torqueLimit * 0.1;
	eeros::math::Vector<4> torqueLimitDown = torqueLimit * 0.4;
	
	auto p = controlSys->pathPlanner.getLastPoint();
	
	p[2] = down;
	controlSys->pathPlanner.gotoPoint(p);
	waitUntilPointReached();
	
	controlSys->torqueLimitation.setLimit(-torqueLimitDown, torqueLimitDown);
	
	p[2] = touch;
	controlSys->pathPlanner.gotoPoint(p);
	waitUntilPointReached();
}

void MoveBlockSequence_server::grab() {
	log.trace() << "grab block";
	controlSys->board.power_out[0] = true;
	controlSys->board.power_out[1] = true;
}

void MoveBlockSequence_server::release() {
	log.trace() << "release block";
	controlSys->board.power_out[0] = false;
	controlSys->board.power_out[1] = false;
}

void MoveBlockSequence_server::move(int position) {
	log.trace() << "move to position " << position;
	auto p = controlSys->pathPlanner.getLastPoint();
	p[0] = calibration.position[position].x;
	p[1] = calibration.position[position].y;
	if (p[3] > 1) {
		p[3] = calibration.position[position].r;
	}
	else {
		p[3] = calibration.position[position].r + pi / 2.0;
	}
	controlSys->pathPlanner.gotoPoint(p);
	waitUntilPointReached();
}

void MoveBlockSequence_server::waitUntilPointReached() {
	while (!controlSys->pathPlanner.posReached()) {
		usleep(100000);
		yield();
	}
}


