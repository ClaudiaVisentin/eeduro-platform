#ifndef CH_NTB_EEDURO_DELTA_MAINSEQUENCEFAULHABER_SERVER_HPP_
#define CH_NTB_EEDURO_DELTA_MAINSEQUENCEFAULHABER_SERVER_HPP_

#include <eeros/sequencer/Sequence.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include "../../control/server/ControlSystem_server.hpp"
#include "MoveBlockSequence_server.hpp"
#include "SortSequence_server.hpp"

namespace eeduro {
	namespace delta {
		class MainSequenceFaulhaber_server : public eeros::sequencer::Sequence<void> {
		public:
			MainSequenceFaulhaber_server(eeros::sequencer::Sequencer* sequencer, eeduro::delta::ControlSystem_server* controlSys, eeros::safety::SafetySystem* safetySys);
			
			virtual void run();
			
		private:
			MoveBlockSequence_server moveBlock;
			SortSequence_server sort;
			
			eeduro::delta::ControlSystem_server* controlSys;
			eeros::safety::SafetySystem* safetySys;
		};
	}
}

#endif // CH_NTB_EEDURO_DELTA_MAINSEQUENCEFAULHABER_SERVER_HPP_
