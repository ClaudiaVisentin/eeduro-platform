#ifndef CH_NTB_EEDURO_DELTA_SORTSEQUENCE_CLIENT_HPP_
#define CH_NTB_EEDURO_DELTA_SORTSEQUENCE_CLIENT_HPP_

#include <eeros/sequencer/Sequence.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include "../../control/client/ControlSystem_client.hpp"
#include "MoveBlockSequence_client.hpp"
#include "DetectSequence_client.hpp"
#include <array>

namespace eeduro {
	namespace delta {
		class SortSequence_client : public eeros::sequencer::Sequence<void> {
		public:
			SortSequence_client(eeros::sequencer::Sequencer* sequencer, eeduro::delta::ControlSystem_client* controlSys, eeros::safety::SafetySystem* safetySys);
			
			virtual void run();
			
		private:
			virtual void move(int position);
			virtual void waitUntilPointReached();
			virtual void sortBlocks(std::array<int,4> blocks);
			virtual void unsortBlocks();
			virtual int find(const std::array<int,4> &blocks, int block);
			
			MoveBlockSequence_client moveBlock;
			DetectSequence_client detect;
			eeduro::delta::ControlSystem_client* controlSys;
			eeros::safety::SafetySystem* safetySys;
			Calibration calibration;
		};
	}
}

#endif // CH_NTB_EEDURO_DELTA_SORTSEQUENCE_CLIENT_HPP_
