#include <piwcsprwmodel/nodes.h>

#include "debug.h"
#include "nodetypeinfo.h"

namespace piwcs::prw {

Node::Node(NodeType type, Identifier id)
    : m_type(type), m_id(std::move(id)), m_slots{} {}

SlotId Node::sectionCount() const { return type()->slotCount; };

SlotId Node::slotOf(IdRef sectionId) const {
    std::size_t count = sectionCount();
    for (SlotId i = 0; i < count; i++) {
        if (m_slots[i] == sectionId) {
            return i;
        }
    }
    return SLOT_INVALID;
}

bool Node::couldTraverse(SlotId from, SlotId to) const {
    const auto &type = *this->type();
    if (from >= type.slotCount || to >= type.slotCount) {
        return false;
    }

    _ASSERT(from < Node::MAX_SLOTS, "slotCount >= MAX_SLOTS");
    _ASSERT(to < Node::MAX_SLOTS, "slotCount >= MAX_SLOTS");

    return type.allowedRoutes[from][to];
}

// clang-format off
static constexpr NodeTypeInfo THRU_INFO {
    "THRU", 2,
    {
        // 0 <-> 1
        { false, true,  },
        { true,  false, },
    }
};
extern const NodeType THRU = &THRU_INFO;

static constexpr NodeTypeInfo MOTORIZED_INFO {
    "MOTORIZED", 3,
    {
        // 0 -> 1, 0 -> 2
        { false, true,  true,  },
        { false, false, false, },
        { false, false, false, },
    }
};
extern const NodeType MOTORIZED = &MOTORIZED_INFO;

static constexpr NodeTypeInfo PASSIVE_INFO {
    "PASSIVE", 3,
    {
        // 1 -> 0, 2 -> 0
        { false, false, false, },
        { true,  false, false, },
        { true,  false, false, },
    }
};
extern const NodeType PASSIVE = &PASSIVE_INFO;

static constexpr NodeTypeInfo FIXED_INFO {
    "FIXED", 3,
    {
        // 0 -> 1, 2 -> 0
        { false, true,  false, },
        { false, false, false, },
        { true,  false, false, },
    }
};
extern const NodeType FIXED = &FIXED_INFO;

static constexpr NodeTypeInfo CROSSING_INFO {
    "CROSSING", 4,
    {
        // 0 <-> 1, 2 <-> 3
        { false, true,  false, false, },
        { true,  false, false, false, },
        { false, false, false, true,  },
        { false, false, true,  false, },
    }
};
extern const NodeType CROSSING = &CROSSING_INFO;

static constexpr NodeTypeInfo END_INFO {
    "END", 1,
    {
        { false, },
    }
};
extern const NodeType END = &END_INFO;
// clang-format on

} // namespace piwcs::prw
