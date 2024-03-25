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

} // namespace piwcs::prw

#endif // PIWCS_PRW_MODEL_ALGORITHMS
