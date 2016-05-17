#ifndef CH_NTB_EEDURO_DELTA_SORTSEQUENCE_SERVER_HPP_
#define CH_NTB_EEDURO_DELTA_SORTSEQUENCE_SERVER_HPP_

#include <eeros/sequencer/Sequence.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include "../../control/server/ControlSystem_server.hpp"
#include "MoveBlockSequence_server.hpp"
#include "DetectSequence_server.hpp"
#include "../../Calibration.hpp"
#include <array>

namespace eeduro {
	namespace delta {
		class SortSequence_server : public eeros::sequencer::Sequence<void> {
		public:
			SortSequence_server(eeros::sequencer::Sequencer* sequencer, eeduro::delta::ControlSystem_server* controlSys, eeros::safety::SafetySystem* safetySys);
			
			virtual void run();
			
		private:
			virtual void move(int position);
			virtual void waitUntilPointReached();
			virtual void sortBlocks(std::array<int,4> blocks);
			virtual void unsortBlocks();
			virtual int find(const std::array<int,4> &blocks, int block);
			
			MoveBlockSequence_server moveBlock;
			DetectSequence_server detect;
			eeduro::delta::ControlSystem_server* controlSys;
			eeros::safety::SafetySystem* safetySys;
			Calibration calibration;
		};
	}
}

#endif // CH_NTB_EEDURO_DELTA_SORTSEQUENCE_SERVER_HPP_
