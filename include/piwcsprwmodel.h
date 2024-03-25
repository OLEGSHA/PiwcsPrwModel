#ifndef PIWCS_PRW_MODEL
#define PIWCS_PRW_MODEL

/**
 * @file
 *
 * This header declares the Model describing the PIWCS Public Railway and
 * associated I/O routines.
 *
 * ## Background
 *
 * PIWCS is a private Minecraft server. PIWCS Public Railway (PRW) is a
 * community project implemented by the players of PIWCS to build an extensive
 * in-game rail network for automated, public use.
 *
 * PRW is designed to be used for assisted player travel and fully automated
 * freight traffic. Players can use private or PRW-provided trains with routing
 * capabilities to travel to a _destination_ by entering its address. The same
 * applies to automated trains. Destinations are stations, automated train
 * depots and special servicing facilities; the list of destinations is defined
 * explicitly. PRW is set up to allow navigating to any destination from any
 * location, known as _routing_.
 *
 * To help maintain and develop the network, a digital description of the
 * railway is needed. This library introduces a single data structure and
 * storage format for this model.
 *
 * ## Model overview
 *
 * ### Nodes and sections
 *
 * PIWCS PRW Model is primarily concerned with the following data:
 *   - structure of PRW railways and supporting railways
 *   - routing information
 *   - destinations
 *   - various metadata on the above entities such as maintainer, track type,
 *     etc.
 *
 * The model represents the railway network using a collection of _nodes_
 * connected by _sections_. Sections represent lengths of track that trains
 * can traverse. Nodes represent abstract locations on tracks. Nodes and
 * sections are assigned unique short non-descriptive text IDs.
 *
 * (TODO: describe ID conventions.)
 *
 * Nodes and sections are connected with _slots_. All sections have only a
 * "start" and an "end" slot, while slots of nodes vary depending on their
 * _node type_. Slots are numbered `0...N`. At most one connection is allowed
 * for each slot, and a section cannot connect to the one node with both its
 * slots. For example, "diverging" slot (slot `2`) of node `M145` may be
 * connected to the "start" slot (slot `0`) of section `S400` (see Illustrated
 * Example below).
 *
 * ### Completeness
 *
 * Nodes may have any subset of their slots connected, while sections are either
 * connected on both sides or not connected at all. Models that have at least
 * one empty slot are called _incomplete_; an accurate description of the PRW
 * must be _complete_.
 *
 * ### Allowed Travel and Correctness
 *
 * Not all movement along sections is possible or allowed.
 *
 * Sections have _directionality_, also referred to as _allowed travel_. This
 * property refers to the ways routed traffic may traverse this length track;
 * trains controlled manually or in exceptional conditions may ignore these
 * restrictions. Sections can be unidirectional (start to end only),
 * bidirectional (choice of start and end is arbitrary) or forbid all routed
 * traffic.
 *
 * Some nodes, mostly switches, impose restrictions on the directionality of
 * connected sections and may limit transitions between slots. For example,
 * all switches forbid transitions between "straight" (`1`) and "diverging"
 * (`2`) slots; trains must move through the "common" (`0`) slot. These rules
 * are determined by node type. See nodes.h for further details.
 *
 * Additionally, as should be obvious, trains cannot switch their travel
 * direction in the middle of a bidirection section.
 *
 * Models where all paths allowed by these rules have no dead ends or
 * unreachable starting positions (except for bidirectional `END` nodes) are
 * called _correct_. Incorrect models, including models where the rules layed
 * out above are violated, can be represented by the model.
 *
 * ### Destinations
 *
 * Some (a few) sections are destinations. Each destination is identified by
 * a unique address and a human-friendly name. For the purposes of this
 * documentation destinations are considered a third kind of entity in the model
 * in addition to nodes and sections; each destination is assigned to one
 * section.
 *
 * ### Metadata
 *
 * All model entities (nodes, sections, destinations) may have additional data
 * attached. Metadata are arbitrary strings keyed by arbitrary strings.
 * The meaning of metadata entries is generally outside of the scope of this
 * library.
 *
 * The convention for metadata key names is `snake_case`. Should namespaces be
 * required, they should be separated with dots: `my_namespace.subgroup.my_key`.
 *
 * This implementation preserves and exposes all metadata entries and does not
 * interpret any of them.
 *
 * ## Illustrated Example
 *
 * @image html "illustration-1.svg" "Small model example"
 *
 * In this example, a tiny model consists of two nodes, `MOTORIZED` `M145` and
 * `THRU` `T1034` and one section `S400` that connects them. Slots and node
 * traversal rules are depicted in light gray. Destinations and metadata are not
 * present.
 *
 * Two connections are visible: slot `2` of `M145` connects to starting slot of
 * `S400`, and slot `0` of `T1034` connects to ending slot of `S400`.
 *
 * This model is incomplete and incorrect, as the only allowed path has both a
 * dead end at `T1034` and a unreachable starting position at `M145`.
 *
 * @image html "illustration-2.svg" "Larger model example"
 *
 * In this example, a slightly larger model describes two conjoined loops of
 * unidirectional track with a station and some metadata.
 *
 * Section `S256` is assigned to a destination called "Apple Yard" with address
 * `1.0.1`. Note that no specific location for the stop is specified. Such data,
 * should it be necessary, could be stored in the metadata of the destination.
 *
 * Section `S308` contains metadata. Although it is not interpreted by this
 * library, it might mean that the section is in a tunnel, and that GUI editors
 * should display it in red. Note that the value of `tunnel` is not enforced to
 * be a string representing a boolean value.
 *
 * Node `T998` exists to separate the part of the left track that lies inside a
 * tunnel from the part that does not.
 *
 * Locations and orientations of the entities on this diagram are chosen
 * arbitrarily as the model does not interpret locational data.
 *
 * This model is complete and correct.
 *
 * To maintain correctness, `S309` cannot be bidirectional and it cannot be
 * reversed, as it would violate the rules of `M106` for slot `2`. No
 * directionality of `S309` could violate the rules of `T998`. `S309` must allow
 * travel away from `M106` to provide a path for routed trains choosing to
 * divert at `M106`, and it must allow travel to `T998` to make the start of
 * `S308` reachable.
 */

#include "piwcsprwmodel/fwd.h"

#include "piwcsprwmodel/util.h"

#include "piwcsprwmodel/nodes.h"

#include "piwcsprwmodel/section.h"

#include "piwcsprwmodel/model.h"

#include "piwcsprwmodel/io.h"

#include "piwcsprwmodel/algorithms.h"

#endif // PIWCS_PRW_MODEL
