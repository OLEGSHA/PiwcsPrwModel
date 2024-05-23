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
 * complete and all nodes are _locally correct_
 * (for definition of _local correctness_
 * see @ref piwcs::prw::isLocallyCorrect "isLocallyCorrect").
 *
 * @param model the Model to examine
 *
 * @return `true` if and only if the model is _correct_.
 */
bool isCorrect(const Model &model);

/**
 * Checks the node of a provided model referenced by provided id for
 * _local correctness_
 *
 * A node except for a bidirectional end is _locally correct_ if for every slot
 * with section in it allowing travel __into__ node there is another slot
 * reachable from first, and second slot has section that allows travel
 * __from__ node. A bidirectional end is correct iff its only slot is occupied
 * by forbidden or bidirectional section. A node with an empty slot is __not__
 * _locally correct_.
 *
 * @param model the Model containing the node to examine
 * @param id Identifier of the node to examine
 *
 * @return `true` if Node with given id exists in model and is
 * _locally correct_, `false` otherwise.
 *
 * @note
 * This will return `false` if node with provided id is not in model.
 */
bool isLocallyCorrect(const Model &model, IdRef id);

} // namespace piwcs::prw

#endif // PIWCS_PRW_MODEL_ALGORITHMS
