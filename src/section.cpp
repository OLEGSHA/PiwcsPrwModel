#include <piwcsprwmodel/section.h>

namespace piwcs::prw {

Section::Section(Identifier id, AllowedTravel dir, Length length,
                 std::unique_ptr<Destination> dest)
    : m_id(std::move(id)), m_start(ID_NULL), m_end(ID_NULL), m_dir(dir),
      m_length(length), m_dest(std::move(dest)) {}

} // namespace piwcs::prw
