#include "debug.h"
#include <piwcsprwmodel/model.h>
#include <piwcsprwmodel/nodes.h>
#include <piwcsprwmodel/section.h>

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
    SlotId count = node.sectionCount();
    for (SlotId i = 0; i < count; i++) {
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
        for (const auto &[_, otherSection] : sections()) {
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

Model::RemoveResult Model::removeNode(IdRef id) {

    auto it = m_nodes.find(id);

    if (it == m_nodes.end()) {
        return RemoveResult::NOT_FOUND;
    }

    const Node &node = it->second;

    // Check for non-null section IDs
    // This is equivalent to searching for references to node but much faster
    SlotId count = node.sectionCount();
    for (SlotId i = 0; i < count; i++) {
        if (isId(node.section(i))) {
            return RemoveResult::REFERENCED;
        }
    }

    m_nodes.erase(it);
    return RemoveResult::OK;
}

Model::RemoveResult Model::removeSection(IdRef id) {

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

Model::LinkResult Model::link(IdRef sectionId, IdRef startNodeId,
                              SlotId startSlot, IdRef endNodeId,
                              SlotId endSlot) {

    Section *section = this->section(sectionId);
    Node *start = this->node(startNodeId);
    Node *end = this->node(endNodeId);

    if (section == nullptr || start == nullptr || end == nullptr ||
        startSlot >= start->sectionCount() || endSlot >= end->sectionCount()) {
        return LinkResult::NOT_FOUND;
    }

    if (start == end) {
        return LinkResult::SAME_NODE;
    }

    if (start->section(startSlot) != ID_NULL ||
        end->section(endSlot) != ID_NULL) {
        return LinkResult::NODE_OCCUPIED;
    }

    if (section->isConnected()) {
        return LinkResult::SECTION_OCCUPIED;
    }

    _DEBUG_ONLY() if (section->end() != ID_NULL) {
        _FAIL("section->start() == ID_NULL, section->end() != ID_NULL");
    }

    start->m_slots[startSlot] = sectionId;
    end->m_slots[endSlot] = sectionId;
    section->m_start = startNodeId;
    section->m_end = endNodeId;

    return LinkResult::OK;
}

Model::UnlinkResult Model::unlink(IdRef sectionId) {
    Section *section = this->section(sectionId);
    if (section == nullptr) {
        return UnlinkResult::NOT_FOUND;
    }

    if (!section->isConnected()) {
        return UnlinkResult::NOT_LINKED;
    }

    _DEBUG_ONLY() if (section->end() != ID_NULL) {
        _FAIL("section->start() != ID_NULL, section->end() == ID_NULL");
    }

    _DEBUG_ONLY() if (section->start() == section->end()) {
        _FAIL("section->start() == section->end()");
    }

    Node *start = this->node(section->start());
    Node *end = this->node(section->end());

    _ASSERT(start != nullptr, "start node not found");
    _ASSERT(end != nullptr, "end node not found");

    for (auto *node : {start, end}) {
        _DEBUG_ONLY(bool found);

        for (auto &slot : node->m_slots) { // NOLINT *-array-to-pointer-decay
            if (slot == sectionId) {
                slot = ID_NULL;
                _DEBUG_ONLY(found = true);
                break;
            }
        }

        _ASSERT(found, "node did not connect to unlinked section");
    }

    section->m_start = ID_NULL;
    section->m_end = ID_NULL;

    return UnlinkResult::OK;
}

const Node *Model::node(IdRef id) const {
    auto it = m_nodes.find(id);
    return it == m_nodes.end() ? nullptr : &it->second;
}

const Section *Model::section(IdRef id) const {
    auto it = m_sections.find(id);
    return it == m_sections.end() ? nullptr : &it->second;
}

Node *Model::node(IdRef id) {
    auto it = m_nodes.find(id);
    return it == m_nodes.end() ? nullptr : &it->second;
}

Section *Model::section(IdRef id) {
    auto it = m_sections.find(id);
    return it == m_sections.end() ? nullptr : &it->second;
}

} // namespace piwcs::prw
