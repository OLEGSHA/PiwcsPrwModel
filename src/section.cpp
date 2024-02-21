#include <piwcsprwmodel/section.h>

namespace piwcs::prw {

Section::Section(Identifier id, bool isBidir, Length length,
                 std::unique_ptr<Destination> dest)
    : m_id(std::move(id)), m_start(ID_NULL), m_end(ID_NULL),
      m_bidirectional(isBidir), m_length(length), m_dest(std::move(dest)) {}

} // namespace piwcs::prw
