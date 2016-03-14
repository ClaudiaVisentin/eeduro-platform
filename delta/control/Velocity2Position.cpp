#include "Velocity2Position.hpp"
#include "constants.hpp"
#include <iostream>

using namespace eeduro;
using namespace eeduro::delta;
using namespace eeros::control;
using namespace eeros::math;

Velocity2Position::Velocity2Position() {}

Velocity2Position::~Velocity2Position() {}

void Velocity2Position::run() {
	Vector4 mouse_v = mouseIn.getSignal().getValue();
	Vector4 joystick_v = joytstickIn.getSignal().getValue();
	Vector4 sum = mouse_v + joystick_v;
	Vector4 p = positionOut.getSignal().getValue() + sum * dt;
	
	// limitation
	if (p[0] < min_x)
		p[0] = min_x;
	else if (p[0] > max_x)
		p[0] = max_x;
	
	if (p[1] < min_y)
		p[1] = min_y;
	else if (p[1] > max_y)
		p[1] = max_y;
	
	if (p[2] < min_z)
		p[2] = min_z;
	else if (p[2] > max_z)
		p[2] = max_z;
	
	if (p[3] < min_r)
		p[3] = min_r;
	else if (p[3] > max_r)
		p[3] = max_r;

	positionOut.getSignal().setValue(p);
	uint64_t ts = eeros::System::getTimeNs();
	positionOut.getSignal().setTimestamp(ts);
}

Input<Vector4>& Velocity2Position::getMouseIn() {
	return mouseIn;
}

Input<Vector4>& Velocity2Position::getJoystickIn() {
	return joytstickIn;
}

Output<Vector4>& Velocity2Position::getPositionOut() {
	return positionOut;
}

void Velocity2Position::setInitPos(Vector4 initPos) {
	positionOut.getSignal().setValue(initPos);
}
