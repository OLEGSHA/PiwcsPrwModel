#include <array>
#include <format>
#include <gtest/gtest.h>

#include <piwcsprwmodel.h>

#include "../src/correctness.h"
#include "../src/debug.h"
#include "../src/nodetypeinfo.h"

using namespace piwcs::prw;

using SType = Section::AllowedTravel;

struct SectionTestInfo {
    Section::AllowedTravel type;
    bool inward = false; // Section is directed into node if `true`
};

template <std::size_t N>
bool localCorrectnessMinimal(NodeType type,
                             const SectionTestInfo (&sections)[N]) {

    _ASSERT(N >= (type->slotCount),
            "Invalid section array provided, slot counts don't match!");

    Model model;
    model.newNode(type, "n0");

    for (SlotId slot = 0; slot < (type->slotCount); slot++) {
        Identifier nodeId = std::format("n{}", slot + 1);
        Identifier sectionId = std::format("s{}", slot);
        model.newNode(type, nodeId);
        model.newSection(sectionId, sections[slot].type, nullptr);

        if (sections[slot].inward) {
            model.link(sectionId, nodeId, 0, "n0", slot);
        } else {
            model.link(sectionId, "n0", slot, nodeId, 0);
        }
    }

    return isLocallyCorrect(model, "n0");
}

#define UNWRAP(...) __VA_ARGS__
#define INCORRECT_TEST(testName, nodeType, sectionInfo)                        \
    TEST(LocalCorrectness, nodeType##_##testName##_IsIncorrect) {              \
        bool res = localCorrectnessMinimal(nodeType, UNWRAP sectionInfo);      \
        ASSERT_EQ(res, false);                                                 \
    }

#define CORRECT_TEST(testName, nodeType, sectionInfo)                          \
    TEST(LocalCorrectness, nodeType##_##testName##_IsCorrect) {                \
        bool res = localCorrectnessMinimal(nodeType, UNWRAP sectionInfo);      \
        ASSERT_EQ(res, true);                                                  \
    }

TEST(LocalCorrectness, InvalidNode_IsIncorrect){
    Model model;
    for(Identifier id : {ID_NULL, ID_INVALID, static_cast<Identifier>("n0")}){

    EXPECT_FALSE(isLocallyCorrect(model, id))<< "Invalid id not detected: `" << (id==ID_NULL ? static_cast<Identifier>("#null") : id) << '`';
    };
}

TEST(LocalCorrectness, NodeWithEmptySlot_IsIncorrect) {
    Model model;
    model.newNode(END, "n0");
    ASSERT_FALSE(isLocallyCorrect(model, "n0"));
}

TEST(LocalCorrectness, AnyCompletelyForbidden_IsCorrect) {

    SectionTestInfo sections[Node::MAX_SLOTS];

    for (SlotId slot = 0; slot < Node::MAX_SLOTS; ++slot) {
        sections[slot] = {SType::NONE};
    }

    for (const auto *type : {
             THRU,
             MOTORIZED,
             PASSIVE,
             FIXED,
             MANUAL,
             CROSSING,
             END,
         }) {

        bool res = localCorrectnessMinimal(type, sections);
        ASSERT_TRUE(res) << "Expected correct node of type `" << type->name
                         << "`";
    }
}

CORRECT_TEST(Bidirectional, END, ({{SType::BIDIR}}));

INCORRECT_TEST(Unidirectional1, END, ({{SType::UNIDIR, true}}));

INCORRECT_TEST(Unidirectional2, END, ({{SType::UNIDIR, false}}));

CORRECT_TEST(Bidirectional, THRU, ({{SType::BIDIR}, {SType::BIDIR}}));

CORRECT_TEST(Unidirectional, THRU,
             ({{SType::UNIDIR, true}, {SType::UNIDIR, false}}));

INCORRECT_TEST(Unmatching_BIDIR_UNIDIR, THRU,
               ({{SType::UNIDIR}, {SType::BIDIR}}));

INCORRECT_TEST(Unmatching_UNIDIR_NONE, THRU, ({{SType::BIDIR}, {SType::NONE}}));

CORRECT_TEST(, MOTORIZED,
             ({{SType::UNIDIR, true}, {SType::UNIDIR}, {SType::UNIDIR}}));

INCORRECT_TEST(MismatchCommon, MOTORIZED,
               ({{SType::UNIDIR, false}, {SType::UNIDIR}, {SType::UNIDIR}}));

INCORRECT_TEST(
    MismatchStraight, MOTORIZED,
    ({{SType::UNIDIR, true}, {SType::UNIDIR, true}, {SType::UNIDIR}}));

