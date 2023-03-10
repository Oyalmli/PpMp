#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

#include "BigInt.hpp"

std::string extension(std::string file_name) {
    int position = file_name.find_last_of(".");
    std::string result = file_name.substr(position + 1);
    return result;
}

struct State {
    size_t programPtr = 0;
    size_t memoryPtr = 0;
    bool string_mode = false;
};

bool change_program_ptr(State& state, size_t program_length, int delta){
    if (delta == 1 && state.programPtr == program_length) {
        std::cerr << "incremented outside program\n";
        return 0;
    }
    if (delta == -1 && state.programPtr == 0) {
        std::cerr << "decremented outside program\n";
        return 0;
    }
    state.programPtr += delta;
    return 1;
}

void change_memory_ptr(State& state, std::vector<bigint>& memory, int delta){
    //find difference from memory_ptr to start, if delta less, add x to front
    if ((static_cast<int64_t>(state.memoryPtr) + delta) < memory.size() && (static_cast<int64_t>(state.memoryPtr) + delta) >= 0) {
        state.memoryPtr += delta;
        return;
    }
    int64_t delta_copy = delta;
    while (delta_copy < 0) {
        delta_copy++;
        memory.emplace(memory.begin(),0);
    }
    while (delta_copy > 0) {
        memory.push_back(0);
        delta_copy--;
    }
    state.memoryPtr = delta < 0 ? 0 : static_cast<size_t>(static_cast<int64_t>(state.memoryPtr) + delta);
}

void scroll_back(State& state, std::string& program, char end_char) {
    int depth = 0;
    size_t new_ptr = state.programPtr-1;
    while (new_ptr > 0) {
        char op = program.at(new_ptr);
        if (op == end_char) {
            if (depth == 0) {
                break;
            }
            depth--;
        } else 
        if (op == ']') {
            depth ++;
        }
        new_ptr--;
    }
    state.programPtr = new_ptr;
}

void mathOp(const char op, std::vector<bigint>& stack) {
    auto a = stack.back();
    stack.pop_back();
    auto b = stack.back();
    stack.pop_back();

    switch (op) {
        case '+': stack.push_back(a+b); break;
        case '-': stack.push_back(a-b); break;
        case '*': stack.push_back(a*b); break;
        case '/': stack.push_back(a/b); break;
        case '%': stack.push_back(a%b); break;
        default: std::cout << "Missing impl in mathOp: " << op << "\n"; 
    }
}

const char *white = "\u001b[38;5;195m";
const char *yellow =  "\u001b[38;5;220m";
const char *orange =  "\u001b[38;5;208m";
const char *green =  "\u001b[38;5;118m";
const char *red =  "\u001b[38;5;197m";
const char *clear = "\u001b[0m";

void print_info(const State& state, const std::string& program, const std::vector<bigint>& memory, const std::vector<bigint>& stack, const char &op){
    fprintf(stderr, "%s%s%c%s%s (%2zu %2zu %c)%s [ ", clear, white, (op == '\n' ? 'n' : op), clear, yellow,state.memoryPtr, state.programPtr, state.string_mode ? 'T' : 'F', clear);
    for (int i = 0; i < memory.size(); ++i) {
        auto e = memory.at(i);
        if (i == state.memoryPtr) {
            std::cerr << orange << e << clear << " ";
        } else {
            std::cerr << e << " ";
        }
    }
    std::cerr << "] | ";
    for (int i = 0; i < stack.size(); ++i) {
        auto e = stack.at(i);
        if (i == stack.size()-1) {
            std::cerr << green << e << clear << " ";
        } else {
            std::cerr << e << " ";
        }
    }
    std::cerr << red << '\n';
}

