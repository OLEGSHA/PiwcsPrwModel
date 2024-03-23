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

struct MetadataData {
    std::optional<Metadata> metadata;
};

void parseMetadata(MetadataData &md, value, minijson::istream_context &ctx) {
    minijson::parse_object(ctx, [&](std::string_view key, value v) {
        if (!md.metadata) {
            md.metadata.emplace();
        }
        (*md.metadata)[Identifier(key)] = v.as<std::string_view>();
    });
}

void installMetadata(detail::HasMetadata &target, MetadataData &source) {
    if (source.metadata) {
        target.metadata() = std::move(*source.metadata);
    }
}

struct NodeData : public MetadataData {
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
             MANUAL,
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
    optional_handler("metadata", parseMetadata),
};

void parseNode(minijson::istream_context &ctx, Model &model,
               Identifier nodeId) {
    NodeData data;

    nodeDispatcher.run(ctx, data);

    Node node(data.type, std::move(nodeId));
    installMetadata(node, data);

    if (!model.addNode(std::move(node))) {
        throw IllegalModelError("duplicate node ID");
    }
}

struct Link {
    std::optional<std::string_view> startNode{};
    std::optional<SlotId> startSlot{};
    std::optional<std::string_view> endNode{};
    std::optional<SlotId> endSlot{};
};

struct DestData : public MetadataData {
    std::string_view address{};
    std::string_view name{};
};

struct SectionData : public MetadataData {
    Link link{};
    Section::AllowedTravel dir = Section::AllowedTravel::UNIDIR;
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
    optional_handler("metadata", parseMetadata),
};

void parseDest(SectionData &s, value, minijson::istream_context &ctx) {
    DestData data;
    destDispatcher.run(ctx, data);
    auto d = std::make_unique<Destination>(Destination::Address(data.address),
                                           Destination::Name(data.name));
    installMetadata(*d, data);
    s.dest = std::move(d);
}

const minijson::dispatcher sectionDispatcher{
    optional_handler("link", parseLink),
    optional_handler("dir", into(&SectionData::dir)),
    optional_handler("length", into(&SectionData::length)),
    optional_handler("dest", parseDest),
    optional_handler("metadata", parseMetadata),
};

void parseSection(minijson::istream_context &ctx, Model &model,
                  const Identifier &sectionId) {
    SectionData data;

    sectionDispatcher.run(ctx, data);

    Section section(sectionId, data.dir, data.length, std::move(data.dest));
    installMetadata(section, data);

    if (!model.addSection(std::move(section))) {
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

namespace minijson {

using namespace piwcs::prw;

template <> struct value_as<Section::AllowedTravel> {
    Section::AllowedTravel operator()(const value v) const {
        if (v.type() != String) {
            throw InvalidFormatError(
                "could not convert JSON value to Section::AllowedTravel");
        }

        if (v.raw() == "NONE") {
            return Section::AllowedTravel::NONE;
        }
        if (v.raw() == "UNIDIR") {
            return Section::AllowedTravel::UNIDIR;
        }
        if (v.raw() == "BIDIR") {
            return Section::AllowedTravel::BIDIR;
        }
        throw InvalidFormatError("unknown directionality");
    }
};

} // namespace minijson
