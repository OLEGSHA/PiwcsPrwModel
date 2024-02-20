#include <PiwcsPrwModel>

#include <iostream>

#ifdef DEBUG
#include <stdexcept>
#endif

namespace piwcs::prw {

bool isId(const Identifier &id) { return !id.empty() && id[0] != '#'; }

bool isIdOrNull(const Identifier &id) { return id.empty() || isId(id); }

Model::AddResult Model::addNode(std::unique_ptr<Node> node) {

    // Check ID
    if (!isId(node->id())) {
        return AddResult::BAD_ID;
    }

    // Check for duplicate IDs
    if (this->node(node->id()) != nullptr) {
        return AddResult::DUPLICATE;
    }

    // Check for non-null section IDs
    size_t count = node->sectionCount();
    for (size_t i = 0; i < count; i++) {
        if (isId(node->section(i))) {
            return AddResult::HAS_REF;
        }
    }

    m_nodes[node->id()] = std::move(node);
    return AddResult::OK;
}

Model::AddResult Model::addSection(std::unique_ptr<Section> section) {

    // Check ID
    if (!isId(section->id())) {
        return AddResult::BAD_ID;
    }

    // Check for duplicate IDs
    if (this->section(section->id()) != nullptr) {
        return AddResult::DUPLICATE;
    }

    // Check for duplicate destination address
    if (section->isDestination()) {
        const auto &address = section->destination()->address();
        for (const auto &it : sections()) {
            const auto &otherSection = *it.second;
            if (otherSection.isDestination() &&
                otherSection.destination()->address() == address) {
                return AddResult::DUPLICATE;
            }
        }
    }

    // Check for non-null node IDs
    if (isId(section->start()) || isId(section->end())) {
        return AddResult::HAS_REF;
    }

    m_sections[section->id()] = std::move(section);
    return AddResult::OK;
}

Model::RemoveResult Model::removeNode(const Identifier &id) {

    auto it = m_nodes.find(id);

    if (it == m_nodes.end()) {
        return RemoveResult::NOT_FOUND;
    }

    const Node &node = *it->second;

    // Check for non-null section IDs
    // This is equivalent to searching for references to node but much faster
    size_t count = node.sectionCount();
    for (size_t i = 0; i < count; i++) {
        if (isId(node.section(i))) {
            return RemoveResult::REFERENCED;
        }
    }

    m_nodes.erase(it);
    return RemoveResult::OK;
}

Model::RemoveResult Model::removeSection(const Identifier &id) {

    auto it = m_sections.find(id);

    if (it == m_sections.end()) {
        return RemoveResult::NOT_FOUND;
    }

    const Section &section = *it->second;

    // Check for non-null section IDs
    // This is equivalent to searching for references to section but much faster
    if (isId(section.start()) || isId(section.end())) {
        return RemoveResult::REFERENCED;
    }

    m_sections.erase(it);
    return RemoveResult::OK;
}

Model::LinkResult Model::link(const Identifier &sectionId,
                              const Identifier &startNodeId, SlotId startSlot,
                              const Identifier &endNodeId, SlotId endSlot) {

    const Section *section = this->section(sectionId);
    const Node *start = this->node(startNodeId);
    const Node *end = this->node(endNodeId);

    if (section == nullptr || start == nullptr || end == nullptr ||
        startSlot >= start->sectionCount() || endSlot >= end->sectionCount()) {
        return LinkResult::NOT_FOUND;
    }

    if (start == end && startSlot == endSlot) {
        return LinkResult::SAME_SLOT;
    }

    if (start->section(startSlot) != ID_NULL ||
        end->section(endSlot) != ID_NULL) {
        return LinkResult::NODE_OCCUPIED;
    }

    // Check for section.end() is redundant
    if (section->start() != ID_NULL) {
        return LinkResult::SECTION_OCCUPIED;
    }

    // TODO implement

    return LinkResult::OK;
}

const Node *Model::node(const Identifier &id) {
    auto it = m_nodes.find(id);
    return it == m_nodes.end() ? nullptr : it->second.get();
}

const Section *Model::section(const Identifier &id) {
    auto it = m_sections.find(id);
    return it == m_sections.end() ? nullptr : it->second.get();
}

struct NodeTypeInfo {
    const char *name = {};
    size_t slotCount = {};
    bool allowedRoutes[Node::MAX_SLOTS][Node::MAX_SLOTS] = {};
};

Node::Node(NodeType type, Identifier id)
    : m_type(type), m_id(std::move(id)), m_slots{} {}

SlotId Node::sectionCount() const { return type()->slotCount; };

bool Node::couldTraverse(SlotId from, SlotId to) const {
    const auto &type = *this->type();
    if (from >= type.slotCount || to >= type.slotCount) {
        return false;
    }
    return type.allowedRoutes[from][to];
}

Section::Section(Identifier id, bool isBidir, Length length,
                 std::unique_ptr<Destination> dest)
    : m_id(std::move(id)), m_start(ID_NULL), m_end(ID_NULL),
      m_bidirectional(isBidir), m_length(length), m_dest(std::move(dest)) {}

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

// TODO design and implement CROSSING nodes
extern const NodeType CROSSING = nullptr;

static constexpr NodeTypeInfo END_INFO {
    "END", 1,
    {
        { false, },
    }
};
extern const NodeType END = &END_INFO;
// clang-format on

} // namespace piwcs::prw
