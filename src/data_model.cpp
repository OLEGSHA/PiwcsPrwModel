#include <PiwcsPrwModel>

#include <iostream>

namespace piwcs::prw {

bool isId(const Identifier &id) { return !id.empty() && id[0] != '#'; }

bool isIdOrNull(const Identifier &id) { return id.empty() || isId(id); }

Model::AddResult Model::addNode(std::unique_ptr<Node> node) {

    // Check for duplicate IDs
    if (this->node(node->id()) != nullptr) {
        return ADD_DUPLICATE;
    }

    // Check for non-null section IDs
    size_t count = node->sectionCount();
    for (size_t i = 0; i < count; i++) {
        if (isId(node->section(i))) {
            return ADD_HAS_REF;
        }
    }

    m_nodes[node->id()] = std::move(node);
    return ADD_OK;
}

Model::RemoveResult Model::removeNode(const Identifier &id) {

    auto it = m_nodes.find(id);

    if (it == m_nodes.end()) {
        return REMOVE_NOT_FOUND;
    }

    const Node &node = *it->second;

    // Check for non-null section IDs
    // This is equivalent to searching for references to node but much faster
    size_t count = node.sectionCount();
    for (size_t i = 0; i < count; i++) {
        if (isId(node.section(i))) {
            return REMOVE_REFERENCED;
        }
    }

    m_nodes.erase(it);
    return REMOVE_OK;
}

const Node *Model::node(const Identifier &id) {
    auto it = m_nodes.find(id);
    return it == m_nodes.end() ? nullptr : it->second.get();
}

const Section *Model::section(const Identifier &id) {
    auto it = m_sections.find(id);
    return it == m_sections.end() ? nullptr : it->second.get();
}

ThruNode::ThruNode(const Identifier &id) : Node(THRU, id) {
    m_sections[0] = ID_NULL;
    m_sections[1] = ID_NULL;
}

size_t ThruNode::sectionCount() const { return 2; }

const Identifier &ThruNode::section(size_t index) const {
    if (index > 2) {
        return ID_INVALID;
    }
    return m_sections[index];
}

bool ThruNode::couldTraverse(size_t from, size_t to) const {
    return (from == 0 && to == 1) || (from == 1 && to == 0);
}

void ThruNode::print(std::ostream &out) const {
    out << id() << " [THRU; " << section(0) << ", " << section(1) << "]";
}

} // namespace piwcs::prw
