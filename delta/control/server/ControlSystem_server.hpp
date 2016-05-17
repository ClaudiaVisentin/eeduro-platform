#ifndef CONTROLSYSTEM_SERVER_HPP_
#define CONTROLSYSTEM_SERVER_HPP_

#include <eeduro/Board.hpp>
#include <eeros/control/Sum.hpp>
#include <eeros/control/D.hpp>
#include <eeros/control/Gain.hpp>
#include <eeros/control/Constant.hpp>
#include <eeros/control/TimeDomain.hpp>
#include <eeros/math/Matrix.hpp>
#include <eeros/control/Switch.hpp>
#include <eeros/control/Saturation.hpp>
#include <eeros/control/XBoxInput.hpp>
#include <eeros/control/MouseInput.hpp>
#include <eeros/control/PeripheralOutput.hpp>
#include "../Kinematic.hpp"
#include "../Jacobian.hpp"
#include "../DirectKinematic.hpp"
#include "../Jacobi.hpp"
#include "../Inertia.hpp"
#include "../MotorModel.hpp"
#include "../PathPlanner.hpp"
#include "../Velocity2Position.hpp"
#include "ServerData.hpp"
#include "../constants.hpp"
#include "../types.hpp"

#include "../../socket/Server.hpp"

namespace eeduro {
	namespace delta {
		
		class ControlSystem_server {
			
		public:
			ControlSystem_server(Server* serverThread);
			
			void start();
			void stop();
			
			// Methods for the sequencer
			void enableAxis();
			void disableAxis();
			void initBoard();
			void setVoltageForInitializing(AxisVector u);
			bool switchToPosControl();
			bool axisHomed();
			AxisVector getTcpPos();
			AxisVector getAxisPos();
			void goToPos(double x, double y, double z, double phi);
			
//		protected:
			AxisVector i;
			AxisVector kM;
			AxisVector RA;
			Kinematic kinematic;
			Jacobian jacobian;
			bool homed;
			
			eeduro::delta::ServerData 							serverData;
			
			// Blocks
			eeduro::Board										board;
			
			// Version 1 with single inputs (one for joystick, one for mouse)
			eeros::control::XBoxInput							joystick;
			eeros::control::MouseInput							mouse;

			// Version 2 with both inputs together
			eeros::control::D<AxisVector>						derMouse;
			eeros::control::D<AxisVector>						derJoystick;
			eeduro::Velocity2Position							vel2posInputs;
			
			eeduro::delta::PathPlanner							pathPlanner;
			eeros::control::Switch<3, AxisVector>				inputSwitch;
			eeros::control::Sum<2, AxisVector>					posSum;
			eeros::control::Gain<AxisVector>					posController;
			eeros::control::D<AxisVector>						posDiff;
			eeros::control::Sum<3, AxisVector>					speedSum;
			eeros::control::Saturation<AxisVector>				speedLimitation;
			eeros::control::Gain<AxisVector>					speedController;
			eeros::control::Sum<2, AxisVector>					accSum;
			eeduro::delta::Inertia								inertia;
			eeros::control::Saturation<AxisVector>				forceLimitation;
			eeduro::delta::Jacobi								jacobi;
			eeros::control::Saturation<AxisVector>				torqueLimitation;
			eeros::control::Gain<AxisVector, AxisVector, true>	torqueGear;
			eeros::control::Gain<AxisVector, AxisVector, true>	angleGear;
			eeduro::delta::MotorModel							motorModel;
			eeros::control::Switch<2, AxisVector>				voltageSwitch;
			eeros::control::Constant<AxisVector>				voltageSetPoint;
			eeduro::delta::DirectKinematic						directKin;
			
			eeros::control::TimeDomain timedomain;
			
		private:
			bool allAxisStopped(double maxSpeed = 0.001);
		};
	}
}

#endif // CONTROLSYSTEM_SERVER_HPP_
