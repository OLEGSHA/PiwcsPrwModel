#include <array>

#include <piwcsprwmodel/algorithms.h>

#include "correctness.h"

namespace piwcs::prw {

bool isLocallyCorrect(const Model& model, IdRef id){
	const Node* node = model.node(id);

	std::array<bool, Node::MAX_SLOTS> isInward;
	std::array<bool, Node::MAX_SLOTS> isOutward;
	isInward.fill(false);
	isOutward.fill(false);

	if (node->type()==END) {
		// Single slot of end must be connected to bidir. section or forbidden section
		return model.section(node->section(0))->dir()!=Section::AllowedTravel::UNIDIR;
	}

	else{
		// Set outward/inward flags
		for(SlotId start = 0; start < node->sectionCount(); ++start){
			for(SlotId end = 0; end < node->sectionCount(); ++end){
				if(
					node->couldTraverse(start, end) &&
					(
						model.section(node->section(start))->isBidir() ||
						(
							model.section(node->section(start))->isUnidir() &&
							model.section(node->section(start))->end() == id
						)
					)
				){
					isInward[start] = true;
					isOutward[end] = true;
				} // if
			} // for end
		} // for start

		// Check slot correctness
		for(SlotId slot = 0; slot < node->sectionCount(); ++slot){
			const Section* section = model.section(node->section(slot));

			// Bidirectional slot
			if(isInward[slot] && isOutward[slot]){
				if (!section->isBidir()){
					return false;
				}
				continue;
			}

			// None slot
			if(!(isInward[slot]) && !(isOutward[slot])) {
				if(section->allowsTravel()){
					return false;
				}
				continue;
			}

			// Unidirectional slot
			if(!section->isUnidir()){
				return false;
			}

			if(
				isInward[slot] &&
				section->end() != id
				){
				return false;
			}

			if(
				isOutward[slot] &&
				section->start() != id
				){
				return false;
			}
		} // for slot

		// Reset outward/inward flags
		for(SlotId slot = 0; slot < node->sectionCount(); ++slot){
			isInward[slot] = false;
			isOutward[slot] = false;
		}

	} // else

	return true;

}

bool isCorrect(const Model &model){

	// Incomplete model is not correct
	if (!isComplete(model)){
		return false;
	}

	for (const auto &[id, node]: model.nodes()){
		if(!isLocallyCorrect(model, id)){
			return false;
		}
	} // for nodes

	return true;

}

} // namespace piwcs::prw
