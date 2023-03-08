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

void scroll_back(State& state, std::string& program) {
    int depth = 0;
    size_t new_ptr = state.programPtr-1;
    while (new_ptr > 0) {
        char op = program.at(new_ptr);
        if (op == '[') {
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

void print_info(State state, std::string& program, std::vector<bigint>& memory, std::vector<bigint>& stack, char op){
    //std::cout << program << " | ";
    std::cout << "("<<(op == '\n' ? 'n' : op)<<") {" 
        << state.memoryPtr << "," 
        << state.programPtr << ","
        << state.string_mode << "} [ "; 
    for (auto e : memory) {
        std::cout << e << " ";
    } 
    std::cout << "] | ";
    for (auto e : stack) {
        std::cout << e << " ";
    }
    std::cout << "<\n";
}

void run(State state, std::string& program, std::vector<bigint>& memory, std::vector<bigint>& stack) {
    using namespace std::this_thread;
    using namespace std::chrono;

    while (state.programPtr < program.size()) {
        sleep_for(milliseconds(20));
        char op = program.at(state.programPtr);
        //print_info(state, program, memory, stack, op);
        if (state.string_mode) {
            if (op == '"') {
                state.string_mode = false;
            } else {
                stack.push_back(op);
            }
            change_program_ptr(state, program.size(), 1);
            continue;
        }
        if (op == '\n' || op == ' ' || op == '[') {
            change_program_ptr(state, program.size(), 1);
            continue;
        }
        switch (op) {
            case '+': mathOp(op, stack); break;
            case '-': mathOp(op, stack); break;
            case '*': mathOp(op, stack); break;
            case '/': mathOp(op, stack); break;
            case '%': mathOp(op, stack); break;
            
            case '>': {bigint delta = stack.back(); stack.pop_back(); if (!change_program_ptr(state, program.size(), delta._to_i64())) { goto end; };} break;
            case '<': {bigint delta = stack.back(); stack.pop_back(); if (!change_program_ptr(state, program.size(),-delta._to_i64())) { goto end; };} break;
            case '^': change_memory_ptr(state, memory, 1); break;
            case 'v': change_memory_ptr(state, memory, -1); break;
            case 'j': {bigint delta = stack.back(); stack.pop_back(); change_memory_ptr(state, memory, delta._to_i64()); } break;
            case 'J': {bigint delta = memory.at(state.memoryPtr); change_memory_ptr(state, memory, delta._to_i64()); } break;
            case 'e': {bigint tmp = stack.back() % 128; stack.pop_back(); program[state.programPtr] = static_cast<char>((tmp % 128)._to_i64()); } break;
            case 'E': {bigint tmp = memory.at(state.memoryPtr); program[state.programPtr] = static_cast<char>((tmp % 128)._to_i64()); } break;
            case ']': {bigint tmp = stack.back(); if (0!=tmp) { scroll_back(state, program); } else { stack.pop_back(); } } break;

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
        //std::cout.flush();
        change_program_ptr(state, program.size(), 1);
        if (state.programPtr >= program.size()) {
            break;
        }
    }
    end:
    return;
}

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