INCORRECT_TEST(
    MismatchDiverging, MOTORIZED,
    ({{SType::UNIDIR, true}, {SType::UNIDIR}, {SType::UNIDIR, true}}));

/*
 * This situation would have been correct
 * if motorized node allowed straight -> common travel
 */
INCORRECT_TEST(MismatchCommonAsBidirectional, MOTORIZED,
               ({{SType::BIDIR}, {SType::BIDIR}, {SType::UNIDIR}}));

CORRECT_TEST(, FIXED,
             ({{SType::BIDIR}, {SType::UNIDIR}, {SType::UNIDIR, true}}));

INCORRECT_TEST(
    MismatchCommon, FIXED,
    ({{SType::BIDIR}, {SType::UNIDIR, true}, {SType::UNIDIR, true}}));

INCORRECT_TEST(MismatchDiverging, FIXED,
               ({{SType::BIDIR}, {SType::UNIDIR}, {SType::UNIDIR}}));

CORRECT_TEST(ForbiddenDiverging, FIXED,
             ({{SType::UNIDIR, true}, {SType::UNIDIR}, {SType::NONE}}));

CORRECT_TEST(, MANUAL,
             ({{SType::UNIDIR, true}, {SType::UNIDIR}, {SType::NONE}}));

INCORRECT_TEST(AllowedDiverging, MANUAL,
               ({{SType::UNIDIR, true}, {SType::UNIDIR}, {SType::UNIDIR}}));

CORRECT_TEST(AllConnected, PASSIVE,
             ({{SType::UNIDIR}, {SType::UNIDIR, true}, {SType::UNIDIR, true}}));

CORRECT_TEST(StraightConnected, PASSIVE,
             ({{SType::UNIDIR}, {SType::UNIDIR, true}, {SType::NONE}}));

CORRECT_TEST(DivergingConnected, PASSIVE,
             ({{SType::UNIDIR}, {SType::NONE}, {SType::UNIDIR, true}}));

INCORRECT_TEST(CommonForbidden, PASSIVE,
               ({{SType::NONE}, {SType::UNIDIR, true}, {SType::UNIDIR, true}}));

INCORRECT_TEST(StraightAsBidirectional, PASSIVE,
               ({{SType::BIDIR}, {SType::BIDIR}, {SType::UNIDIR, true}}));

CORRECT_TEST(
    AllBidirectional, CROSSING,
    ({{SType::BIDIR}, {SType::BIDIR}, {SType::BIDIR}, {SType::BIDIR}}));

CORRECT_TEST(
    BidirectionalAndUnidirectional, CROSSING,
    ({{SType::UNIDIR}, {SType::UNIDIR, true}, {SType::BIDIR}, {SType::BIDIR}}));

CORRECT_TEST(AllUnidirectional, CROSSING,
             ({{SType::UNIDIR},
               {SType::UNIDIR, true},
               {SType::UNIDIR},
               {SType::UNIDIR, true}}));

CORRECT_TEST(
    UnidirectionalAndForbidden, CROSSING,
    ({{SType::UNIDIR}, {SType::UNIDIR, true}, {SType::NONE}, {SType::NONE}}));

INCORRECT_TEST(
    BidirectionalMismatch, CROSSING,
    ({{SType::BIDIR}, {SType::UNIDIR, true}, {SType::NONE}, {SType::NONE}}));

INCORRECT_TEST(
    UnidirectionalMismatch, CROSSING,
    ({{SType::UNIDIR}, {SType::UNIDIR}, {SType::NONE}, {SType::NONE}}));

INCORRECT_TEST(
    ForbiddenMismatch, CROSSING,
    ({{SType::UNIDIR}, {SType::NONE}, {SType::NONE}, {SType::NONE}}));

TEST(Correctness, Basic) {
    Model model;
    model.newNode(THRU, "n1");
    model.newNode(THRU, "n2");
    model.newSection("s1", Section::AllowedTravel::BIDIR, nullptr);
    model.newSection("s2", Section::AllowedTravel::BIDIR, nullptr);
    model.link("s1", "n1", 0, "n2", 0);
    model.link("s2", "n1", 1, "n2", 1);

    ASSERT_TRUE(isCorrect(model));
}

TEST(Correctness, EmptyIsCorrect) { ASSERT_TRUE(isCorrect(Model())); }

TEST(Correctness, IncompleteIsIncorrect) {
    Model model;
    model.newNode(THRU, "n1");
    model.newNode(THRU, "n2");
    model.newSection("s1", Section::AllowedTravel::BIDIR, nullptr);
    model.newSection("s2", Section::AllowedTravel::BIDIR, nullptr);

    ASSERT_FALSE(isComplete(model));
}
