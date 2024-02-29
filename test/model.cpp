#include <gtest/gtest.h>

#include <piwcsprwmodel.h>

using namespace piwcs::prw;

TEST(Identifiers, Validators) {
    EXPECT_TRUE(isId("123"));
    EXPECT_TRUE(isId("1"));
    EXPECT_TRUE(isId(Identifier(IDENT_LENGTH, '1')));

    EXPECT_FALSE(isId(ID_NULL));
    EXPECT_FALSE(isId(""));
    EXPECT_TRUE(isIdOrNull(ID_NULL));

    EXPECT_FALSE(isId(ID_INVALID));
}

TEST(Model, Constructor) {
    Model model;

    EXPECT_TRUE(model.nodes().empty());
    EXPECT_TRUE(model.sections().empty());
}

TEST(Model, AddNode) {
    Model model;

    auto res = model.newNode(THRU, "123");
    EXPECT_EQ(res, Model::AddResult::OK);
    EXPECT_EQ(model.nodes().size(), 1);

    res = model.newNode(THRU, ID_INVALID);
    EXPECT_EQ(res, Model::AddResult::BAD_ID);
    EXPECT_EQ(model.nodes().size(), 1);

    res = model.newNode(THRU, "123");
    EXPECT_EQ(res, Model::AddResult::DUPLICATE);
    EXPECT_EQ(model.nodes().size(), 1);

    res = model.newNode(THRU, "456");
    EXPECT_EQ(res, Model::AddResult::OK);
    EXPECT_EQ(model.nodes().size(), 2);
}

TEST(Model, FindNode) {
    Model model;

    model.newNode(THRU, "123");
    model.newNode(THRU, "456");

    const Node *node = model.node("123");
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->id(), "123");

    node = model.node("000");
    EXPECT_EQ(node, nullptr);
}

TEST(Model, RemoveNode) {
    Model model;

    model.newNode(THRU, "123");

    auto res = model.removeNode("000");
    EXPECT_EQ(res, Model::RemoveResult::NOT_FOUND);
    EXPECT_FALSE(model.nodes().empty());

    res = model.removeNode("123");
    EXPECT_EQ(res, Model::RemoveResult::OK);
    EXPECT_TRUE(model.nodes().empty());
}

TEST(Model, AddSection) {
    Model model;

    auto res = model.newSection("123", false);
    EXPECT_EQ(res, Model::AddResult::OK);
    EXPECT_EQ(model.sections().size(), 1);

    res = model.newSection(ID_INVALID, false);
    EXPECT_EQ(res, Model::AddResult::BAD_ID);
    EXPECT_EQ(model.sections().size(), 1);

    res = model.newSection("123", false);
    EXPECT_EQ(res, Model::AddResult::DUPLICATE);
    EXPECT_EQ(model.sections().size(), 1);

    res = model.newSection("456", false);
    EXPECT_EQ(res, Model::AddResult::OK);
    EXPECT_EQ(model.sections().size(), 2);
}

TEST(Model, AddSectionWithDestination) {
    Model model;

    auto res = model.newSection(
        "123", false, 0, std::make_unique<Destination>("1.0.0", "Name1"));
    EXPECT_EQ(res, Model::AddResult::OK);
    EXPECT_EQ(model.sections().size(), 1);

    res = model.newSection("456", false, 0,
                           std::make_unique<Destination>("1.0.1", "Name1"));
    EXPECT_EQ(res, Model::AddResult::OK);
    EXPECT_EQ(model.sections().size(), 2);

    res = model.newSection("789", false, 0,
                           std::make_unique<Destination>("1.0.1", "Name1"));
    EXPECT_EQ(res, Model::AddResult::DUPLICATE);
    EXPECT_EQ(model.sections().size(), 2);
}

TEST(Model, FindSection) {
    Model model;

    model.newSection("123", false);
    model.newSection("456", false);

    const Section *section = model.section("123");
    EXPECT_NE(section, nullptr);
    EXPECT_EQ(section->id(), "123");

    section = model.section("000");
    EXPECT_EQ(section, nullptr);
}

