#ifndef PIWCS_PRW_MODEL_UTIL
#define PIWCS_PRW_MODEL_UTIL

#include "fwd.h"
#include <string>

/**
 * @file
 *
 * This header declares the utility functions and data types of piwcsprwmodel.
 */

namespace piwcs::prw {

/**
 * Maximum supported length of identifiers in UTF-8 bytes.
 */
constexpr std::size_t IDENT_LENGTH = 15;

/**
 * Type alias for identifiers. Effectively a string.
 */
using Identifier = std::string;

/**
 * Type alias for read-only identifier access.
 */
using IdRef = std::string_view;

/**
 * The Identifier value that should be used to express a lack of an identifier.
 */
static const Identifier ID_NULL;

/**
 * An invalid identifier.
 *
 * This is not the only possible invalid identifier; use `isId` to check for
 * validity.
 */
static const Identifier ID_INVALID = "#invalid";

/**
 * Determines if the provided Identifier is valid.
 *
 * Unlike `isId`, this function returns `true` for `ID_NULL`.
 *
 * @return whether the argument is a valid identifier
 */
bool isIdOrNull(IdRef);

/**
 * Determines if the provided Identifier is valid and not `ID_NULL`.
 *
 * @return whether the argument is a valid, non-null Identifier value
 *
 * @see isIdOrNull to only test validity
 */
bool isId(IdRef);

/**
 * Type alias for Node and Section slots. Effectively an unsigned integer.
 */
using SlotId = std::size_t;

} // namespace piwcs::prw

#endif // PIWCS_PRW_MODEL_UTIL
