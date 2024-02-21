#include <PiwcsPrwModel>

namespace piwcs::prw {

bool isId(const Identifier &id) { return !id.empty() && id[0] != '#'; }

bool isIdOrNull(const Identifier &id) { return id.empty() || isId(id); }

} // namespace piwcs::prw
