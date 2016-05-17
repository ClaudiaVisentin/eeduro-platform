#ifndef DELTASAFETYPROPERTIES_CLIENT_HPP
#define DELTASAFETYPROPERTIES_CLIENT_HPP

#include <eeros/safety/SafetyProperties.hpp>
#include <eeros/hal/PeripheralOutput.hpp>
#include <eeros/hal/PeripheralInput.hpp>
#include <eeros/logger/Logger.hpp>
#include <eeros/logger/LogWriter.hpp>

namespace eeduro {
	namespace delta {
		
		class ControlSystem_client;
		
		// Define all possible events
		enum events {
			doOff,
			doSwInit,
			swInitDone,
			doEmergency,
			doEmergencyReset,
			emergencyResetDone,
			doControlStart,
			doControlStop,
			controlStoppingDone,
			controlStartingDone,
			doPoweringUp,
			poweringUpDone,
			doPoweringDown,
			poweringDownDone,
			doHoming,
			doParking,
			parkingDone,
			homeingDone,
			doSystemReady,
			doAutoMoving,
			doMouseTeaching,
			doJoystickTeaching,
			stopMoving
		};
		
		// Name all levels
		enum levels {
			off = 0,
			swInitializing = 1,
			swInitialized = 2,
			emergency = 5,
			resetingEmergency = 6,
	//		waitingForApproval = 10,
			controlStopping = 11,
			controlStarting = 12,
			systemOn = 20,
			poweringDown = 21,
			poweringUp = 22,
			powerOn = 25,
			homeing = 30,
			axesHomed = 31,
			parking = 32,
			parked = 33,
			systemReady = 40,
			autoMoving = 50,
			mouseTeaching = 51,
			joystickTeaching = 52
		};

		class DeltaSafetyProperties_client : public eeros::safety::SafetyProperties {

			
		public:
			DeltaSafetyProperties_client(ControlSystem_client* cs);
			
			
			// criticcal outputs
			eeros::hal::PeripheralOutput<bool>* power;
			eeros::hal::PeripheralOutput<bool>* enable0;
			eeros::hal::PeripheralOutput<bool>* enable1;
			eeros::hal::PeripheralOutput<bool>* enable2;
			eeros::hal::PeripheralOutput<bool>* enable3;
			eeros::hal::PeripheralOutput<bool>* led;
			
			// criticcal inputs
			eeros::hal::PeripheralInput<bool>* emergencyStop;
			eeros::hal::PeripheralInput<bool>* approval;
			eeros::hal::PeripheralInput<double>* q0;
			eeros::hal::PeripheralInput<double>* q1;
			eeros::hal::PeripheralInput<double>* q2;
			eeros::hal::PeripheralInput<double>* q3;
			
		private:
			ControlSystem_client* controlSys;
		};
	}
}
#endif // DELTASAFETYPROPERTIES_CLIENT_HPP
