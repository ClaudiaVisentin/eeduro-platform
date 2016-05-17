#ifndef CH_NTB_EEDURO_DELTA_MAINSEQUENCE_CLIENT_HPP_
#define CH_NTB_EEDURO_DELTA_MAINSEQUENCE_CLIENT_HPP_

#include <eeros/sequencer/Sequence.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include "../../control/client/ControlSystem_client.hpp"
#include "MoveBlockSequence_client.hpp"
#include "SortSequence_client.hpp"

namespace eeduro {
	namespace delta {
		class MainSequence_client : public eeros::sequencer::Sequence<void> {
		public:
			MainSequence_client(eeros::sequencer::Sequencer* sequencer, eeduro::delta::ControlSystem_client* controlSys, eeros::safety::SafetySystem* safetySys);
			
			virtual void run();
			
		private:
			MoveBlockSequence_client moveBlock;
			SortSequence_client sort;
			
			eeduro::delta::ControlSystem_client* controlSys;
			eeros::safety::SafetySystem* safetySys;
		};
	}
}

#endif // CH_NTB_EEDURO_DELTA_MAINSEQUENCE_CLIENT_HPP_
