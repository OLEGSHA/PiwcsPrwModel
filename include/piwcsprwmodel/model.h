#ifndef PIWCS_PRW_MODEL_MODEL
#define PIWCS_PRW_MODEL_MODEL

#include "fwd.h"
#include "util.h"
#include <unordered_map>

// Both required by unordered_map
#include "nodes.h"
#include "section.h"

/**
 * @file
 *
 * This header declares the Model class.
 */

namespace piwcs::prw {

/**
 * Model of the Public Railway (PRW) found on PIWCS.
 *
 * The model is fundamentally a directed graph, with switches and section
 * boundaries acting as nodes, and railway track sections acting as edges.
 *
 * A model that conforms to all Model invariants (see below) is said to be
 * _consistent_. A model that additionally has no empty slots is said to be
 * _complete_.
 *
 * @par Class-enforced invariants
 *   - Node identifiers are valid, non-null and globally unique
 *   - Section identifiers are valid, non-null and globally unique
 *   - All identifier slots are either empty or contain IDs of known entities
 *   - Nodes and section connection information is internally consistent
 *   - All Destination objects are valid
 *   - Destination addresses are globally unique
 */
class Model {

    std::unordered_map<Identifier, Node> m_nodes;
    std::unordered_map<Identifier, Section> m_sections;

  public:
    /**
     * Provides access to the internal mapping of IDs to all Nodes.
     *
     * The returned reference is valid as long as this Model object exists, and
     * its contents update dynamically as this Model object changes.
     *
     * @return a mapping of IDs to Nodes
     */
    const std::unordered_map<Identifier, Node> &nodes() const {
        return m_nodes;
    }

    /**
     * Provides access to the internal mapping of IDs to all Sections.
     *
     * The returned reference is valid as long as this Model object exists, and
     * its contents update dynamically as this Model object changes.
     *
     * @return a mapping of IDs to Sections
     */
    const std::unordered_map<Identifier, Section> &sections() const {
        return m_sections;
    }

    /**
     * Possible return values of `add*` methods.
     */
    enum class AddResult {
        /**
         * Addition was successful, the Model has changed.
         */
        OK,

        /**
         * Addition was aborted because the ID is null or invalid.
         */
        BAD_ID,

        /**
         * Addition was aborted because an entity with an identical ID is
         * already present.
         */
        DUPLICATE,

        /**
         * Addition was aborted because the entity being added has some non-null
         * references to other entities.
         */
        HAS_REF
    };

    /**
     * Adds an existing Node to the model.
     *
     * The operation may fail if the node with given ID already exists
     * (`DUPLICATE`), or if the node references any entities (`HAS_REF`). Should
     * addition fail, all objects remain valid and no change is made.
     *
     * @param node the Node to add
     *
     * @return `OK` upon success, `DUPLICATE` or `UNKNOWN_REF` upon failure
     */
    AddResult addNode(Node node);

    /**
     * Adds an existing Section to the model.
     *
     * The operation may fail if the section with given ID already exists
     * (`DUPLICATE`), if the section is a destination and another destination
     * with the same address already exists (`DUPLICATE`), or if the section
     * references any entities (`HAS_REF`). Should addition fail, all objects
     * remain valid and no change is made.
     *
     * @param node the Section to add
     *
     * @return `OK` upon success, `DUPLICATE` or `UNKNOWN_REF` upon failure
     */
    AddResult addSection(Section section);

    /**
     * Possible return values of `remove*` methods.
     */
    enum class RemoveResult {
        /**
         * Removal was successful, the Model has changed.
         */
        OK,

        /**
         * Removal has failed because an entity with such ID could not be found.
         */
        NOT_FOUND,

        /**
         * Removal was aborted because other entities exist that reference the
         * requested entity.
         */
        REFERENCED
    };

