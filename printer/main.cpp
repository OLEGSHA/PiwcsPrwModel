#include <PiwcsPrwModel>
#include <iostream>

using namespace piwcs::prw;

int main() {

    Model model;

    model.addNode(std::make_unique<ThruNode>("12345"));

    return 0;
}
