#include "ControlSystem_client.hpp"
#include "../../safety/DeltaSafetyProperties.hpp"

#include <eeros/core/EEROSException.hpp>
#include <unistd.h>

#include <iostream>

using namespace eeduro::delta;
using namespace eeros;
using namespace eeros::control;

ControlSystem_client::ControlSystem_client(Client* clientThread) :
	clientData(clientThread),
	
	i(i1524, i1524, i1524, i0816),
	kM(kM1524, kM1524, kM1524, kM0816),
	RA(RA1524, RA1524, RA1524, RA0816),
	
	jacobian(kinematic.get_offset()),
	
	homed(false),
	
	joystick("/dev/input/js0"),
	mouse("/dev/input/event1"),
	pathPlanner({1, 1, 1, 5}, {10, 10, 10, 50}, dt), // TODO
	inputSwitch(1),
	posController(kp),
	speedController(kd),
	inertia(jacobian),
	jacobi(jacobian),
	motorModel(kM, RA),
	voltageSwitch(1),
	directKin(kinematic),
	timedomain("Main time domain", dt, true) {
		
	clientData.getIn().connect(board.getPosOut());
	
	torqueLimitation.enable();
	torqueGear.setGain(1.0 / i);
	angleGear.setGain(1.0 / i);
	
	posSum.negateInput(1);
	speedSum.negateInput(1);
	
	board.getIn().connect(voltageSwitch.getOut());
	
// 	// Version 1
// 	inputSwitch.getIn(0).connect(pathPlanner.getPosOut());
// 	inputSwitch.getIn(1).connect(mouse.getOut());     
// 	inputSwitch.getIn(2).connect(joystick.getOut());
	
	// Version 2
	inputSwitch.getIn(0).connect(pathPlanner.getPosOut());
	derMouse.getIn().connect(mouse.getOut());
	derJoystick.getIn().connect(joystick.getOut());
	vel2posInputs.getMouseIn().connect(derMouse.getOut());        
	vel2posInputs.getJoystickIn().connect(derJoystick.getOut());  
	inputSwitch.getIn(1).connect(vel2posInputs.getPositionOut()); 
	
	posSum.getIn(0).connect(inputSwitch.getOut());
	posSum.getIn(1).connect(directKin.getOut());
	posController.getIn().connect(posSum.getOut());
	posDiff.getIn().connect(directKin.getOut());
	speedSum.getIn(0).connect(posController.getOut());
	speedSum.getIn(1).connect(posDiff.getOut());
// 	speedSum.getIn(2).connect(pathPlanner.getVelOut());
	speedLimitation.getIn().connect(speedSum.getOut());
	speedController.getIn().connect(speedLimitation.getOut());
	accSum.getIn(0).connect(speedController.getOut());
// 	accSum.getIn(1).connect(spathPlanner.getAccOut());
	inertia.getAccelerationInput().connect(accSum.getOut());
	inertia.getJointPosInput().connect(angleGear.getOut());
	inertia.getTcpPosInput().connect(directKin.getOut());
	forceLimitation.getIn().connect(inertia.getOut());
	jacobi.getForceInput().connect(forceLimitation.getOut());
	jacobi.getJointPosInput().connect(angleGear.getOut());
	jacobi.getTcpPosInput().connect(directKin.getOut());
	torqueLimitation.getIn().connect(jacobi.getOut());
	torqueGear.getIn().connect(torqueLimitation.getOut());
	angleGear.getIn().connect(board.getPosOut());
	motorModel.getTorqueIn().connect(torqueGear.getOut());
	motorModel.getSpeedIn().connect(board.getSpeedOut());
	voltageSwitch.getIn(0).connect(motorModel.getOut());
	voltageSwitch.getIn(1).connect(voltageSetPoint.getOut());
	directKin.getIn().connect(angleGear.getOut());
	
	timedomain.addBlock(&clientData);
	
	timedomain.addBlock(&joystick);
	timedomain.addBlock(&mouse);
	timedomain.addBlock(&derJoystick);
	timedomain.addBlock(&derMouse);
	timedomain.addBlock(&vel2posInputs); 
	timedomain.addBlock(&pathPlanner);
	timedomain.addBlock(&inputSwitch);
	timedomain.addBlock(&board);
	timedomain.addBlock(&angleGear);
	timedomain.addBlock(&directKin);
	timedomain.addBlock(&posDiff);
	timedomain.addBlock(&posSum);
	timedomain.addBlock(&posController);
	timedomain.addBlock(&speedSum);
	timedomain.addBlock(&speedLimitation);
	timedomain.addBlock(&speedController);
	timedomain.addBlock(&accSum);
	timedomain.addBlock(&inertia);
	timedomain.addBlock(&forceLimitation);
	timedomain.addBlock(&jacobi);
	timedomain.addBlock(&torqueLimitation);
	timedomain.addBlock(&torqueGear);
	timedomain.addBlock(&motorModel);
	timedomain.addBlock(&voltageSetPoint);
	timedomain.addBlock(&voltageSwitch);
	
}

void ControlSystem_client::start() {
	timedomain.start();
}

void ControlSystem_client::stop() {
	timedomain.stop();
	timedomain.join();
}

void ControlSystem_client::enableAxis() {
	board.setEnable(true);
	board.setReset(false);
}

void ControlSystem_client::disableAxis() {
	board.setEnable(false);
	board.setReset(true);
}

void ControlSystem_client::setVoltageForInitializing(AxisVector u) {
	voltageSetPoint.setValue(u);
}

bool ControlSystem_client::switchToPosControl() {
	if(homed || !allAxisStopped()) return false;
	board.resetPositions(q012homingOffset, q012homingOffset, q012homingOffset, q3homingOffset);	
	setVoltageForInitializing({0, 0, 0, 0});
	homed = true;
	return true;
}


void ControlSystem_client::goToPos(double x, double y, double z, double phi) {
	AxisVector p;
	p << x, y, z, phi;
	pathPlanner.gotoPoint(p);
}

void ControlSystem_client::initBoard() {
	if(!board.open("/dev/spidev1.0"))
		throw EEROSException("failed to open SPI device");
}

AxisVector ControlSystem_client::getTcpPos() {
	return directKin.getOut().getSignal().getValue();
}

AxisVector ControlSystem_client::getAxisPos() {
	return angleGear.getOut().getSignal().getValue();
}

bool ControlSystem_client::allAxisStopped(double maxSpeed) {
	for(int i = 0; i < nofAxis; i++) {
		if(board.getSpeedOut().getSignal().getValue()[i] > maxSpeed) return false;
	}
	return true;
}

bool ControlSystem_client::axisHomed() {
	return homed;
}