    /**
     * Removes a Node from the model.
     *
     * The operation may fail if the node with given ID cannot be found
     * (`NOT_FOUND`), or if the node is still referenced by some entity
     * (`REFERENCED`). Should removal fail, all objects remain valid and no
     * change is made.
     *
     * @param id the ID of the Node to remove
     *
     * @return `OK` upon success, `NOT_FOUND` or `REFERENCED` upon failure
     */
    RemoveResult removeNode(const Identifier &id);

    /**
     * Removes a Section from the model.
     *
     * The operation may fail if the section with given ID cannot be found
     * (`NOT_FOUND`), or if the section is still referenced by some entity
     * (`REFERENCED`). Should removal fail, all objects remain valid and no
     * change is made.
     *
     * @param id the ID of the Section to remove
     *
     * @return `OK` upon success, `NOT_FOUND` or `REFERENCED` upon failure
     */
    RemoveResult removeSection(const Identifier &id);

    /**
     * Possible return values of `link` method.
     */
    enum class LinkResult {
        /**
         * Linkage was successful, the Model has changed.
         */
        OK,

        /**
         * Linkage was aborted because some of the nodes, section or slots could
         * not be found.
         */
        NOT_FOUND,

        /**
         * Linkage was aborted because one of the nodes is already occupied.
         */
        NODE_OCCUPIED,

        /**
         * Linkage was aborted because the section is already occupied.
         */
        SECTION_OCCUPIED,

        /**
         * Linkage was aborted because the start and the end are the same slot
         * of the same node.
         */
        SAME_SLOT
    };

    /**
     * Links two Nodes using a Section.
     *
     * The operation may fail if some of the IDs cannot be resolved to existing
     * entities (`NOT_FOUND`), some requested slot does not exist in its Node
     * (`NOT_FOUND`), some slot is already connected (`NODE_OCCUPIED`), the
     * section is already linking some Nodes (`SECTION_OCCUPIED`), or
     * `startNode == endNode && startSlotId == endSlotId` (`SAME_SLOT`).
     *
     * @param sectionId ID of the section to use
     * @param startNodeId ID of the Node to connect to the start of the section
     * @param startSlotId SlotId of the start Node to connect to
     * @param endNodeId ID of the Node to connect to the end of the section
     * @param endSlotId SlotId of the end Node to connect to
     *
     * @return `OK` upon success, `NOT_FOUND`, `NODE_OCCUPIED`,
     * `SECTION_OCCUPIED` or `SAME_SLOT` upon failure
     */
    LinkResult link(const Identifier &sectionId, const Identifier &startNodeId,
                    SlotId startSlotId, const Identifier &endNodeId,
                    SlotId endSlotId);

    /**
     * Searches for a Node with the given ID.
     *
     * The returned pointer is valid until a change is made to this Model
     * object.
     *
     * @param id the ID of the Node to find
     *
     * @return a raw pointer to the Node, or `nullptr` if none found
     */
    const Node *node(const Identifier &id) const;

    /**
     * Searches for a Section with the given ID.
     *
     * The returned pointer is valid until a change is made to this Model
     * object.
     *
     * @param id the ID of the Section to find
     *
     * @return a raw pointer to the Section, or `nullptr` if none found
     */
    const Section *section(const Identifier &id) const;

    /**
     * Searches for a Node with the given ID.
     *
     * The returned pointer is valid until a change is made to this Model
     * object.
     *
     * @param id the ID of the Node to find
     *
     * @return a raw pointer to the Node, or `nullptr` if none found
     */
    Node *node(const Identifier &id);

    /**
     * Searches for a Section with the given ID.
     *
     * The returned pointer is valid until a change is made to this Model
     * object.
     *
     * @param id the ID of the Section to find
     *
     * @return a raw pointer to the Section, or `nullptr` if none found
     */
    Section *section(const Identifier &id);
};

inline bool operator!(Model::AddResult r) { return r != Model::AddResult::OK; }

inline bool operator!(Model::RemoveResult r) {
    return r != Model::RemoveResult::OK;
}

inline bool operator!(Model::LinkResult r) {
    return r != Model::LinkResult::OK;
}

} // namespace piwcs::prw

#endif // PIWCS_PRW_MODEL_MODEL
