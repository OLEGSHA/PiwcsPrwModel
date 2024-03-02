#ifndef PIWCS_PRW_MODEL_NODES
#define PIWCS_PRW_MODEL_NODES

#include "fwd.h"
#include "metadata.h"
#include "util.h"
#include <iosfwd>

/**
 * @file
 *
 * This header declares the Node class and NodeType type.
 */

namespace piwcs::prw {

struct NodeTypeInfo;

/**
 * A kind of Node reflecting its role, structure and function.
 *
 * ### Switches
 *
 * A switch with three connected tracks. Switches can have one of three Types:
 * `MOTORIZED`, `PASSIVE` or `FIXED`.
 *
 * @par Switch structure
 *
    @dot
    graph switch {
        node [shape = plaintext];
        common [layer = 0];
        straight [layer = 2];
        diverging [layer = 2];
        switch [layer = 1];
        common -- switch -- straight;
        switch -- diverging [style = "dashed"];
    }
    @enddot
 *
 * Switches physically prohibit travel between straight and diverging tracks.
 * When a train enters the switch from its straight or diverging track, it is
 * directed into the common track. When a train enters the switch from its
 * common track, the switch chooses a track to direct it towards.
 *
 * `MOTORIZED` switches can only be entered from the common track. A routing
 * table consulted to determine whether the train should be diverted. For
 * motorized switches, straight track is the default direction.
 *
 * `PASSIVE` switches can only be exited from the common track. As such, they
 * require no controller. For passive switches, straight track choice is
 * arbitrary.
 *
 * `FIXED` switches can only be entered from the common track, and will always
 * direct all trains to the same exit. They are found at the edges of
 * bidirectional segments. For fixed switches, straight track is the exit for
 * trains entering through common track.
 *
 * (All other switch configurations are invalid on the PRW.)
 *
 * ### Crossings
 *
 * Crossing nodes indicate an intersection of tracks where trains on
 * intersecting tracks need to give way to one another. (Intersections where
 * trains can travel freely, e.g. by means of a bridge, should not be marked
 * with crossing nodes.)
 *
 * @par Crossing structure
 * Track A connects slots 0 and 1. Track B connects slots 2 and 3. Travel is
 * permitted on tracks A and B bidirectionally, but travelling between tracks
 * is forbidden.
 *
 */
using NodeType = const NodeTypeInfo *;

/**
 * A node connecting two sections of a single track.
 */
extern const NodeType THRU;

/**
 * A motorized switch operated by a routing table.
 */
extern const NodeType MOTORIZED;

/**
 * A switch that cannot be entered from the common track.
 */
extern const NodeType PASSIVE;

/**
 * A switch that is fixed in a single position that can be entered from
 * the common track.
 */
extern const NodeType FIXED;

/**
 * A level crossing of two tracks.
 */
extern const NodeType CROSSING;

/**
 * A dead end connected to a bidirectional section.
 */
extern const NodeType END;

/**
 * A Node at the joint or intersection of Sections.
 *
 * Nodes, as all Model entities, are mutable objects.
 */
class Node : public detail::HasMetadata {

  public:
    static constexpr std::size_t MAX_SLOTS = 4;

  private:
    NodeType m_type;
    Identifier m_id;
    Identifier m_slots[MAX_SLOTS];

  public:
    /**
     * Constructs a Node with the given type and ID.
     */
    Node(NodeType, Identifier);

    /**
     * Returns the Type of this Node.
     *
     * Node Type determines the role, structure and function of the node. Node
     * types are more specific than implementing classes, and multiple types can
     * be implemented by a single class. Use this value to differentiate between
     * different kinds of nodes.
     *
     * @return the Type of this Node
     */
    [[nodiscard]] NodeType type() const { return m_type; }

    /**
     * Returns the Identifier of this Node.
     *
     * @return the ID of this Node
     */
    [[nodiscard]] IdRef id() const { return m_id; }

    /**
     * Returns the number of Sections this Node connects.
     *
     * This value is constant for a given Type.
     *
     * @return the number of Sections Nodes of this Type connect
     */
    [[nodiscard]] SlotId sectionCount() const;

    /**
     * Returns the ID of the section in the requested slot, `ID_NULL` if the
     * slot is not yet connected, or `ID_INVALID` if no such slot exists.
     *
     * @param slot the slot ID to inspect
     *
     * @return the ID of the requested section, `ID_NULL` or `ID_INVALID`
     */
    [[nodiscard]] IdRef section(SlotId slot) const {
        return slot > sectionCount() ? ID_INVALID : m_slots[slot];
    }

    /**
     * Returns the slot ID that the Section with the provided ID occupies in
     * this Node, or `SLOT_INVALID` if the connection could not be found.
     *
     * Result is unspecified when `sectionId` is `ID_NULL`.
     *
     * @param sectionId the ID of the section to look for
     *
     * @return slot ID _S_ such that `section(S) == sectionId` or `SLOT_INVALID`
     */
    [[nodiscard]] SlotId slotOf(IdRef sectionId) const;

    /**
     * Determines whether travel from slot `from` to slot `to` would be possible
     * if appropriate section were connected in these slots.
     *
     * This method must not query actual sections connected, if any.
     *
     * This value is constant for a given (Type, `from`, `to`) tuple. `false` is
     * returned for invalid slots. Note that most, but not all Nodes will return
     * `false` when `from == to`.
     *
     * @return `true` if and only if there exists a configuration of Sections
     * that would permit travel from the Section at slot `from` to the Section
     * at slot `to`
     */
    [[nodiscard]] bool couldTraverse(SlotId from, SlotId to) const;

    /**
     * Outputs a textual representation of this Node to the `ostream`.
     */
    friend std::ostream &operator<<(std::ostream &, const Node &);

    friend class Model;
};

/**
 * SlotId of the common track.
 */
static constexpr SlotId COMMON = 0;

/**
 * SlotId of the straight track.
 */
static constexpr SlotId STRAIGHT = 1;

/**
 * SlotId of the diverging track.
 */
static constexpr SlotId DIVERGING = 2;

} // namespace piwcs::prw

#endif // PIWCS_PRW_MODEL_NODES
