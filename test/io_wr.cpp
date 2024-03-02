#include <gtest/gtest.h>

#include <piwcsprwmodel.h>

#include <sstream>

using namespace piwcs::prw;

namespace {

void modelsMustBeEqual(const Model &a, const Model &b) {
    EXPECT_EQ(a.nodes().size(), b.nodes().size());
    EXPECT_EQ(a.sections().size(), b.sections().size());

    for (const auto &[id, an] : a.nodes()) {
        const auto *bn = b.node(id);
        EXPECT_NE(bn, nullptr);
        EXPECT_EQ(an.type(), bn->type());
        for (SlotId i = 0; i < an.sectionCount(); i++) {
            EXPECT_EQ(an.section(i), bn->section(i));
        }
    }

    for (const auto &[id, as] : a.sections()) {
        const auto *bs = b.section(id);
        EXPECT_NE(bs, nullptr);

        EXPECT_EQ(as.start(), bs->start());
        EXPECT_EQ(as.end(), bs->end());

        EXPECT_EQ(as.isBidir(), bs->isBidir());
        EXPECT_EQ(as.length(), bs->length());

        EXPECT_EQ(as.isDestination(), bs->isDestination());
        if (as.isDestination()) {
            const auto *ad = as.destination();
            const auto *bd = as.destination();

            EXPECT_EQ(ad->address(), bd->address());
            EXPECT_EQ(ad->name(), bd->name());
        }
    }
}

void writeReadCheck(const Model &model) {
    std::stringstream buffer;
    writeModel(buffer, model);
    modelsMustBeEqual(model, readModel(buffer));
}

} // namespace

TEST(IoWriteRead, Basic) {
    Model model;
    model.newNode(THRU, "n1");
    model.newNode(THRU, "n2");
    model.newSection("s1", false);
    writeReadCheck(model);
}

TEST(IoWriteRead, Empty) {
    Model model;
    writeReadCheck(model);
}

TEST(IoWriteRead, BasicWithLink) {
    Model model;
    model.newNode(THRU, "n1");
    model.newNode(THRU, "n2");
    model.newSection("s1", false);
    model.link("s1", "n1", 0, "n2", 1);
    writeReadCheck(model);
}

TEST(IoWriteRead, Maximal) {
    Model model;

    model.newNode(THRU, "n1");
    model.newNode(MOTORIZED, "n2");
    model.newNode(PASSIVE, "n3");
    model.newNode(FIXED, "n4");
    model.newNode(CROSSING, "n5");
    model.newNode(END, "n6");

    model.newSection("s1", true, 42,
                     std::make_unique<Destination>("1.0.1", "My Name"));

    model.link("s1", "n1", 0, "n2", 1);

    writeReadCheck(model);
}
