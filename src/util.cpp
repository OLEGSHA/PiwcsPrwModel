#include <piwcsprwmodel/util.h>

namespace piwcs::prw {

bool isId(IdRef id) { return !id.empty() && id[0] != '#'; }

bool isIdOrNull(IdRef id) { return id.empty() || isId(id); }

} // namespace piwcs::prw
