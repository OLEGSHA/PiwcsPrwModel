#include <gtest/gtest.h>

#include <PiwcsPrwModel>

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

    auto res = model.addNode(Node(THRU, "123"));
    EXPECT_EQ(res, Model::AddResult::OK);
    EXPECT_EQ(model.nodes().size(), 1);

    res = model.addNode(Node(THRU, ID_INVALID));
    EXPECT_EQ(res, Model::AddResult::BAD_ID);
    EXPECT_EQ(model.nodes().size(), 1);

    res = model.addNode(Node(THRU, "123"));
    EXPECT_EQ(res, Model::AddResult::DUPLICATE);
    EXPECT_EQ(model.nodes().size(), 1);

    res = model.addNode(Node(THRU, "456"));
    EXPECT_EQ(res, Model::AddResult::OK);
    EXPECT_EQ(model.nodes().size(), 2);
}

TEST(Model, FindNode) {
    Model model;

    model.addNode(Node(THRU, "123"));
    model.addNode(Node(THRU, "456"));

    const Node *node = model.node("123");
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->id(), "123");

    node = model.node("000");
    EXPECT_EQ(node, nullptr);
}

TEST(Model, RemoveNode) {
    Model model;

    model.addNode(Node(THRU, "123"));

    auto res = model.removeNode("000");
    EXPECT_EQ(res, Model::RemoveResult::NOT_FOUND);
    EXPECT_FALSE(model.nodes().empty());

    res = model.removeNode("123");
    EXPECT_EQ(res, Model::RemoveResult::OK);
    EXPECT_TRUE(model.nodes().empty());
}

TEST(Model, AddSection) {
    Model model;

    auto res = model.addSection(Section("123", false));
    EXPECT_EQ(res, Model::AddResult::OK);
    EXPECT_EQ(model.sections().size(), 1);

    res = model.addSection(Section(ID_INVALID, false));
    EXPECT_EQ(res, Model::AddResult::BAD_ID);
    EXPECT_EQ(model.sections().size(), 1);

    res = model.addSection(Section("123", false));
    EXPECT_EQ(res, Model::AddResult::DUPLICATE);
    EXPECT_EQ(model.sections().size(), 1);

    res = model.addSection(Section("456", false));
    EXPECT_EQ(res, Model::AddResult::OK);
    EXPECT_EQ(model.sections().size(), 2);
}

TEST(Model, AddSectionWithDestination) {
    Model model;

    auto res = model.addSection(Section(
        "123", false, 0, std::make_unique<Destination>("1.0.0", "Name1")));
    EXPECT_EQ(res, Model::AddResult::OK);
    EXPECT_EQ(model.sections().size(), 1);

    res = model.addSection(Section(
        "456", false, 0, std::make_unique<Destination>("1.0.1", "Name1")));
    EXPECT_EQ(res, Model::AddResult::OK);
    EXPECT_EQ(model.sections().size(), 2);

    res = model.addSection(Section(
        "789", false, 0, std::make_unique<Destination>("1.0.1", "Name1")));
    EXPECT_EQ(res, Model::AddResult::DUPLICATE);
    EXPECT_EQ(model.sections().size(), 2);
}

TEST(Model, FindSection) {
    Model model;

    model.addSection(Section("123", false));
    model.addSection(Section("456", false));

    const Section *section = model.section("123");
    EXPECT_NE(section, nullptr);
    EXPECT_EQ(section->id(), "123");

    section = model.section("000");
    EXPECT_EQ(section, nullptr);
}

TEST(Model, RemoveSection) {
    Model model;

    model.addSection(Section("123", false));

    auto res = model.removeSection("000");
    EXPECT_EQ(res, Model::RemoveResult::NOT_FOUND);
    EXPECT_FALSE(model.sections().empty());

    res = model.removeSection("123");
    EXPECT_EQ(res, Model::RemoveResult::OK);
    EXPECT_TRUE(model.sections().empty());

    // N.B.: there isn't a good way to force RemoveResult::REFERENCED at this
    // point in unit tests, so this will get covered in more advanced tests
}
