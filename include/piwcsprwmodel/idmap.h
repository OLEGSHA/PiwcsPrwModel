#ifndef PIWCS_PRW_MODEL_IDMAP
#define PIWCS_PRW_MODEL_IDMAP

#include "fwd.h"
#include "util.h"
#include <unordered_map>

/**
 * @file
 *
 * This header declares IdMap, a type alias for `std::unordered_map` that uses
 * Identifier as keys and allows lookup with IdRef.
 */

namespace piwcs::prw {

/**
 * An unordered map with Identifiers as keys.
 */
template <typename V>
using IdMap = std::unordered_map<Identifier, V, IdHash, std::equal_to<>>;

} // namespace piwcs::prw

#endif // PIWCS_PRW_MODEL_IDMAP