TEST(Model, RemoveSection) {
    Model model;

    model.newSection("123", false);

    auto res = model.removeSection("000");
    EXPECT_EQ(res, Model::RemoveResult::NOT_FOUND);
    EXPECT_FALSE(model.sections().empty());

    res = model.removeSection("123");
    EXPECT_EQ(res, Model::RemoveResult::OK);
    EXPECT_TRUE(model.sections().empty());
}

TEST(Model, Link) {
    Model model;
    EXPECT_TRUE(!!model.newSection("s1", false));
    EXPECT_TRUE(!!model.newSection("s2", false));
    EXPECT_TRUE(!!model.newNode(THRU, "n1"));
    EXPECT_TRUE(!!model.newNode(THRU, "n2"));

    auto checkStatusQuo = [&]() {
        auto s1 = model.section("s1");
        auto s2 = model.section("s2");
        auto n1 = model.node("n1");
        auto n2 = model.node("n2");
        EXPECT_EQ(n1->section(0), s1->id());
        EXPECT_EQ(n1->section(1), ID_NULL);
        EXPECT_EQ(n2->section(0), ID_NULL);
        EXPECT_EQ(n2->section(1), s1->id());
        EXPECT_EQ(s1->start(), n1->id());
        EXPECT_EQ(s1->end(), n2->id());
        EXPECT_EQ(s2->start(), ID_NULL);
        EXPECT_EQ(s2->end(), ID_NULL);
    };

    // OK
    auto res = model.link("s1", "n1", 0, "n2", 1);
    EXPECT_EQ(res, Model::LinkResult::OK);
    checkStatusQuo();

    // s9 does not exist
    res = model.link("s9", "n1", 1, "n2", 0);
    EXPECT_EQ(res, Model::LinkResult::NOT_FOUND);
    checkStatusQuo();

    // n9 does not exist
    res = model.link("s2", "n9", 1, "n2", 0);
    EXPECT_EQ(res, Model::LinkResult::NOT_FOUND);
    checkStatusQuo();

    // n1.9 does not exist
    res = model.link("s2", "n1", 9, "n2", 0);
    EXPECT_EQ(res, Model::LinkResult::NOT_FOUND);
    checkStatusQuo();

    // n9 does not exist
    res = model.link("s2", "n1", 1, "n9", 0);
    EXPECT_EQ(res, Model::LinkResult::NOT_FOUND);
    checkStatusQuo();

    // n2.9 does not exist
    res = model.link("s2", "n1", 1, "n2", 9);
    EXPECT_EQ(res, Model::LinkResult::NOT_FOUND);
    checkStatusQuo();

    // start occupied
    res = model.link("s2", "n1", 0, "n2", 0);
    EXPECT_EQ(res, Model::LinkResult::NODE_OCCUPIED);
    checkStatusQuo();

    // end occupied
    res = model.link("s2", "n1", 1, "n2", 1);
    EXPECT_EQ(res, Model::LinkResult::NODE_OCCUPIED);
    checkStatusQuo();

    // section occupied
    res = model.link("s1", "n1", 1, "n2", 0);
    EXPECT_EQ(res, Model::LinkResult::SECTION_OCCUPIED);
    checkStatusQuo();

    // start == end
    res = model.link("s2", "n1", 0, "n1", 1);
    EXPECT_EQ(res, Model::LinkResult::SAME_NODE);
    checkStatusQuo();
}

