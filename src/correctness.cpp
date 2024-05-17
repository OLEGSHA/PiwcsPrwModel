#include <array>

#include <piwcsprwmodel/algorithms.h>

#include "correctness.h"
#include "debug.h"

namespace piwcs::prw {

bool isLocallyCorrect(const Model &model, IdRef id) {
    const Node *node = model.node(id);
    if(!node){
        // Node not found
        return false;
    }

    std::array<bool, Node::MAX_SLOTS> isInward;
    std::array<bool, Node::MAX_SLOTS> isOutward;
    isInward.fill(false);
    isOutward.fill(false);

    std::array<const Section *, Node::MAX_SLOTS> sections;
    sections.fill(nullptr);

    for (SlotId slot = 0; slot < node->sectionCount(); ++slot) {
        IdRef sectionId = node->section(slot);
        _ASSERT(sectionId != ID_INVALID, "invalid slot index");

        sections[slot] = model.section(sectionId);

        /*
         * If section pointer is nullptr then id of Section in slot is not
         * valid. Model is incomplete and node locally incorrect
         */
        if (!sections[slot]) {
            return false;
        }
    }

    if (node->type() == END) {
        /*
         * Single slot of end must be connected to bidir. section or
         * forbidden section
         */
        return sections.at(0)->dir() != Section::AllowedTravel::UNIDIR;
    }

    else {
        // Set outward/inward flags
        for (SlotId start = 0; start < node->sectionCount(); ++start) {
            for (SlotId end = 0; end < node->sectionCount(); ++end) {

                bool isTraversable = node->couldTraverse(start, end);
                bool isBidirectional = sections.at(start)->isBidir();
                bool isUnidirectional = sections.at(start)->isUnidir();
                bool isSectionEnd = sections.at(start)->end() == id;

                if (isTraversable &&
                    (isBidirectional || (isUnidirectional && isSectionEnd))) {
                    isInward[start] = true;
                    isOutward[end] = true;
                } // if
            } // for end
        } // for start

        // Check slot correctness
        for (SlotId slot = 0; slot < node->sectionCount(); ++slot) {
            const Section *section = sections.at(slot);

            // Bidirectional slot
            if (isInward[slot] && isOutward[slot]) {
                if (!section->isBidir()) {
                    return false;
                }
                continue;
            }

            // None slot
            if (!(isInward[slot] || isOutward[slot])) {
                if (section->allowsTravel()) {
                    return false;
                }
                continue;
            }

            bool isUnidirectional = section->isUnidir();
            bool isEnd = section->end() == id;

            /*
             * All previous conditionals were not triggered, so this slot is
             * unidirectional
             */
            if (!isUnidirectional) {
                return false;
            }

            /*
             * If slot is inward, the section must lead to it
             */
            if (isInward[slot] && !isEnd) {
                return false;
            }

            /*
             * If slot is outward, the section must lead from it
             */
            if (isOutward[slot] && isEnd) {
                return false;
            }

        } // for slot

    } // else

    return true;
}

bool isCorrect(const Model &model) {

    // Incomplete model is not correct
    if (!isComplete(model)) {
        return false;
    }

    for (const auto &[id, node] : model.nodes()) {
        if (!isLocallyCorrect(model, id)) {
            return false;
        }
    } // for nodes

    return true;
}

} // namespace piwcs::prw
