#include <piwcsprwmodel/io.h>

#include "debug.h"
#include "nodetypeinfo.h"
#include <fstream>
#include <iostream>
#include <minijson_writer/minijson_writer.hpp>

namespace piwcs::prw {

namespace {

void writeMetadata(minijson::object_writer &pw,
                   const detail::HasMetadata &obj) {
    if (!obj.hasMetadata()) {
        return;
    }

    auto w = pw.nested_object("metadata");
    for (const auto &[key, value] : obj.metadata()) {
        w.write(key.c_str(), value);
    }
}

void writeNode(minijson::object_writer &pw, const Node &node, const char *id) {
    auto w = pw.nested_object(id); // TODO s/id/section.id()/
    w.write("type", node.type()->name);
    writeMetadata(w, node);
}

void writeNodes(minijson::array_writer &pw, const Model &model) {
    auto w = pw.nested_object();
    for (const auto &[nodeId, node] : model.nodes()) {
        writeNode(w, node, nodeId.c_str());
    }
}

void writeLink(minijson::object_writer &pw, const Section &section,
               const Model &model) {
    if (!section.isConnected()) {
        return;
    }

    auto w = pw.nested_object("link");

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

void writeDestination(minijson::object_writer &pw, const Section &section) {
    if (!section.isDestination()) {
        return;
    }

    const auto &dest = *section.destination();

    auto w = pw.nested_object("dest");
    w.write("address", dest.address());
    w.write("name", dest.name());
    writeMetadata(w, dest);
}

void writeSection(minijson::object_writer &pw, const Section &section,
                  const char *id, const Model &model) {
    auto w = pw.nested_object(id); // TODO s/id/section.id()/

    writeLink(w, section, model);

    const char *dirName{};
    switch (section.dir()) {
    case Section::AllowedTravel::NONE:
        dirName = "NONE";
        break;
    case Section::AllowedTravel::UNIDIR:
        dirName = "UNIDIR";
        break;
    case Section::AllowedTravel::BIDIR:
        dirName = "BIDIR";
        break;
    }
    w.write("dir", dirName);

    writeDestination(w, section);
    writeMetadata(w, section);
}

void writeSections(minijson::array_writer &pw, const Model &model) {
    auto w = pw.nested_object();
    for (const auto &[sectionId, section] : model.sections()) {
        writeSection(w, section, sectionId.c_str(), model);
    }
}

} // namespace

void writeModel(std::ostream &out, const Model &model) {
    minijson::array_writer w(
        out, minijson::writer_configuration().pretty_printing(true));

    writeNodes(w, model);
    writeSections(w, model);
}

void writeModel(const std::string &filename, const Model &model) {
    std::ofstream out(filename);
    out.exceptions(std::ios_base::failbit); // throws if the file failed to open
    writeModel(out, model);
}

} // namespace piwcs::prw
