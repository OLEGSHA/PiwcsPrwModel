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

} // namespace piwcs::prw
