#include <piwcsprwmodel/io.h>

#include "nodetypeinfo.h"
#include <fstream>
#include <iostream>
#include <minijson_reader/minijson_reader.hpp>
#include <optional>
#include <sstream>

namespace piwcs::prw {

namespace {

using namespace minijson::handlers;
using minijson::value;

template <typename S, typename V> auto into(V S::*dest) {
    return [dest](S &s, value v) { v.to(s.*dest); };
}

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
             CROSSING,
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

    if (!model.newNode(data.type, std::move(nodeId))) {
        throw IllegalModelError("duplicate node ID");
    }
}

struct Link {
    std::optional<std::string_view> startNode{};
    std::optional<SlotId> startSlot{};
    std::optional<std::string_view> endNode{};
    std::optional<SlotId> endSlot{};
};

struct DestData {
    std::string_view address{};
    std::string_view name{};
};

struct SectionData {
    Link link{};
    bool bidir = false;
    Section::Length length = 0;
    std::unique_ptr<Destination> dest{};
};

const minijson::dispatcher linkDispatcher{
    handler("startNode", into(&Link::startNode)),
    handler("startSlot", into(&Link::startSlot)),
    handler("endNode", into(&Link::endNode)),
    handler("endSlot", into(&Link::endSlot)),
};

void parseLink(SectionData &s, value, minijson::istream_context &ctx) {
    linkDispatcher.run(ctx, s.link);
}

const minijson::dispatcher destDispatcher{
    handler("address", into(&DestData::address)),
    handler("name", into(&DestData::name)),
};

void parseDest(SectionData &s, value, minijson::istream_context &ctx) {
    DestData data;
    destDispatcher.run(ctx, data);
    s.dest = std::make_unique<Destination>(Destination::Address(data.address),
                                           Destination::Name(data.name));
}

const minijson::dispatcher sectionDispatcher{
    optional_handler("link", parseLink),

    optional_handler("bidir", into(&SectionData::bidir)),
    optional_handler("length", into(&SectionData::length)),

    optional_handler("dest", parseDest),
};

void parseSection(minijson::istream_context &ctx, Model &model,
                  const Identifier &sectionId) {
    SectionData data;

    sectionDispatcher.run(ctx, data);

    if (!model.newSection(sectionId, data.bidir, data.length,
                          std::move(data.dest))) {
        throw IllegalModelError("duplicate section ID or destination address");
    }

    if (data.link.startNode) {
        const auto &l = data.link;
        if (!model.link(sectionId, *l.startNode, *l.startSlot, *l.endNode,
                        *l.endSlot)) {
            throw IllegalModelError("linkage inconsistency found");
        }
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

    if (part == 0) {
        throw InvalidFormatError("main array is empty");
    } else if (part == 1) {
        throw InvalidFormatError("section data not found");
    }
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
    std::ifstream in(filename);
    in.exceptions(std::ios_base::failbit); // throws if the file failed to open
    return readModel(in);
}

} // namespace piwcs::prw
