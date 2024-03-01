#include <piwcsprwmodel/io.h>

#include "debug.h"
#include "nodetypeinfo.h"
#include "string_view_writer.h"
#include <fstream>
#include <iostream>
#include <minijson_writer/minijson_writer.hpp>

namespace minijson {

template <> struct default_value_writer<std::string_view> {
    void operator()(std::ostream &stream, std::string_view p,
                    minijson::writer_configuration) const {
        write_quoted_string(stream, p);
    }
};

} // namespace minijson

namespace piwcs::prw {

namespace {

void writeNode(minijson::object_writer w, const Node &node) {
    w.write("type", node.type()->name);
    w.close();
}

void writeNodes(minijson::object_writer w, const Model &model) {
    for (const auto &[nodeId, node] : model.nodes()) {
        writeNode(w.nested_object(nodeId.c_str()), node);
    }
    w.close();
}

void writeDestination(minijson::object_writer w, const Destination &dest) {
    w.write("address", dest.address());
    w.write("name", dest.name());
    w.close();
}

void writeSection(minijson::object_writer w, const Section &section,
                  const Model &model) {
    if (section.start() != ID_NULL) {
        const Node *start = model.node(section.start());
        const Node *end = model.node(section.end());
        _ASSERT(start != nullptr, "start not found");
        _ASSERT(end != nullptr, "end not found");

        SlotId startSlot = start->slotOf(section.id());
        SlotId endSlot = end->slotOf(section.id());
        _ASSERT(startSlot != SLOT_INVALID, "start slot not found");
        _ASSERT(endSlot != SLOT_INVALID, "end slot not found");

        w.write("startNode", section.start());
        w.write("startSlot", startSlot);
        w.write("endNode", section.end());
        w.write("endSlot", endSlot);
    }

    if (section.isBidir()) {
        w.write("bidir", true);
    }

    if (section.length() > 0) {
        w.write("length", section.length());
    }

    if (section.isDestination()) {
        writeDestination(w.nested_object("dest"), *section.destination());
    }

    w.close();
}

void writeSections(minijson::object_writer w, const Model &model) {
    for (const auto &[sectionId, section] : model.sections()) {
        writeSection(w.nested_object(sectionId.c_str()), section, model);
    }
    w.close();
}

} // namespace

void writeModel(std::ostream &out, const Model &model) {
    minijson::array_writer w(
        out, minijson::writer_configuration().pretty_printing(true));

    writeNodes(w.nested_object(), model);
    writeSections(w.nested_object(), model);

    w.close();
}

void writeModel(const std::string &filename, const Model &model) {
    std::ofstream out(filename);
    out.exceptions(std::ios_base::failbit); // throws if the file failed to open
    writeModel(out, model);
}

} // namespace piwcs::prw
