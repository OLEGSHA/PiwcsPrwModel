#ifndef NODETYPEINFO_H
#define NODETYPEINFO_H

#include <piwcsprwmodel/nodes.h>

namespace piwcs::prw {

struct NodeTypeInfo {
    const char *name = {};
    std::size_t slotCount = {};
    bool allowedRoutes[Node::MAX_SLOTS][Node::MAX_SLOTS] = {};
};

} // namespace piwcs::prw

#endif // NODETYPEINFO_H
