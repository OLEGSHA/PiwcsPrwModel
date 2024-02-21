#ifndef NODES_H
#define NODES_H

#include <PiwcsPrwModel>

namespace piwcs::prw {

struct NodeTypeInfo {
    const char *name = {};
    size_t slotCount = {};
    bool allowedRoutes[Node::MAX_SLOTS][Node::MAX_SLOTS] = {};
};

} // namespace piwcs::prw

#endif
