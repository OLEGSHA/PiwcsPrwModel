#include <gtest/gtest.h>

#include <PiwcsPrwModel>

using namespace piwcs::prw;

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

TEST(DataModel, Constructor) {
    Model model;

    EXPECT_TRUE(model.nodes().empty());
    EXPECT_TRUE(model.sections().empty());
}

TEST(DataModel, AddNode) {
    Model model;

    auto res = model.addNode(std::make_unique<ThruNode>("123"));
    EXPECT_EQ(res, Model::ADD_OK);
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

TEST(DataModel, FindNode) {
    Model model;

    model.addNode(std::make_unique<ThruNode>("123"));
    model.addNode(std::make_unique<ThruNode>("456"));

    const Node *node = model.node("123");
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->id(), "123");

    node = model.node("000");
    EXPECT_EQ(node, nullptr);
}

TEST(DataModel, RemoveNode) {
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
