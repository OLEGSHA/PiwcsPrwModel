#include <gtest/gtest.h>

#include <piwcsprwmodel.h>

#include "../src/correctness.h"

using namespace piwcs::prw;

TEST(LocalCorrectness, BidirectionalENDIsCorrect){
	Model model;
	model.newNode(END, "n1");
	model.newNode(END, "n2");
	model.newSection("s1", Section::AllowedTravel::NONE, nullptr);
	model.link("s1", "n1", 0, "n2", 0);
	ASSERT_TRUE(isLocallyCorrect(model, "n1"));
}

TEST(Correctness, Basic){
	Model model;
    model.newNode(THRU, "n1");
    model.newNode(THRU, "n2");
    model.newSection("s1", Section::AllowedTravel::BIDIR, nullptr);
    model.newSection("s2", Section::AllowedTravel::BIDIR, nullptr);
    model.link("s1", "n1", 0, "n2", 0);
    model.link("s2", "n1", 1, "n2", 1);

    ASSERT_TRUE(isCorrect(model));

}

TEST(Correctness, EmptyIsCorrect){

	ASSERT_TRUE(isCorrect(Model()));

}

TEST(Correctness, IncompleteIsIncorrect){
	Model model;
    model.newNode(THRU, "n1");
    model.newNode(THRU, "n2");
    model.newSection("s1", Section::AllowedTravel::BIDIR, nullptr);
    model.newSection("s2", Section::AllowedTravel::BIDIR, nullptr);

    ASSERT_FALSE(isComplete(model));

}
