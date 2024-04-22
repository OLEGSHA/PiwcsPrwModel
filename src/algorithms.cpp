#include <vector>

#include <piwcsprwmodel/algorithms.h>

namespace piwcs::prw {

bool isComplete(const Model &model) {
    // Check nodes
    for (const auto &[id, node] : model.nodes()) {
        for (SlotId slot = 0; slot < node.sectionCount(); slot++) {
            if (node.section(slot) == ID_NULL) {
                return false;
            }
        }
    }

    // Check sections
    for (const auto &[id, section] : model.sections()) {
        if (!section.isConnected()) {
            return false;
        }
    }

    return true;
}

bool isCorrect(const Model &model){

	// Incomplete model is not correct
	if (!isComplete(model)){
		return false;
	}

	auto isInward = std::vector<bool>(Node::MAX_SLOTS, false);
	auto isOutward = std::vector<bool>(Node::MAX_SLOTS, false);

	for (auto &[id, node]: model.nodes()){


		/*
		bool m_trackDirCorrect = [&](SlotId from, SlotId to){
			// Correct directional track must connect two unidirectional sections
			if (
				!(model.section(node.section(from))->dir() == Section::AllowedTravel::UNIDIR) ||
				!(model.section(node.section(from))->dir() == Section::AllowedTravel::UNIDIR)
			){
				return false;
			}
			// Correct directional track must connect different slots of sections
			if (model.section(node.section(from))->end() == id && model.section(node.section(to))->start() == id){
				return true;
			}

			return false;
		};

		bool m_trackBidirCorrect = [&](SlotId from, SlotId to){
			// Correct bidirectional track must connect two bidirectional sections
			if (
				!(model.section(node.section(from))->dir() == Section::AllowedTravel::BIDIR) ||
				!(model.section(node.section(to))->dir() == Section::AllowedTravel::BIDIR)
			){
				return false;
			}

			return true;
		};
		*/
		/*
		// Check if node is inside forbidden region
		bool isInForbidden = true;
		for (SlotId slot=0; slot<=node.sectionCount(); ++slot) {
			// If section in `slot` is not forbidden `node` is not in forbidden region
			if (!(model.section(node.section(slot))->allowsTravel())){
				isInForbidden = false;
				break;
			}
		}

		// Forbidden node is always locally correct
		if (isInForbidden){
			continue;
		}
		*/
		switch (node.type()) {
			case END:
				// Single slot of end must be connected to bidir. section or forbidden section
				if (
						model.section(node.section(0))->dir()==Section::AllowedTravel::NONE ||
						model.section(node.section(0))->dir()==Section::AllowedTravel::BIDIR
					) {
					continue;
				}

				return false;
			/*
			case THRU:
				// Check correctness of single track

				if(m_trackCorrect(0, 1)){
					continue;
				}

				return false;

			case CROSSING:
				// Check correctness of both tracks
				if(m_trackCorrect(0, 1) && m_trackCorrect(2, 3)){
					continue;
				}

				return false;

			case MANUAL:
				// Check correctness of common->straight track
				if(!m_trackCorrect(COMMON, STRAIGHT)){
					return false;
				}

				// Check that diverging connected to forbidden section
				if(!model.section(node.section(DIVERGING))->allowsTravel()){
					continue;
				}
				return false;
			*/
			default:
				// Set outward/inward flags
				for(SlotId start = 0; start<=node.sectionCount(); ++start){
					for(SlotId end = 0; end<=node.sectionCount(); ++end){
						if(
							node.couldTraverse(start, end) &&
							(
								model.section(node.section(start))->isBidir() ||
								(
									model.section(node.section(start))->isUnidir() &&
									model.section(node.section(start))->end() == id
								)
							)
						){
							isInward[start] = true;
							isOutward[end] = false;
						} // if
					} // for end
				} // for start

				// Check slot correctness
				for(SlotId slot = 0; slot<=node.sectionCount(); ++slot){
					const Section* section = model.section(node.section(slot));

					// Bidirectional slot
					if(isInward[slot] && isOutward[slot]){
						if (!section->isBidir()){
							return false;
						}
					}

					// None slot
					if(!isInward[slot] && !isOutward[slot]) {
						if(section->allowsTravel()){
							return false;
						}
					}

					// Unidirectional slot
					if(!section->isUnidir()){
						return false;
					}

					if(
						isInward[slot] &&
						!(section->end() == id)
						){
						return false;
					}

					if(
						isOutward[slot] &&
						!(section->start() == id)
						){
						return false;
					}
				}

				// Reset outward/inward flags
				for(SlotId slot = 0; slot<=node.sectionCount(); ++slot){
					isInward[slot] = false;
					isOutward[slot] = false;
				}

		} // switch(node.type())


	} // for nodes

	return true;

}

} // namespace piwcs::prw
