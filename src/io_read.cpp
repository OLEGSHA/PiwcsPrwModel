#include <piwcsprwmodel/io.h>

#include "nodetypeinfo.h"
#include <fstream>
#include <iostream>
#include <minijson_reader/minijson_reader.hpp>
#include <sstream>

namespace piwcs::prw {

namespace {

using namespace minijson::handlers;
using minijson::value;

struct NodeData {
    NodeType type{};
};

void parseNodeType(NodeData &nd, value v) {
    std::string_view declar;
    v.to(declar);

    for (const auto *type : {
             THRU,
             MOTORIZED,
             PASSIVE,
             FIXED,
             // CROSSING,
             END,
         }) {
        if (type->name == declar) {
            nd.type = type;
            return;
        }
    }

    throw InvalidFormatError("unknown node \"type\"");
}

const minijson::dispatcher nodeDispatcher{
    handler("type", parseNodeType),
};

void parseNode(minijson::istream_context &ctx, Model &model,
               Identifier nodeId) {
    NodeData data;

    nodeDispatcher.run(ctx, data);

    if (!model.addNode(Node(data.type, std::move(nodeId)))) {
        throw IllegalModelError("duplicate node ID");
    }
}

struct SectionData {
    std::string_view startNode{};
    SlotId startSlot{};
    std::string_view endNode{};
    SlotId endSlot{};
    bool bidir = false;
    Section::Length length = 0;
};

const minijson::dispatcher sectionDispatcher{
    handler("startNode", [](SectionData &sd, value v) { v.to(sd.startNode); }),
    handler("startSlot", [](SectionData &sd, value v) { v.to(sd.startSlot); }),
    handler("endNode", [](SectionData &sd, value v) { v.to(sd.endNode); }),
    handler("endSlot", [](SectionData &sd, value v) { v.to(sd.endSlot); }),

    optional_handler("bidir", [](SectionData &sd, value v) { v.to(sd.bidir); }),
    optional_handler("length",
                     [](SectionData &sd, value v) { v.to(sd.length); }),

    // TODO read destination data
};

void parseSection(minijson::istream_context &ctx, Model &model,
                  const Identifier &sectionId) {
    SectionData data;

    sectionDispatcher.run(ctx, data);

    if (!model.addSection(
            Section(sectionId, data.bidir, data.length, nullptr))) {
        throw IllegalModelError("duplicate section ID or destination address");
    }
    if (!model.link(sectionId, Identifier(data.startNode), data.startSlot,
                    Identifier(data.endNode), data.endSlot)) {
        throw IllegalModelError("linkage inconsistency found");
    }
}

void parseMain(minijson::istream_context &ctx, Model &model) {
    int part = 0;

    minijson::parse_array(ctx, [&](value) {
        switch (part) {
        case 0:
            minijson::parse_object(ctx, [&](std::string_view name, value) {
                parseNode(ctx, model, Identifier(name));
            });
            break;
        case 1:
            minijson::parse_object(ctx, [&](std::string_view name, value) {
                parseSection(ctx, model, Identifier(name));
            });
            break;
        default:
            throw InvalidFormatError("unused data in main array found");
        }
        part++;
    });
}

template <typename S> [[noreturn]] void wrap_exception(const char *lhs, S rhs) {
    std::ostringstream msg;
    msg << lhs << ": " << rhs;
    throw InvalidFormatError(msg.str());
}

} // namespace

Model readModel(std::istream &in) {
    Model model;
    minijson::istream_context ctx(in);

    try {
        parseMain(ctx, model);
    } catch (const minijson::parse_error &e) {
        wrap_exception("JSON parse error", e.what());
    } catch (const minijson::bad_value_cast &e) {
        throw InvalidFormatError("type mismatch");
    } catch (const minijson::unhandled_field_error &e) {
        wrap_exception("unused field found", e.field_name_truncated());
    } catch (const minijson::missing_field_error &e) {
        wrap_exception("field not found", e.field_name_truncated());
    }

    return std::move(model);
}

Model readModel(const std::string &filename) {
    std::ifstream in(filename, std::ios_base::binary);
    in.exceptions(std::ios_base::failbit); // throws if the file failed to open
    return readModel(in);
}

} // namespace piwcs::prw
