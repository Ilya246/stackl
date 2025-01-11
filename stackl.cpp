#include <cstdint>
#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <vector>

enum function_t { add, sub, mult, frac, idiv, jump, greater, lesser, print };

union value_u {
    int64_t int_v; // integer
    double dbl_v; // double
    char chr_v; // char
    function_t fun_v; // function
};

enum value_t { int_t, dbl_t, chr_t, fun_t, nul_t };

struct value {
    value_u val;
    value_t type;
};

const value nul_v = {{}, nul_t};

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
        case '+': if (size != 1) return nul_v;
                  else return {{.fun_v = add                      }, fun_t};

        case '-': if (size != 1) return nul_v;
                  else return {{.fun_v = sub                      }, fun_t};

        case '*': if (size != 1) return nul_v;
                  else return {{.fun_v = mult                     }, fun_t};

        case '<': if (size != 1) return nul_v;
                  else return {{.fun_v = lesser                   }, fun_t};

        case '>': if (size != 1) return nul_v;
                  else return {{.fun_v = greater                  }, fun_t};

        case 'p': if (size != 1) return nul_v;
                  else return {{.fun_v = print                    }, fun_t};

        // variable-character tokens
        case '/': if (size != 1 && (size != 2 || tok[1] != '/')) return nul_v;
                  else return {{.fun_v = (size == 2 ? idiv : frac)}, fun_t}; // either division or integer division

        case 'd': return {{.dbl_v = get_val<double>    (tok.substr(1)) }, dbl_t};
        case 'i': return {{.int_v = get_val<int>       (tok.substr(1)) }, int_t};
        case '\\': return {{.chr_v = (char)get_val<int> (tok.substr(1)) }, chr_t};
        case 'j': return {{.fun_v = jump                               }, fun_t};

        if (size < 2) return nul_v;
        // 2-character tokens
        case 'c': return {{tok[1]}, chr_t};

        default: return nul_v;
    }
}

int main() {
    std::stack<value> v_stack;
    std::vector<std::string> toks;
    size_t exec_pos = 0;
    while (true) {
        if (exec_pos >= toks.size()) {
            std::string tok;
            std::cin >> tok;
            toks.push_back(tok);
        }
    }
}