TEST(Model, RemoveLinked) {
    Model model;
    EXPECT_TRUE(!!model.newSection("s1", false));
    EXPECT_TRUE(!!model.newNode(THRU, "n1"));
    EXPECT_TRUE(!!model.newNode(THRU, "n2"));
    EXPECT_TRUE(!!model.link("s1", "n1", 0, "n2", 1));
    auto s1 = model.section("s1");
    auto n1 = model.node("n1");
    auto n2 = model.node("n2");

    auto checkStatusQuo = [&]() {
        EXPECT_NE(model.section("s1"), nullptr);
        EXPECT_NE(model.node("n1"), nullptr);
        EXPECT_NE(model.node("n2"), nullptr);
        EXPECT_EQ(n1->section(0), s1->id());
        EXPECT_EQ(n1->section(1), ID_NULL);
        EXPECT_EQ(n2->section(0), ID_NULL);
        EXPECT_EQ(n2->section(1), s1->id());
        EXPECT_EQ(s1->start(), n1->id());
        EXPECT_EQ(s1->end(), n2->id());
    };
    checkStatusQuo();

    auto res = model.removeNode("n1");
    EXPECT_EQ(res, Model::RemoveResult::REFERENCED);
    checkStatusQuo();

    res = model.removeNode("n2");
    EXPECT_EQ(res, Model::RemoveResult::REFERENCED);
    checkStatusQuo();

    res = model.removeSection("s1");
    EXPECT_EQ(res, Model::RemoveResult::REFERENCED);
    checkStatusQuo();
}

TEST(Model, UnlinkFailure) {
    Model model;
    EXPECT_TRUE(!!model.newSection("s1", false));
    EXPECT_TRUE(!!model.newSection("s2", false));
    EXPECT_TRUE(!!model.newNode(THRU, "n1"));
    EXPECT_TRUE(!!model.newNode(THRU, "n2"));
    EXPECT_TRUE(!!model.link("s1", "n1", 0, "n2", 1));
    auto s1 = model.section("s1");
    auto s2 = model.section("s2");
    auto n1 = model.node("n1");
    auto n2 = model.node("n2");

    auto checkStatusQuo = [&]() {
        EXPECT_NE(model.section("s1"), nullptr);
        EXPECT_NE(model.section("s2"), nullptr);
        EXPECT_NE(model.node("n1"), nullptr);
        EXPECT_NE(model.node("n2"), nullptr);
        EXPECT_EQ(n1->section(0), s1->id());
        EXPECT_EQ(n1->section(1), ID_NULL);
        EXPECT_EQ(n2->section(0), ID_NULL);
        EXPECT_EQ(n2->section(1), s1->id());
        EXPECT_EQ(s1->start(), n1->id());
        EXPECT_EQ(s1->end(), n2->id());
        EXPECT_EQ(s2->start(), ID_NULL);
        EXPECT_EQ(s2->end(), ID_NULL);
    };
    checkStatusQuo();

    auto res = model.unlink("s3");
    EXPECT_EQ(res, Model::UnlinkResult::NOT_FOUND);
    checkStatusQuo();

    res = model.unlink("s2");
    EXPECT_EQ(res, Model::UnlinkResult::NOT_LINKED);
    checkStatusQuo();
}

TEST(Model, Unlink) {
    Model model;
    EXPECT_TRUE(!!model.newSection("s1", false));
    EXPECT_TRUE(!!model.newSection("s2", false));
    EXPECT_TRUE(!!model.newNode(THRU, "n1"));
    EXPECT_TRUE(!!model.newNode(THRU, "n2"));
    EXPECT_TRUE(!!model.link("s1", "n1", 0, "n2", 1));
    EXPECT_TRUE(!!model.link("s2", "n1", 1, "n2", 0));

    auto res = model.unlink("s1");

    EXPECT_EQ(res, Model::UnlinkResult::OK);
    auto s1 = model.section("s1");
    auto s2 = model.section("s2");
    auto n1 = model.node("n1");
    auto n2 = model.node("n2");
    EXPECT_NE(s1, nullptr);
    EXPECT_NE(s2, nullptr);
    EXPECT_NE(n1, nullptr);
    EXPECT_NE(n2, nullptr);
    EXPECT_EQ(n1->section(0), ID_NULL);
    EXPECT_EQ(n1->section(1), s2->id());
    EXPECT_EQ(n2->section(0), s2->id());
    EXPECT_EQ(n2->section(1), ID_NULL);
    EXPECT_EQ(s1->start(), ID_NULL);
    EXPECT_EQ(s1->end(), ID_NULL);
    EXPECT_EQ(s2->start(), n1->id());
    EXPECT_EQ(s2->end(), n2->id());
}
