#include <cstdint>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <vector>

enum function_t { add, sub, mult, frac, idiv, jump, greater, lesser, print, cast };
enum value_t { int_t, dbl_t, chr_t, fun_t, typ_t, nul_t, skp_t };
std::map<std::string, value_t> type_map {{"int", int_t}, {"dbl", dbl_t}, {"chr", chr_t}};

union value_u {
    int64_t int_v; // integer
    double dbl_v; // double
    char chr_v; // char
    function_t fun_v; // function
    value_t typ_v; // numeric type
};


struct value {
    value_u val;
    value_t type;

    double get_dbl() const {
        return type == dbl_t ? val.dbl_v : (type == int_t ? (double)val.int_v : (double)val.chr_v);
    }
    int64_t get_int() const {
        return type == dbl_t ? (int64_t)val.dbl_v : (type == int_t ? val.int_v : (int64_t)val.chr_v);
    }

    value operator+(const value& rhs) const {
        if (type == dbl_t || rhs.type == dbl_t) return {{.dbl_v = get_dbl() + rhs.get_dbl()}, dbl_t};
        else return {{.int_v = get_int() + rhs.get_int()}, int_t};
    }
    value operator-(const value& rhs) const {
        if (type == dbl_t || rhs.type == dbl_t) return {{.dbl_v = get_dbl() - rhs.get_dbl()}, dbl_t};
        else return {{.int_v = get_int() - rhs.get_int()}, int_t};
    }
    value operator*(const value& rhs) const {
        if (type == dbl_t || rhs.type == dbl_t) return {{.dbl_v = get_dbl() * rhs.get_dbl()}, dbl_t};
        else return {{.int_v = get_int() * rhs.get_int()}, int_t};
    }
    value operator/(const value& rhs) const {
        return {{.dbl_v = get_dbl() / rhs.get_dbl()}, dbl_t};
    }
    value idiv(const value& rhs) const {
        if (rhs.type == dbl_t) return {{.int_v = (int64_t)(get_dbl() / rhs.get_dbl())}, int_t};
        else return {{.int_v = get_int() / rhs.get_int()}, int_t};
    }
    value operator>(const value& rhs) const {
        if (type == dbl_t || rhs.type == dbl_t) return {{.int_v = get_dbl() > rhs.get_dbl()}, int_t};
        else return {{.int_v = get_int() > rhs.get_int()}, int_t};
    }
    value operator<(const value& rhs) const {
        if (type == dbl_t || rhs.type == dbl_t) return {{.int_v = get_dbl() < rhs.get_dbl()}, int_t};
        else return {{.int_v = get_int() < rhs.get_int()}, int_t};
    }
};

const value nul_v = {{}, nul_t};
const value skip_v = {{}, skp_t}; // special value that can never be written to the stack

template<typename T>
T get_val(const std::string& s) {
    std::istringstream ist(s);
    T val;
    ist >> val;
    return val;
}

value parse_tok(std::string tok) {
    size_t size = tok.size();
    if (size == 0) return nul_v;
    switch (tok[0]) {
        // 1-character tokens
        case '+': if (size != 1) return nul_v;             // add 2 values, output 1 numeric
                  else return {{.fun_v = add    }, fun_t};

        case '-': if (size != 1) return nul_v;             // substract 2 values, output 1 numeric
                  else return {{.fun_v = sub    }, fun_t};

        case '*': if (size != 1) return nul_v;             // multiply 2 values, output 1 numeric
                  else return {{.fun_v = mult   }, fun_t};

        case '<': if (size != 1) return nul_v;             // compare 2 values, output 1 int
                  else return {{.fun_v = lesser }, fun_t};

        case '>': if (size != 1) return nul_v;             // compare 2 values, output 1 int
                  else return {{.fun_v = greater}, fun_t};

        case 'p': if (size != 1) return nul_v;             // print values until \0 character encountered
                  else return {{.fun_v = print  }, fun_t};

        case 'c': if (size != 1) return nul_v;             // convert second value to type specified
                  else return {{.fun_v = cast   }, fun_t};

        // variable-character tokens
        case '/': if (size != 1 && (size != 2 || tok[1] != '/')) return nul_v;
                  else return {{.fun_v = (size == 2 ? idiv : frac)}, fun_t}; // either division or integer division

        case 'd':  return {{.dbl_v =    get_val<double>(tok.substr(1)) }, dbl_t};
        case 'i':  return {{.int_v =       get_val<int>(tok.substr(1)) }, int_t};
        case '\\': return {{.chr_v = (char)get_val<int>(tok.substr(1)) }, chr_t}; // character by-value
        case 't':  return {{.typ_v =           type_map[tok.substr(1)] }, typ_t}; // type specifier for c
        case 'j':  return {{.fun_v =                              jump }, fun_t};

        if (size < 2) return nul_v;
        // 2-character tokens
        case 'l': return {{tok[1]}, chr_t}; // character literal

        default: return nul_v;
    }
}


