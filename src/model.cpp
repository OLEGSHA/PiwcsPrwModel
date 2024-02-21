#include <PiwcsPrwModel>

namespace piwcs::prw {

Model::AddResult Model::addNode(Node node) {

    // Check ID
    if (!isId(node.id())) {
        return AddResult::BAD_ID;
    }

    // Check for duplicate IDs
    if (this->node(node.id()) != nullptr) {
        return AddResult::DUPLICATE;
    }

    // Check for non-null section IDs
    size_t count = node.sectionCount();
    for (size_t i = 0; i < count; i++) {
        if (isId(node.section(i))) {
            return AddResult::HAS_REF;
        }
    }

    m_nodes.emplace(node.id(), std::move(node));
    return AddResult::OK;
}

Model::AddResult Model::addSection(Section section) {

    // Check ID
    if (!isId(section.id())) {
        return AddResult::BAD_ID;
    }

    // Check for duplicate IDs
    if (this->section(section.id()) != nullptr) {
        return AddResult::DUPLICATE;
    }

    // Check for duplicate destination address
    if (section.isDestination()) {
        const auto &address = section.destination()->address();
        for (const auto &it : sections()) {
            const auto &otherSection = it.second;
            if (otherSection.isDestination() &&
                otherSection.destination()->address() == address) {
                return AddResult::DUPLICATE;
            }
        }
    }

    // Check for non-null node IDs
    if (isId(section.start()) || isId(section.end())) {
        return AddResult::HAS_REF;
    }

    m_sections.emplace(section.id(), std::move(section));
    return AddResult::OK;
}

Model::RemoveResult Model::removeNode(const Identifier &id) {

    auto it = m_nodes.find(id);

    if (it == m_nodes.end()) {
        return RemoveResult::NOT_FOUND;
    }

    const Node &node = it->second;

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

    const Section &section = it->second;

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

    Section *section = this->section(sectionId);
    Node *start = this->node(startNodeId);
    Node *end = this->node(endNodeId);

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

    start->m_slots[startSlot] = sectionId;
    end->m_slots[endSlot] = sectionId;
    section->m_start = startNodeId;
    section->m_end = endNodeId;

    return LinkResult::OK;
}

const Node *Model::node(const Identifier &id) const {
    auto it = m_nodes.find(id);
    return it == m_nodes.end() ? nullptr : &it->second;
}

const Section *Model::section(const Identifier &id) const {
    auto it = m_sections.find(id);
    return it == m_sections.end() ? nullptr : &it->second;
}

Node *Model::node(const Identifier &id) {
    auto it = m_nodes.find(id);
    return it == m_nodes.end() ? nullptr : &it->second;
}

Section *Model::section(const Identifier &id) {
    auto it = m_sections.find(id);
    return it == m_sections.end() ? nullptr : &it->second;
}

} // namespace piwcs::prw
