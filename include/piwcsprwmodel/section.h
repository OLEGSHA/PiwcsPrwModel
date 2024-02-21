#ifndef PIWCS_PRW_MODEL_SECTION
#define PIWCS_PRW_MODEL_SECTION

#include "fwd.h"
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
class Section {

  public:
    using Length = uint32_t;

  private:
    Identifier m_id;

    Identifier m_start;
    Identifier m_end;
    bool m_bidirectional;

    Length m_length;

    std::unique_ptr<Destination> m_dest;

  public:
    /**
     * Constructs a new Section with the given ID, directionality, and,
     * optionally, length, destination data.
     */
    Section(Identifier, bool isBidir, Length length = 0,
            std::unique_ptr<Destination> dest = nullptr);

    /**
     * Returns the Identifier of this Section.
     *
     * @return the ID of this section
     */
    [[nodiscard]] const Identifier &id() const { return m_id; }

    /**
     * Returns the ID of the Node at the start of this Section.
     *
     * Equivalent to `node(0)`.
     *
     * @return the ID of the start Node or `ID_NULL`
     */
    [[nodiscard]] const Identifier &start() const { return m_start; }

    /**
     * Returns the ID of the Node at the end of this Section.
     *
     * Equivalent to `node(1)`.
     *
     * @return the ID of the end Node or `ID_NULL`
     */
    [[nodiscard]] const Identifier &end() const { return m_end; }

    /**
     * Returns `true` iff the Section is bidirectional, i.e. allows travel from
     * end to start.
     *
     * @return whether this Section is bidirectional
     */
    [[nodiscard]] bool isBidir() const { return m_bidirectional; }

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
    [[nodiscard]] const Identifier &node(size_t index) const {
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
     * Checks whether travel is allowed from Node in slot with index `from` to
     * Node in slot with index `to`.
     *
     * See @ref Section::node for layout.
     *
     * Travel is always allowed from 0 to 1. If this Section is bidirectional,
     * travel is additionally allowed from 1 to 0. All other inputs return
     * `false`.
     *
     * @param from index of starting Node
     * @param to index of ending Node
     *
     * @return `true` if and only if travel is possible and allowed in given
     * direction
     */
    [[nodiscard]] bool canTraverse(size_t from, size_t to) const {
        return (from == 0 && to == 1) || (isBidir() && from == 1 && to == 0);
    }

    /**
     * Returns the length of this section.
     *
     * Length can be zero exactly, meaning it is negligeable. Otherwise it
     * should be significantly greater than zero.
     *
     * @return the lenght of this section
     */
    [[nodiscard]] Length length() const { return m_length; }

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
class Destination {

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
