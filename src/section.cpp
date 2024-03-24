#include <piwcsprwmodel/section.h>

#include "debug.h"

namespace piwcs::prw {

Section::Section(Identifier id, AllowedTravel dir,
                 std::unique_ptr<Destination> dest)
    : m_id(std::move(id)), m_start(ID_NULL), m_end(ID_NULL), m_dir(dir),
      m_dest(std::move(dest)) {}

bool Section::canTraverse(SlotId from, SlotId to) const {
    switch (m_dir) {
    case AllowedTravel::NONE:
        return false;
    case AllowedTravel::UNIDIR:
        return from == 0 && to == 1;
    case AllowedTravel::BIDIR:
        return (from == 0 && to == 1) || (from == 1 && to == 0);
    }
    _FAIL("Unhandled AllowedTravel case");
    return false; // Unreachable
}

} // namespace piwcs::prw
