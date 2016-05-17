#ifndef CH_NTB_EEDURO_DEMO_MOVEBLOCKSEQUENCE_CLIENT_HPP_
#define CH_NTB_EEDURO_DEMO_MOVEBLOCKSEQUENCE_CLIENT_HPP_

#include <eeros/sequencer/Sequence.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include "../../control/client/ControlSystem_client.hpp"
#include "../../Calibration.hpp"

namespace eeduro {
	namespace delta {
		class MoveBlockSequence_client : public eeros::sequencer::Sequence<void, int, int> {
		public:
			MoveBlockSequence_client(eeros::sequencer::Sequencer* sequencer, eeduro::delta::ControlSystem_client* controlSys, eeros::safety::SafetySystem* safetySys);
			
			virtual void run(int from, int to);
			virtual void run(int to);
			
		private:
			virtual void up();
			virtual void down();
			virtual void grab();
			virtual void release();
			virtual void move(int position);
			virtual void waitUntilPointReached();
			
			eeduro::delta::ControlSystem_client* controlSys;
			eeros::safety::SafetySystem* safetySys;
			Calibration calibration;
			int position;
		};
	}
}

#endif // CH_NTB_EEDURO_DEMO_MOVEBLOCKSEQUENCE_CLIENT_HPP_
