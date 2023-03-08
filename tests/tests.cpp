#include <algorithm>
#include <vector>

#include "../ppmp.hpp"
#include "catch.hpp"

TEST_CASE("Math") {
    SECTION("+ Adds") {
        std::vector<bigint> memory = {0};
        std::vector<bigint> stack = {1, 2};
        State state;
        std::string program = "+";
        run(state, program, memory, stack);
        REQUIRE(stack.back()._to_i64() == 3);
    }
    SECTION("- Subtracts") {
        std::vector<bigint> memory = {0};
        std::vector<bigint> stack = {1, 2};
        State state;
        std::string program = "-";
        run(state, program, memory, stack);
        REQUIRE(stack.back()._to_i64() == 1);
    }
    SECTION("* Multiplies") {
        std::vector<bigint> memory = {0};
        std::vector<bigint> stack = {3, 5};
        State state;
        std::string program = "*";
        run(state, program, memory, stack);
        REQUIRE(stack.back()._to_i64() == 15);
    }
    SECTION("/ Divides") {
        std::vector<bigint> memory = {0};
        std::vector<bigint> stack = {4, 16};
        State state;
        std::string program = "/";
        run(state, program, memory, stack);
        REQUIRE(stack.back()._to_i64() == 4);
    }
    SECTION("% Modulos") {
        std::vector<bigint> memory = {0};
        std::vector<bigint> stack = {2, 5};
        State state;
        std::string program = "%";
        run(state, program, memory, stack);
        REQUIRE(stack.back()._to_i64() == 1);
    }
}
