#include <piwcsprwmodel/io.h>

#include "nodetypeinfo.h"
#include <fstream>
#include <iostream>
#include <minijson_writer/minijson_writer.hpp>

namespace piwcs::prw {

namespace {

void writeNode(minijson::object_writer w, const Node &node) {
    w.write("type", node.type()->name);
    w.close();
}

void writeNodes(minijson::object_writer w, const Model &model) {
    for (auto &&[nodeId, node] : model.nodes()) {
        writeNode(w.nested_object(nodeId.c_str()), node);
    }
    w.close();
}

void writeSections(minijson::object_writer w, const Model &model) {
    // TODO implement (waiting for OwnedSection for section.endSlot())
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
