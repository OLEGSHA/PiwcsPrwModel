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

/**
 * A Node that has non-null connections.
 */
class ConnectedNode : public Node {
    Identifier m_section;

  public:
    ConnectedNode(const Identifier &id)
        : Node(static_cast<Node::Type>(127), id), m_section("") {}

    size_t sectionCount() const { return 1; }

    const Identifier &section(size_t i) const {
        return i == 0 ? m_section : ID_NULL;
    }

    void setSection(const Identifier &section) { m_section = section; }

    bool couldTraverse(size_t from, size_t to) const { return true; }

    void print(std::ostream &s) const { s << "Test ConnectedNode " << id(); }
};

TEST(Model, Constructor) {
    Model model;

    EXPECT_TRUE(model.nodes().empty());
    EXPECT_TRUE(model.sections().empty());
}

TEST(Model, AddNode) {
    Model model;

    auto res = model.addNode(std::make_unique<ThruNode>("123"));
    EXPECT_EQ(res, Model::ADD_OK);
    EXPECT_EQ(model.nodes().size(), 1);

    res = model.addNode(std::make_unique<ThruNode>(ID_INVALID));
    EXPECT_EQ(res, Model::ADD_BAD_ID);
    EXPECT_EQ(model.nodes().size(), 1);

    res = model.addNode(std::make_unique<ThruNode>("123"));
    EXPECT_EQ(res, Model::ADD_DUPLICATE);
    EXPECT_EQ(model.nodes().size(), 1);

    auto connected = std::make_unique<ConnectedNode>("456");
    connected->setSection("555");
    res = model.addNode(std::move(connected));
    EXPECT_EQ(res, Model::ADD_HAS_REF);
    EXPECT_EQ(model.nodes().size(), 1);

    res = model.addNode(std::make_unique<ThruNode>("456"));
    EXPECT_EQ(res, Model::ADD_OK);
    EXPECT_EQ(model.nodes().size(), 2);
}

TEST(Model, FindNode) {
    Model model;

    model.addNode(std::make_unique<ThruNode>("123"));
    model.addNode(std::make_unique<ThruNode>("456"));

    const Node *node = model.node("123");
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->id(), "123");

    node = model.node("000");
    EXPECT_EQ(node, nullptr);
}

TEST(Model, RemoveNode) {
    Model model;

    model.addNode(std::make_unique<ThruNode>("123"));

    auto res = model.removeNode("000");
    EXPECT_EQ(res, Model::REMOVE_NOT_FOUND);
    EXPECT_FALSE(model.nodes().empty());

    res = model.removeNode("123");
    EXPECT_EQ(res, Model::REMOVE_OK);
    EXPECT_TRUE(model.nodes().empty());

    // Hack in a node that has connections
    // N.B.: this part of the test is rather white box-y, may break on refactor
    auto connected = std::make_unique<ConnectedNode>("456");
    auto sneaky = connected.get();
    model.addNode(std::move(connected));
    sneaky->setSection("555");

    res = model.removeNode("456");
    EXPECT_EQ(res, Model::REMOVE_REFERENCED);
    EXPECT_FALSE(model.nodes().empty());
}

TEST(Model, AddSection) {
    Model model;

    auto res = model.addSection(std::make_unique<Section>("123", false));
    EXPECT_EQ(res, Model::ADD_OK);
    EXPECT_EQ(model.sections().size(), 1);

    res = model.addSection(std::make_unique<Section>(ID_INVALID, false));
    EXPECT_EQ(res, Model::ADD_BAD_ID);
    EXPECT_EQ(model.sections().size(), 1);

    res = model.addSection(std::make_unique<Section>("123", false));
    EXPECT_EQ(res, Model::ADD_DUPLICATE);
    EXPECT_EQ(model.sections().size(), 1);

    // N.B.: there isn't a good way to force ADD_HAS_REF at this point in unit
    // tests, so this will get covered in more advanced tests

    res = model.addSection(std::make_unique<Section>("456", false));
    EXPECT_EQ(res, Model::ADD_OK);
    EXPECT_EQ(model.sections().size(), 2);
}

TEST(Model, AddSectionWithDestination) {
    Model model;

    auto res = model.addSection(std::make_unique<Section>(
        "123", false, 0, std::make_unique<Destination>("1.0.0", "Name1")));
    EXPECT_EQ(res, Model::ADD_OK);
    EXPECT_EQ(model.sections().size(), 1);

    res = model.addSection(std::make_unique<Section>(
        "456", false, 0, std::make_unique<Destination>("1.0.1", "Name1")));
    EXPECT_EQ(res, Model::ADD_OK);
    EXPECT_EQ(model.sections().size(), 2);

    res = model.addSection(std::make_unique<Section>(
        "789", false, 0, std::make_unique<Destination>("1.0.1", "Name1")));
    EXPECT_EQ(res, Model::ADD_DUPLICATE);
    EXPECT_EQ(model.sections().size(), 2);
}

TEST(Model, FindSection) {
    Model model;

    model.addSection(std::make_unique<Section>("123", false));
    model.addSection(std::make_unique<Section>("456", false));

    const Section *section = model.section("123");
    EXPECT_NE(section, nullptr);
    EXPECT_EQ(section->id(), "123");

    section = model.section("000");
    EXPECT_EQ(section, nullptr);
}

TEST(Model, RemoveSection) {
    Model model;

    model.addSection(std::make_unique<Section>("123", false));

    auto res = model.removeSection("000");
    EXPECT_EQ(res, Model::REMOVE_NOT_FOUND);
    EXPECT_FALSE(model.sections().empty());

    res = model.removeSection("123");
    EXPECT_EQ(res, Model::REMOVE_OK);
    EXPECT_TRUE(model.sections().empty());

    // N.B.: there isn't a good way to force REMOVE_REFERENCED at this point in
    // unit tests, so this will get covered in more advanced tests
}
