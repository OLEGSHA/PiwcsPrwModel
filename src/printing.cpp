#include <piwcsprwmodel/nodes.h>
#include <piwcsprwmodel/section.h>

#include "nodetypeinfo.h"
#include <iostream>

namespace piwcs::prw {

namespace {
const char *fmt(const Identifier &id) {
    if (isIdOrNull(id) && !isId(id)) {
        return "#null";
    }

    return id.c_str();
}
} // namespace

std::ostream &operator<<(std::ostream &out, const Node &node) {
    const auto &type = *node.type();

    out << '[' << type.name << " node " << node.id() << ' ';

    out << fmt(node.section(0));
    for (SlotId i = 1; i < type.slotCount; i++) {
        out << '/' << fmt(node.section(i));
    }

    out << ']';

    return out;
}

std::ostream &operator<<(std::ostream &out, const Section &section) {
    out << "[Section " << section.id() << ' ' << fmt(section.start()) << '/'
        << fmt(section.end()) << ']';
    return out;
}

} // namespace piwcs::prw