void push_value(std::stack<value>& stack, const value& val) {
    if (val.type != skp_t) stack.push(val);
}

value pop_value(std::stack<value>& stack) {
    if (stack.empty()) {
        std::cerr << "Stack underflow!" << std::endl;
        exit(1);
    }
    value val = stack.top();
    stack.pop();
    return val;
}

value execute_function(std::stack<value>& stack, function_t func, size_t& exec_pos) {
    switch (func) {
        case add:     return pop_value(stack) + pop_value(stack);
        case sub:     return pop_value(stack) - pop_value(stack);
        case mult:    return pop_value(stack) * pop_value(stack);
        case frac:    return pop_value(stack) / pop_value(stack);
        case idiv:    return pop_value(stack).idiv(pop_value(stack));
        case greater: return pop_value(stack) > pop_value(stack);
        case lesser:  return pop_value(stack) < pop_value(stack);
        case print: {
            value val;
            while ((val = pop_value(stack)).type != nul_t) {
                switch (val.type) {
                    case int_t: std::cout << val.val.int_v; break;
                    case dbl_t: std::cout << val.val.dbl_v; break;
                    case chr_t: std::cout << val.val.chr_v; break;
                    default: return nul_v;
                }
            }
            return skip_v;
        }
        case cast: {
            value type_to  = pop_value(stack);
            value cast_val = pop_value(stack);
            value_t& type = cast_val.type;
            value_u& val = cast_val.val;
            if (type_to.type != typ_t) return nul_v;
            switch (type_to.val.typ_v) {
                case int_t: return {{.int_v = (type == int_t ?         val.int_v : (type == dbl_t ? (int64_t)val.dbl_v : (int64_t)val.chr_v))}, int_t};
                case dbl_t: return {{.dbl_v = (type == int_t ? (double)val.int_v : (type == dbl_t ?          val.dbl_v : (double)val.chr_v))},  dbl_t};
                case chr_t: return {{.chr_v = (type == int_t ?   (char)val.int_v : (type == dbl_t ?    (char)val.dbl_v :         val.chr_v))},  int_t};
                default: return nul_v;
            }
        }
        case jump: {
            value amt = pop_value(stack);
            if (amt.type != int_t) return nul_v;
            exec_pos = amt.val.int_v;
            return skip_v;
        }
        default: return skip_v;
    }
}

int main() {
    std::stack<value> v_stack;
    std::vector<std::string> toks;
    size_t exec_pos = 0;
    while (true) {
        if (exec_pos >= toks.size()) {
            std::string tok;
            if (!(std::cin >> tok)) break;
            toks.push_back(tok);
        }
        value val = parse_tok(toks[exec_pos]);
        if (val.type == nul_t) {
            std::cerr << "Invalid token: " << toks[exec_pos] << std::endl;
            exit(1);
        }
        if (val.type == fun_t) {
            push_value(v_stack, execute_function(v_stack, val.val.fun_v, exec_pos));
        } else {
            push_value(v_stack, val);
        }
        exec_pos++;
    }
    return 0;
}
