#ifndef PIWCS_PRW_MODEL_ALGORITHMS
#define PIWCS_PRW_MODEL_ALGORITHMS

#include "fwd.h"
#include "model.h"

/**
 * @file
 *
 * This header declares advanced algorithms that operate on Models.
 */

namespace piwcs::prw {

/**
 * Checks the provided model for _completeness_.
 *
 * To be complete, all slots that exist in the model must be connected. This
 * includes both Node and Section slots.
 *
 * @param model the Model to examine
 *
 * @return `true` if and only if the model is _complete_.
 */
bool isComplete(const Model &model);

/**
 * Checks the provided model for _correctness_.
 *
 * To be correct, model must be complete and all allowed
 * (see @ref allowed-travel-and-correctness "Allowed Travel and Correctness" ) paths
 * must have no dead ends (i.e. places from which a train will have no valid path) or unreachable starting positions
 * except for bidirectional end nodes.
 *
 * This means firstly, that all slots with _outward_ allowed travel must be connected to _inward_ slots and vice versa
 * and all _both_ allowed travel slots must connect to _both_ slots.
 * Secondly, nodes inside forbidden regions can not be considered as valid starting positions
 * and @ref piwcs::prw::MANUAL "MANUAL" switches do not allow trains to enter diverging track. Thus in a correct model
 * nodes that connect sections with @ref piwcs::prw::Section::AllowedTravel "NONE" allowed travel
 * must be either @ref piwcs::prw::MANUAL "MANUAL" switches with such section connected to diverging track only
 * or have sections with @ref piwcs::prw::Section::AllowedTravel "NONE"
 * allowed travel connected to every slot.
 *
 * @note
 * Allowed travel of a slot, be it section slot or node slot, is
 * - _outward_ iff a train must leave this section or node through this slot
 * - _inward_ iff a train must enter this section or node through this slot
 * - _both_ iff a train is able to both leave or enter this section or node through this slot
 *
 * @param model the Model to examine
 *
 * @return `true` if and only if the model is _correct_.
 */
bool isCorrect(const Model &model);

} // namespace piwcs::prw

#endif // PIWCS_PRW_MODEL_ALGORITHMS
