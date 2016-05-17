#ifndef CH_NTB_EEDURO_DEMO_DETECTSEQUENCE_SERVER_HPP_
#define CH_NTB_EEDURO_DEMO_DETECTSEQUENCE_SERVER_HPP_

#include <eeros/sequencer/Sequence.hpp>
#include "../../control/server/ControlSystem_server.hpp"
#include "../../Calibration.hpp"

namespace eeduro {
	namespace delta {
		class DetectSequence_server : public eeros::sequencer::Sequence<int, int> {
		public:
			DetectSequence_server(eeros::sequencer::Sequencer* sequencer, eeduro::delta::ControlSystem_server* controlSys);
			
			virtual int run(int position);
			
		private:
			virtual void waitUntilPointReached();
			
			eeduro::delta::ControlSystem_server* controlSys;
			Calibration calibration;
		};
	}
}

#endif // CH_NTB_EEDURO_DEMO_DETECTSEQUENCE_SERVER_HPP_
