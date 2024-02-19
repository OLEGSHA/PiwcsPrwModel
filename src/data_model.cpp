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
                              const Identifier &startNodeId, Slot startSlot,
                              const Identifier &endNodeId, Slot endSlot) {

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

ThruNode::ThruNode(const Identifier &id) : Node(Type::THRU, id) {
    m_sections[0] = ID_NULL;
    m_sections[1] = ID_NULL;
}

const Identifier &ThruNode::section(size_t index) const {
    if (index > 2) {
        return ID_INVALID;
    }
    return m_sections[index];
}

bool ThruNode::couldTraverse(size_t from, size_t to) const {
    return (from == 0 && to == 1) || (from == 1 && to == 0);
}

SwitchNode::SwitchNode(Type type, Identifier id)
    : Node(type, std::move(id)), m_common(ID_NULL), m_straight(ID_NULL),
      m_diverging(ID_NULL) {
#ifdef DEBUG
    switch (type) {
    case Type::MOTORIZED:
    case Type::PASSIVE:
    case Type::FIXED:
        break;
    default:
        throw std::invalid_argument("Illegal SwitchNode type");
    }
#endif
}

const Identifier &SwitchNode::section(size_t index) const {
    switch (index) {
    case 0:
        return m_common;
    case 1:
        return m_straight;
    case 2:
        return m_diverging;
    default:
        return ID_INVALID;
    }
}

bool SwitchNode::couldTraverse(size_t from, size_t to) const {
    if (from == to || from > 2 || to > 2) {
        return false;
    }

    switch (type()) {
    case Type::MOTORIZED:
        return to == 0;
    case Type::PASSIVE:
        return from == 0;
    default:
        return to == 0 || (from == 0 && to == 1);
    }
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
    node.print(out);
    return out;
}

std::ostream &operator<<(std::ostream &out, const Section &section) {
    out << fmt(section.id()) << " [Section; " << fmt(section.start()) << ", "
        << fmt(section.end()) << "]";
    return out;
}

void ThruNode::print(std::ostream &out) const {
    out << id() << " [THRU; " << fmt(section(0)) << ", " << fmt(section(1))
        << "]";
}

void SwitchNode::print(std::ostream &out) const {
    out << id();

    switch (type()) {
    case Type::MOTORIZED:
        out << " [MOTORIZED; ";
        break;
    case Type::PASSIVE:
        out << " [PASSIVE; ";
        break;
    default:
        out << " [PASSIVE; ";
        break;
    }

    out << "C/S/D: " << fmt(common()) << ", " << fmt(straight()) << ", "
        << fmt(diverging()) << "]";
}

} // namespace piwcs::prw
