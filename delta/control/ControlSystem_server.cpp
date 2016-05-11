#include "ControlSystem_server.hpp"
#include "../safety/DeltaSafetyProperties.hpp"

#include <eeros/core/EEROSException.hpp>
#include <unistd.h>

#include <iostream>

using namespace eeduro::delta;
using namespace eeros;
using namespace eeros::control;

ControlSystem_server::ControlSystem_server(Server* serverThread) :

	serverData(serverThread),
	i(i1524, i1524, i1524, i0816),
	kM(kM1524, kM1524, kM1524, kM0816),
	RA(RA1524, RA1524, RA1524, RA0816),
	
	jacobian(kinematic.get_offset()),
	
	homed(false),
	
	timedomain("Main time domain", dt, true) {

	serverData.getIn().connect(board.getPosOut());
// 	getOut());
// 	vel2posInputs.getMouseIn().connect(derMouse.getOut());        
// 	vel2posInputs.getJoystickIn().connect(derJoystick.getOut());  
// 	inputSwitch.getIn(1).connect(vel2posInputs.getPositionOut()); 
// 	
// 	posSum.getIn(0).connect(inputSwitch.getOut());
// 	posSum.getIn(1).connect(directKin.getOut());
// 	posController.getIn().connect(posSum.getOut());
// 	posDiff.getIn().connect(directKin.getOut());
// 	speedSum.getIn(0).connect(posController.getOut());
// 	speedSum.getIn(1).connect(posDiff.getOut());
// // 	speedSum.getIn(2).connect(pathPlanner.getVelOut());
// 	speedLimitation.getIn().connect(speedSum.getOut());
// 	speedController.getIn().connect(speedLimitation.getOut());
// 	accSum.getIn(0).connect(speedController.getOut());
// // 	accSum.getIn(1).connect(spathPlanner.getAccOut());
// 	inertia.getAccelerationInput().connect(accSum.getOut());
// 	inertia.getJointPosInput().connect(angleGear.getOut());
// 	inertia.getTcpPosInput().connect(directKin.getOut());
// 	forceLimitation.getIn().connect(inertia.getOut());
// 	jacobi.getForceInput().connect(forceLimitation.getOut());
// 	jacobi.getJointPosInput().connect(angleGear.getOut());
// 	jacobi.getTcpPosInput().connect(directKin.getOut());
// 	torqueLimitation.getIn().connect(jacobi.getOut());
// 	torqueGear.getIn().connect(torqueLimitation.getOut());
// 	angleGear.getIn().connect(board.getPosOut());
// 	motorModel.getTorqueIn().connect(torqueGear.getOut());
// 	motorModel.getSpeedIn().connect(board.getSpeedOut());
// 	voltageSwitch.getIn(0).connect(motorModel.getOut());
// 	voltageSwitch.getIn(1).connect(voltageSetPoint.getOut());
// 	directKin.getIn().connect(angleGear.getOut());
	
	timedomain.addBlock(&board);
	timedomain.addBlock(&serverData);
// 	timedomain.addBlock(&joystick);
// 	timedomain.addBlock(&mouse);
// 	timedomain.addBlock(&derJoystick);
// 	timedomain.addBlock(&derMouse);
// 	timedomain.addBlock(&vel2posInputs); 
// 	timedomain.addBlock(&pathPlanner);
// 	timedomain.addBlock(&inputSwitch);
// 	timedomain.addBlock(&board);
// 	timedomain.addBlock(&angleGear);
// 	timedomain.addBlock(&directKin);
// 	timedomain.addBlock(&posDiff);
// 	timedomain.addBlock(&posSum);
// 	timedomain.addBlock(&posController);
// 	timedomain.addBlock(&speedSum);
// 	timedomain.addBlock(&speedLimitation);
// 	timedomain.addBlock(&speedController);
// 	timedomain.addBlock(&accSum);
// 	timedomain.addBlock(&inertia);
// 	timedomain.addBlock(&forceLimitation);
// 	timedomain.addBlock(&jacobi);
// 	timedomain.addBlock(&torqueLimitation);
// 	timedomain.addBlock(&torqueGear);
// 	timedomain.addBlock(&motorModel);
// 	timedomain.addBlock(&voltageSetPoint);
// 	timedomain.addBlock(&voltageSwitch);
	
	
}

void ControlSystem_server::start() {
	timedomain.start();
}

void ControlSystem_server::stop() {
	timedomain.stop();
	timedomain.join();
}

void ControlSystem_server::enableAxis() {
	board.setEnable(true);
	board.setReset(false);
}

void ControlSystem_server::disableAxis() {
	board.setEnable(false);
	board.setReset(true);
}

void ControlSystem_server::setVoltageForInitializing(AxisVector u) {
// 	voltageSetPoint.setValue(u);
}

bool ControlSystem_server::switchToPosControl() {
	if(homed || !allAxisStopped()) return false;
	board.resetPositions(q012homingOffset, q012homingOffset, q012homingOffset, q3homingOffset);	
	setVoltageForInitializing({0, 0, 0, 0});
	homed = true;
	return true;
}


void ControlSystem_server::goToPos(double x, double y, double z, double phi) {
	AxisVector p;
	p << x, y, z, phi;
// 	pathPlanner.gotoPoint(p);
}

void ControlSystem_server::initBoard() {
	if(!board.open("/dev/spidev1.0"))
		throw EEROSException("failed to open SPI device");
}

AxisVector ControlSystem_server::getTcpPos() {
	return serverData.getIn().getSignal().getValue();
// 	return directKin.getOut().getSignal().getValue();
}

AxisVector ControlSystem_server::getAxisPos() {
	return serverData.getOut().getSignal().getValue();
// 	return angleGear.getOut().getSignal().getValue();
}

bool ControlSystem_server::allAxisStopped(double maxSpeed) {
	for(int i = 0; i < nofAxis; i++) {
		if(board.getSpeedOut().getSignal().getValue()[i] > maxSpeed) return false;
	}
	return true;
}

bool ControlSystem_server::axisHomed() {
	return homed;
}
