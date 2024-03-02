#include <piwcsprwmodel/metadata.h>

namespace piwcs::prw::detail {

bool HasMetadata::hasMetadata() const { return m_meta && !m_meta->empty(); }

Metadata &HasMetadata::metadata() {
    if (!m_meta) {
        m_meta.emplace();
    }
    return *m_meta;
}

const Metadata &HasMetadata::metadata() const {
    if (!m_meta) {
        static const Metadata EMPTY{};
        return EMPTY;
    }
    return *m_meta;
}

bool HasMetadata::hasMetadata(IdRef key) const {
    return m_meta && m_meta->contains(key);
}

std::string_view HasMetadata::metadata(IdRef key) const {
    if (!m_meta) {
        return "";
    }

    auto it = m_meta->find(key);
    if (it == m_meta->cend()) {
        return "";
    }

    return it->second;
}

std::string &HasMetadata::metadata(IdRef key) {
    auto &map = metadata();
    auto it = map.find(key);
    if (it == map.cend()) {
        return map.emplace(Identifier(key), std::string{}).first->second;
    }
    return it->second;
}

} // namespace piwcs::prw::detail
