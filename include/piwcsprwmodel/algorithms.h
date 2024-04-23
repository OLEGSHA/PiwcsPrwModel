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
 * To be _correct_, model must be complete and all allowed
 * (see @ref piwcsprwmodel-allowed-travel-and-correctness
 * "Allowed Travel and Correctness" ) paths must have no dead ends (i.e. places
 * from which a train has no valid path) or unreachable starting positions
 * except for bidirectional end nodes. Thus, a model is _correct_ if it is
 * complete and all nodes are _locally correct_ (see note below).
 *
 * @note
 * A node is _locally correct_ if for every section allowing travel __into__
 * node there is a valid path to some section (even the same one) allowing
 * travel __from__ node except for bidirectional ends.
 *
 * @note
 * Bidirectional end is correct if it is connected to forbidden or bidirectional
 * section.
 *
 * @param model the Model to examine
 *
 * @return `true` if and only if the model is _correct_.
 */
bool isCorrect(const Model &model);

} // namespace piwcs::prw

#endif // PIWCS_PRW_MODEL_ALGORITHMS
