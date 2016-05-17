#ifndef CH_NTB_EEDURO_DEMO_DETECTSEQUENCE_CLIENT_HPP_
#define CH_NTB_EEDURO_DEMO_DETECTSEQUENCE_CLIENT_HPP_

#include <eeros/sequencer/Sequence.hpp>
#include "../../control/client/ControlSystem_client.hpp"
#include "../../Calibration.hpp"

namespace eeduro {
	namespace delta {
		class DetectSequence_client : public eeros::sequencer::Sequence<int, int> {
		public:
			DetectSequence_client(eeros::sequencer::Sequencer* sequencer, eeduro::delta::ControlSystem_client* controlSys);
			
			virtual int run(int position);
			
		private:
			virtual void waitUntilPointReached();
			
			eeduro::delta::ControlSystem_client* controlSys;
			Calibration calibration;
		};
	}
}

#endif // CH_NTB_EEDURO_DEMO_DETECTSEQUENCE_CLIENT_HPP_
