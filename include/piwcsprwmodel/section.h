#ifndef PIWCS_PRW_MODEL_SECTION
#define PIWCS_PRW_MODEL_SECTION

#include "fwd.h"
#include "metadata.h"
#include "util.h"
#include <iosfwd>
#include <memory>

/**
 * @file
 *
 * This header declares the Section and Destination classes.
 */

namespace piwcs::prw {

/**
 * A Section connecting exactly two Nodes.
 *
 * Sections usually allow travel in one direction only, from start to end.
 * If not, the Section is said to be bidirectional, in which case start and end
 * are chosen arbitrarily.
 *
 * Some Sections are destinations and own a Destination object.
 *
 * Sections, as all Model entities, are mutable objects.
 */
class Section : public detail::HasMetadata {

  public:
    /**
     * Allowed travel directions for routed trains.
     */
    enum class AllowedTravel {
        /**
         * Travel of routed trains through this Section is disallowed.
         */
        NONE,

        /**
         * Routed trains may only proceed from start to end.
         */
        UNIDIR,

        /**
         * Routed trains may proceed from start to end or in reverse.
         */
        BIDIR
    };

  private:
    Identifier m_id;

    Identifier m_start;
    Identifier m_end;
    AllowedTravel m_dir;

    std::unique_ptr<Destination> m_dest;

  public:
    /**
     * Constructs a new Section with the given ID, directionality, and,
     * optionally, destination data.
     *
     * @param id ID of the section
     * @param dir directionality of the section
     * @param dest destination data for this section or `nullptr`
     */
    Section(Identifier id, AllowedTravel dir,
            std::unique_ptr<Destination> dest);

    /**
     * Consturcts a new unidirectional Section with the given ID.
     *
     * The new Section is not a destination.
     *
     * @param id ID of the section
     */
    Section(Identifier id)
        : Section(std::move(id), AllowedTravel::UNIDIR, nullptr) {}

    /**
     * Returns the Identifier of this Section.
     *
     * @return the ID of this section
     */
    [[nodiscard]] IdRef id() const { return m_id; }

    /**
     * Returns the ID of the Node at the start of this Section.
     *
     * Equivalent to `node(0)`.
     *
     * @return the ID of the start Node or `ID_NULL`
     */
    [[nodiscard]] IdRef start() const { return m_start; }

    /**
     * Returns the ID of the Node at the end of this Section.
     *
     * Equivalent to `node(1)`.
     *
     * @return the ID of the end Node or `ID_NULL`
     */
    [[nodiscard]] IdRef end() const { return m_end; }

    /**
     * Returns the directionality of this Section for routed trains.
     *
     * @return allowed travel directions
     */
    [[nodiscard]] AllowedTravel dir() const { return m_dir; }

    /**
     * Returns `true` iff the Section is bidirectional, i.e. allows travel from
     * start to end and from end to start.
     *
     * @return whether this Section is bidirectional
     */
    [[nodiscard]] bool isBidir() const { return m_dir == AllowedTravel::BIDIR; }

    /**
     * Returns `true` iff the Section is unidirectional, i.e. allows travel from
     * start to end.
     *
     * @return whether this Section is unidirectional
     */
    [[nodiscard]] bool isUnidir() const {
        return m_dir == AllowedTravel::UNIDIR;
    }

    /**
     * Returns `true` iff the Section allows the travel of routed trains.
     *
     * @return whether this Section is unidirectional or bidirectional
     */
    [[nodiscard]] bool allowsTravel() const {
        return m_dir != AllowedTravel::NONE;
    }

    /**
     * Returns the ID of the node at the given index. This is a convenience
     * method to generalize `start()` and `end()`.
     *
     * Index 0 corresponds to `start`, index 1 corresponds to `end`. All other
     * indices return `ID_INVALID`.
     *
     * @param index index of the Node slot to return
     *
     * @return the ID of the Node in the slot with index `index`, `ID_NULL` if
     * the slot is empty, or `ID_INVALID` if the slot does not exist
     */
    [[nodiscard]] IdRef node(SlotId index) const {
        switch (index) {
        case 0:
            return start();
        case 1:
            return end();
        default:
            return ID_INVALID;
        }
    }

    /**
     * Checks whether this section is connected to nodes.
     *
     * @return `true` if this section is connected to nodes
     */
    [[nodiscard]] bool isConnected() const { return m_start != ID_NULL; }

    /**
     * Checks whether travel is allowed from Node in slot with index `from` to
     * Node in slot with index `to`.
     *
     * See @ref Section::node for layout.
     *
     * @param from index of starting Node
     * @param to index of ending Node
     *
     * @return `true` if and only if travel is possible and allowed in given
     * direction
     */
    [[nodiscard]] bool canTraverse(SlotId from, SlotId to) const;

    /**
     * Returns `true` iff this Section has Destination data.
     *
     * @return whether this Section is a destination
     */
    [[nodiscard]] bool isDestination() const {
        return static_cast<bool>(m_dest);
    }

    /**
     * Returns the Destination data for this Section, if any.
     *
     * The returned pointer is valid until `setDestination` is called on this
     * Section object or this Section object is destroyed.
     *
     * @return a raw pointer to the Destination object describing this
     * destination, or `nullptr`
     */
    [[nodiscard]] const Destination *destination() const {
        return m_dest.get();
    }

    /**
     * Outputs a textual representation of this Section to the `ostream`.
     */
    friend std::ostream &operator<<(std::ostream &, const Section &);

    friend class Model;
};

/**
 * A description of a destination.
 *
 * These objects may be attached to Sections to turn them into destinations.
 *
 * Destinations are immutable objects.
 */
class Destination : public detail::HasMetadata {

  public:
    /**
     * Type alias for addresses. Effectively a string.
     */
    using Address = std::string;

    /**
     * Type alias for names. Effectively a string.
     */
    using Name = std::string;

  private:
    Address m_address;
    Name m_name;

  public:
    /**
     * Creates a new Destination with given address and name.
     */
    Destination(Address address, Name name)
        : m_address(std::move(address)), m_name(std::move(name)) {}

    /**
     * Returns the address of this Destination.
     *
     * An address is a machine-friendly identifier of a destination. They are
     * used for routing.
     *
     * Addresses must be ASCII-encoded strings.
     *
     * @return the address of this Destination
     */
    [[nodiscard]] const Address &address() const { return m_address; }

    /**
     * Returns the name of this Destination.
     *
     * A name is a user-friendly label for the destination.
     *
     * Names must not be inspected and may use any encoding.
     *
     * @return the name of this Destination
     */
    [[nodiscard]] const Name &name() const { return m_name; }
};

} // namespace piwcs::prw

#endif // PIWCS_PRW_MODEL_SECTION
