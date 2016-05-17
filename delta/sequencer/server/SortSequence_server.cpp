#include "SortSequence_server.hpp"
#include "../../safety/server/DeltaSafetyProperties_server.hpp"
#include <unistd.h>
#include <iostream>

using namespace eeduro::delta;
using namespace eeros::sequencer;
using namespace eeros::safety;


SortSequence_server::SortSequence_server(Sequencer* sequencer, ControlSystem_server* controlSys, SafetySystem* safetySys) :
	Sequence<void>("sort", sequencer),
	moveBlock(sequencer, controlSys, safetySys),
	detect(sequencer, controlSys),
	controlSys(controlSys),
	safetySys(safetySys)
{
	calibration.loadDefaults();
	if (!calibration.load()) {
		log.warn() << "could not load calibration";
	}
}

void SortSequence_server::run() {
	std::array<int,4> blocks;
	
	// detect positions of all blocks
	for (int i = 0; i < 4; i++) {
		move(i);
		blocks[i] = detect.run(i);
	}

	{
		auto l = log.trace();
		l << "detected blocks:";
		for (int i = 0; i < 4; i++) {
			l << " " << blocks[i];
		}
	}
	
	// check for invalid values
	bool block_ok[4];
	for (int i = 0; i < 4; i++) block_ok[i] = false;
	for (int i = 0; i < 4; i++) {
		if (blocks[i] < 0 || blocks[i] > 3) {
			log.error() << "index out of range (position = " << i << ", value = " << blocks[i] << ")";
			return;
		}
		block_ok[blocks[i]] = true;
	}
	
	// check if all blocks are present
	bool all_ok = true;
	for (int i = 0; i < 4; i++) {
		if (!block_ok[i]) {
			log.error() << "missing block " << i;
			all_ok = false;
		}
	}
	if (!all_ok) return;
	
	// define if you want to sort or unsort
	int wrong_block = (-1);
	int wrong_position = (-1);
	for (int i = 0; i < blocks.size(); i++) {
		wrong_position = find(blocks, i);
		if (wrong_position < 0) {
			log.error() << "cannot find block " << i;
			return;
		}
		if (wrong_position != i) {
			wrong_block = i;
			break;
		}
	}
	
	if (wrong_block < 0)
		unsortBlocks();
	else
		sortBlocks(blocks);
}

void SortSequence_server::sortBlocks(std::array<int,4> blocks) {
	while (true) {
		// find wrong block
		int wrong_block = (-1);
		int wrong_position = (-1);
		for (int i = 0; i < blocks.size(); i++) {
			wrong_position = find(blocks, i);
			if (wrong_position < 0) {
				log.error() << "cannot find block " << i;
				return;
			}
			if (wrong_position != i) {
				wrong_block = i;
				break;
			}
		}
		if (wrong_block < 0) break;
		
		// move block to correct position
		if (wrong_block == 0) {
			int correct_position = find(blocks, wrong_position);
			if (correct_position < 0) {
				log.error() << "cannot find block " << wrong_position;
				return;
			}
			
			log.info() << "move block from position " << correct_position << " to " << wrong_position;
			moveBlock(correct_position, wrong_position);
			std::swap(blocks[correct_position], blocks[wrong_position]);
		}
		else {
			int correct_position = wrong_block;
			int empty_position = find(blocks, 0);
			if (empty_position < 0) {
				log.error() << "cannot find block 0";
				return;
			}
			log.info() << "move block from position " << correct_position << " to " << empty_position;
			moveBlock(correct_position, empty_position);
			std::swap(blocks[correct_position], blocks[empty_position]);
			
			log.info() << "move block from position " << wrong_position << " to " << correct_position;
			moveBlock(wrong_position, correct_position);
			std::swap(blocks[wrong_position], blocks[correct_position]);
		}
	}
	log.info() << "finished sorting";
}

void SortSequence_server::unsortBlocks() {
	int randIndex = rand() % 3;
	if(randIndex == 0) {
		moveBlock(3, 0);
		moveBlock(1, 3);
		moveBlock(2, 1);
	} else if (randIndex == 1) {
		moveBlock(1, 0);
		moveBlock(2, 1);
		moveBlock(3, 2);
	} else {
		moveBlock(2, 0);
		moveBlock(1, 2);
		moveBlock(3, 1);
	}
}

int SortSequence_server::find(const std::array<int,4> &blocks, int block) {
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i] == block)
			return i;
	}
	return -1;
}


void SortSequence_server::move(int position) {
	auto p = controlSys->pathPlanner.getLastPoint();
// 	p[0] = calibration.position[position].x;
// 	p[1] = calibration.position[position].y;
	
	if(position == 0) {
		p[0] = calibration.position[position].x + 0.001;   
		p[1] = calibration.position[position].y ;          
	}                                                      
	else if(position == 1) {                               
		p[0] = calibration.position[position].x;           
		p[1] = calibration.position[position].y;           
	}                                                      
	else if(position == 2) {                               
		p[0] = calibration.position[position].x; // + 0.0015;  
		p[1] = calibration.position[position].y - 0.0015;  
	}                                                      
	else if(position == 3) {                               
		p[0] = calibration.position[position].x; // + 0.0015;  
		p[1] = calibration.position[position].y; // + 0.0015;  
	}                                                      
	else {                                                 
		p[0] = calibration.position[position].x + 0.0015; // FH: + 0.0015  
		p[1] = calibration.position[position].y + 0.0015; // FH: + 0.0015
	}
	 
	controlSys->pathPlanner.gotoPoint(p);
	waitUntilPointReached();
}

void SortSequence_server::waitUntilPointReached() {
	while (!controlSys->pathPlanner.posReached()) {
		usleep(100000);
		yield();
	}
}
