#include <gtest/gtest.h>

#include <piwcsprwmodel.h>

#include <sstream>

using namespace piwcs::prw;

namespace {

void modelsMustBeEqual(const Model &a, const Model &b) {
    auto metaMustBeEqual = [](const detail::HasMetadata &am,
                              const detail::HasMetadata &bm) {
        EXPECT_EQ(am.hasMetadata(), bm.hasMetadata());
        if (am.hasMetadata()) {
            EXPECT_EQ(am.metadata(), bm.metadata());
        }
    };

    EXPECT_EQ(a.nodes().size(), b.nodes().size());
    EXPECT_EQ(a.sections().size(), b.sections().size());

    for (const auto &[id, an] : a.nodes()) {
        const auto *bn = b.node(id);
        EXPECT_NE(bn, nullptr);
        EXPECT_EQ(an.type(), bn->type());
        for (SlotId i = 0; i < an.sectionCount(); i++) {
            EXPECT_EQ(an.section(i), bn->section(i));
        }
        metaMustBeEqual(an, *bn);
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
            const auto *bd = bs->destination();

            EXPECT_EQ(ad->address(), bd->address());
            EXPECT_EQ(ad->name(), bd->name());

            metaMustBeEqual(*ad, *bd);
        }

        metaMustBeEqual(as, *bs);
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

    model.node("n5")->metadata("n5-key1") = "apple";
    model.node("n5")->metadata("n5-key2") = "orange";

    model.newSection("s1", true, 42,
                     std::make_unique<Destination>("1.0.1", "My Name"));

    model.section("s1")->metadata("s1-key1") = "grape";
    model.section("s1")->metadata("s1-key2") = "banana";

    model.section("s1")->destination()->metadata("d-key1") = "tomato";
    model.section("s1")->destination()->metadata("d-key2") = "papaya";

    model.link("s1", "n1", 0, "n2", 1);

    writeReadCheck(model);
}
