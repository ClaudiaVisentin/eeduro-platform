#ifndef CH_NTB_EEDURO_DELTA_CALIBRATESEQUENCE_SERVER_HPP_
#define CH_NTB_EEDURO_DELTA_CALIBRATESEQUENCE_SERVER_HPP_

#include <eeros/sequencer/Sequence.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include "../../control/server/ControlSystem_server.hpp"
#include "../../Calibration.hpp"
#include <vector>

namespace eeduro {
	namespace delta {
		class CalibrateSequence_server : public eeros::sequencer::Sequence<void> {
		public:
			CalibrateSequence_server(eeros::sequencer::Sequencer* sequencer, eeduro::delta::ControlSystem_server* controlSys, eeros::safety::SafetySystem* safetySys);
			
			virtual void run();
			
		protected:
			void waitUntilReady();
			void waitForButton(std::vector<int> buttons);
			void waitForGreenButton();
			void waitForRedButton();
			void waitForBlueButton();
			void waitForBlueOrRedButton();
			
		private:
			Calibration calibration;
			eeduro::delta::ControlSystem_server* controlSys;
			eeros::safety::SafetySystem* safetySys;
		};
	}
}

#endif // CH_NTB_EEDURO_DELTA_CALIBRATESEQUENCE_SERVER_HPP_
