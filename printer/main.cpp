#include <iostream>
#include <piwcsprwmodel.h>

using namespace piwcs::prw;

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cerr << "Usage: " << (argc == 0 ? "printer" : argv[0]) << " FILE"
                  << std::endl;
        return 1;
    }

    Model model = readModel(argv[1]);

    std::cout << "Nodes:\n";
    for (const auto &it : model.nodes()) {
        std::cout << '\t' << it.second << '\n';
    }

    std::cout << "\nSections:\n";
    for (const auto &it : model.sections()) {
        std::cout << '\t' << it.second << '\n';
    }

    return 0;
}
