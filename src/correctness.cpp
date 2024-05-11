#include <array>

#include <piwcsprwmodel/algorithms.h>

#include "correctness.h"

namespace piwcs::prw {

class EmptySlotError {};

bool isLocallyCorrect(const Model &model, IdRef id) {
    const Node *node = model.node(id);

    std::array<bool, Node::MAX_SLOTS> isInward;
    std::array<bool, Node::MAX_SLOTS> isOutward;
    isInward.fill(false);
    isOutward.fill(false);

    auto sectionAt = [&](SlotId slot) -> const Section * {
        const Section *ptr = model.section(node->section(slot));
        if (!ptr) {
            throw EmptySlotError();
        }
        return ptr;
    };

    try {
        if (node->type() == END) {
            // Single slot of end must be connected to bidir. section or
            // forbidden section
            return sectionAt(0)->dir() != Section::AllowedTravel::UNIDIR;
        }

        else {
            // Set outward/inward flags
            for (SlotId start = 0; start < node->sectionCount(); ++start) {
                for (SlotId end = 0; end < node->sectionCount(); ++end) {

                    bool isTraversable = node->couldTraverse(start, end);
                    bool isBidirectional = sectionAt(start)->isBidir();
                    bool isUnidirectional = sectionAt(start)->isUnidir();
                    bool isSectionEnd = sectionAt(start)->end() == id;

                    if (isTraversable && (isBidirectional ||
                                          (isUnidirectional && isSectionEnd))) {
                        isInward[start] = true;
                        isOutward[end] = true;
                    } // if
                } // for end
            } // for start

            // Check slot correctness
            for (SlotId slot = 0; slot < node->sectionCount(); ++slot) {
                const Section *section = sectionAt(slot);

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

                // Unidirectional slot
                if (!section->isUnidir()) {
                    return false;
                }

                bool isUnidirectional = section->isUnidir();
                bool isEnd = section->end() == id;

                if (!isUnidirectional) {
                    return false;
                }

                if (isInward[slot] && !isEnd) {
                    return false;
                }

                if (isOutward[slot] && isEnd) {
                    return false;
                }

            } // for slot

        } // else
    } catch (const EmptySlotError &err) {
        return false;
    }

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
