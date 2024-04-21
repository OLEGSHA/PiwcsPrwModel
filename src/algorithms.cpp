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

	for (auto &[id, node]: model.nodes()){

		bool m_trackCorrect = [&](SlotId start, SlotId end){

			// Connected sections must have same directionality
			if (model.section(node.section(start))->dir() != model.section(node.section(end))->dir()){
				// Otherwise this is certainly a dead end
				return false;
			}

			// Bidirectional track inside node is correct
			if (model.section(node.section(start))->dir() == Section::AllowedTravel::BIDIR){
				return true;
			}

			// Correct directional track must connect different slots of sections
			if (model.section(node.section(0))->start() == id && model.section(node.section(1))->end() == id){
				return true;
			}
			if (model.section(node.section(0))->end() == id && model.section(node.section(1))->start() == id){
				return true;
			}

			return false;
		};

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

		switch (node.type()) {
			case END:
				// Single slot of end must be connected to bidir. section
				if (model.section(node.section(0))->isBidir()) {
					continue;
				}

				return false;

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

			default:

				return false;

		}


	}
}

} // namespace piwcs::prw
