#include <iostream>
#include "ppmp.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Missing program arguments\n";
        std::cerr << "example:\n";
        std::cerr << "./fpmp program.ppmp\n";
        return 1;
    }
    if (extension(argv[1]) != "ppmp") {
        std::cerr << "wrong file extension\n";
        return 1;
    }
    std::ifstream t(argv[1]);
    std::string program((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());

    std::vector<bigint> memory = {0};
    std::vector<bigint> stack;
    State state;
    run(state, program, memory, stack);
    //print_info(state, program, memory, stack, ' ');
}