bool step(State& state, std::string& program, std::vector<bigint>& memory, std::vector<bigint>& stack) {
    char op = program.at(state.programPtr);
    if (state.string_mode) {
        if (op == '"') {
            state.string_mode = false;
        } else {
            stack.push_back(op);
        }
        change_program_ptr(state, program.size(), 1);
        return 1;
    }
    if (op == '\n' || op == ' ' || op == '[') {
        change_program_ptr(state, program.size(), 1);
        return 1;
    }
    switch (op) {
        case '+': mathOp(op, stack); break;
        case '-': mathOp(op, stack); break;
        case '*': mathOp(op, stack); break;
        case '/': mathOp(op, stack); break;
        case '%': mathOp(op, stack); break;
        
        case '>': {bigint delta = stack.back(); stack.pop_back(); if (!change_program_ptr(state, program.size(), delta._to_i64())) { return 0; };} break;
        case '<': {bigint delta = stack.back(); stack.pop_back(); if (!change_program_ptr(state, program.size(),-delta._to_i64())) { return 0; };} break;
        case '^': change_memory_ptr(state, memory, 1); break;
        case 'v': change_memory_ptr(state, memory, -1); break;
        case 'j': {bigint delta = stack.back(); stack.pop_back(); change_memory_ptr(state, memory, delta._to_i64()); } break;
        case 'J': {bigint delta = memory.at(state.memoryPtr); change_memory_ptr(state, memory, delta._to_i64()); } break;
        case 'e': {bigint tmp = stack.back() % 128; stack.pop_back(); program[state.programPtr] = static_cast<char>((tmp % 128)._to_i64()); } break;
        case 'E': {bigint tmp = memory.at(state.memoryPtr); program[state.programPtr] = static_cast<char>((tmp % 128)._to_i64()); } break;
        case ']': {bigint tmp = stack.back(); if (0 != tmp) { scroll_back(state, program, '['); }} break;
        case ')': {bigint tmp = memory.at(state.memoryPtr);     if (0 != tmp) { scroll_back(state, program, '('); }} break;

        case '0': stack.push_back(0); break;
        case '1': stack.push_back(1); break;
        case '2': stack.push_back(2); break;
        case '3': stack.push_back(3); break;
        case '4': stack.push_back(4); break;
        case '5': stack.push_back(5); break;
        case '6': stack.push_back(6); break;
        case '7': stack.push_back(7); break;
        case '8': stack.push_back(8); break;
        case '9': stack.push_back(9); break;

        case '"': state.string_mode = true; break;
        case 'w': {std::cout << static_cast<char>((stack.back() % 128)._to_i64()); stack.pop_back();} break;
        case 'W': std::cout << static_cast<char>((memory.at(state.memoryPtr) % 128)._to_i64()); break;
        case 'p': {std::cout << stack.back(); stack.pop_back(); } break;
        case 'P': std::cout << memory.at(state.memoryPtr); break;
        case 'n': {int64_t tmp; std::cin >> tmp; stack.push_back(tmp); } break;
        case 'N': {int64_t tmp; std::cin >> tmp; memory[state.memoryPtr] = tmp; } break;
        case 'c': {char tmp; std::cin >> tmp; stack.push_back(tmp); } break;
        case 'C': {char tmp; std::cin >> tmp; memory[state.memoryPtr] = tmp; } break;

        case 'g': stack.push_back(memory.at(state.memoryPtr)); break;
        case 's': {bigint tmp = stack.back(); stack.pop_back(); memory[state.memoryPtr] = tmp; } break;
        case 'd': {int64_t delta = stack.back()._to_i64(); stack.pop_back(); memory[state.memoryPtr] = memory[state.memoryPtr + delta]; } break;

        case ':': { stack.push_back(stack.back()); } break;
        case '$': { stack.pop_back(); } break;
        case 'z': { 
            bigint a = stack.back(); stack.pop_back();
            bigint b = stack.back(); stack.pop_back();
            stack.push_back(a); stack.push_back(b);
        } break;
        default: {
            std::cout << "Op not implemented: " << op << "\n";
        }
    }
    change_program_ptr(state, program.size(), 1);
    if (state.programPtr >= program.size()) {
        return 0;
    }
    return 1;
}
void run(State state, std::string& program, std::vector<bigint>& memory, std::vector<bigint>& stack) {
    using namespace std::this_thread;
    using namespace std::chrono;
    do {
        #ifdef SLEEP_TIME 
            sleep_for(milliseconds(SLEEP_TIME));
        #endif
        #ifdef PRINT_INFO 
            print_info(state, program, memory, stack, program.at(state.programPtr)); 
        #endif
    } while (step(state, program, memory, stack));
    return;
}
