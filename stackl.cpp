#include "argparse/args.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <deque>
#include <string>
#include <sstream>
#include <vector>

enum function_t { add, sub, mult, frac, idiv, jump, greater, lesser, print, cast, del, end, repeat, dupe, swap };
enum value_t { int_t, dbl_t, chr_t, fun_t, typ_t, nul_t, skp_t };
std::map<std::string, value_t> str_type_map {{"int", int_t}, {"dbl", dbl_t}, {"chr", chr_t}, {"typ", typ_t}, {"nul", nul_t}};
std::map<value_t, std::string> type_str_map {{int_t, "int"}, {dbl_t, "dbl"}, {chr_t, "chr"}, {typ_t, "typ"}, {nul_t, "nul"}};

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
    int32_t amount; // how many times to repeat the value

    double get_dbl() const {
        return type == dbl_t ?          val.dbl_v : (type == int_t ? (double)val.int_v :  (double)val.chr_v);
    }
    int64_t get_int() const {
        return type == dbl_t ? (int64_t)val.dbl_v : (type == int_t ?         val.int_v : (int64_t)val.chr_v);
    }
    char get_chr() const {
        return type == dbl_t ?    (char)val.dbl_v : (type == int_t ?   (char)val.int_v :          val.chr_v);
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
    std::string to_string(bool as_value = false) const {
        std::ostringstream stream;
        switch (type) {
            case int_t: {
                stream << val.int_v;
                break;
            }
            case dbl_t: {
                stream << val.dbl_v;
                break;
            }
            case chr_t: {
                stream << (as_value ? "c" + std::to_string((int)val.chr_v) : std::string(1, val.chr_v));
                break;
            }
            case typ_t: {
                stream << type_str_map[val.typ_v];
                break;
            }
            default: {
                stream << "NUL";
                break;
            }
        }
        return stream.str();
    }
};

const value nul_v = {{}, nul_t};
const value skip_v = {{}, skp_t}; // special value that can never be written to the queue

template<typename T>
T get_val(const std::string& s) {
    std::istringstream ist(s);
    T val;
    ist >> val;
    return val;
}

value parse_tok(std::string tok, size_t& exec_pos) {
    size_t size = tok.size();
    if (size == 0) return nul_v;
    int32_t amt = get_val<int>(tok);
    size_t nnum = tok.find_first_not_of("-0123456789");
    if (tok[0] == '-' && tok.size() > 1 && nnum == 1) amt = -1; // allow expressions like -i3
    if (amt == 0) amt = 1;
    else tok = tok.substr(nnum);
    switch (tok[0]) {
        // 1-character tokens
        case '+': if (size != 1) return nul_v;             // add 2 values, output 1 numeric
                  else return {{.fun_v = add    }, fun_t, amt};

        case '-': if (size != 1) return nul_v;             // substract 2 values, output 1 numeric
                  else return {{.fun_v = sub    }, fun_t, amt};

        case '*': if (size != 1) return nul_v;             // multiply 2 values, output 1 numeric
                  else return {{.fun_v = mult   }, fun_t, amt};

        case '<': if (size != 1) return nul_v;             // compare 2 values, output 1 int
                  else return {{.fun_v = lesser }, fun_t, amt};

        case '>': if (size != 1) return nul_v;             // compare 2 values, output 1 int
                  else return {{.fun_v = greater}, fun_t, amt};

        case 'p': if (size != 1) return nul_v;             // print values until \0 character encountered
                  else return {{.fun_v = print  }, fun_t, amt};

        case 'c': if (size != 1) return nul_v;             // convert second value to type specified
                  else return {{.fun_v = cast   }, fun_t, amt};

        case '#': return {{.int_v = (int64_t)exec_pos}, int_t, amt}; // returns current execution position
        case '~': return {{.fun_v = del              }, fun_t, amt}; // pops queue
        case 'f': return {{.fun_v = end              }, fun_t, amt}; // ends execution
        case 'r': return {{.fun_v = dupe             }, fun_t, amt}; // dupes current variable
        case 'e': return {{.fun_v = repeat           }, fun_t, amt}; // moves current variable to end of queue
        case '$': return {{.fun_v = swap             }, fun_t, amt}; // outputs swapped input variables

        // variable-character tokens
        case '/': if (size != 1 && (size != 2 || tok[1] != '/')) return nul_v;
                  else return {{.fun_v = (size == 2 ? idiv : frac)}, fun_t, amt}; // either division or integer division

        case 'd':  return {{.dbl_v =    get_val<double>(tok.substr(1)) }, dbl_t, amt};
        case 'i':  return {{.int_v =       get_val<int>(tok.substr(1)) }, int_t, amt};
        case '\\': return {{.chr_v = (char)get_val<int>(tok.substr(1)) }, chr_t, amt}; // character by-value
        case 't':  return {{.typ_v =       str_type_map[tok.substr(1)] }, typ_t, amt}; // type specifier for c
        case 'j':  return {{.fun_v =                              jump }, fun_t, amt};

        if (size < 2) return nul_v;
        // 2-character tokens
        case '\'': return {{tok[1]}, chr_t}; // character literal

        default: return nul_v;
    }
}


void push_value(std::deque<value>& queue, const value& val, bool back) {
    if (val.type != skp_t) back ? queue.push_back(val) : queue.push_front(val);
}

value get_value(std::deque<value>& queue, bool back) {
    if (queue.empty()) {
        throw std::runtime_error("Stack underflow");
    }
    return back ? queue.back() : queue.front();
}
value pop_value(std::deque<value>& queue, bool back) {
    value val = get_value(queue, back);
    back ? queue.pop_back() : queue.pop_front();
    return val;
}

value execute_function(std::deque<value>& queue, function_t func, size_t& exec_pos, bool back) {
    switch (func) {
        case add:     return pop_value(queue, back) + pop_value(queue, back);
        case mult:    return pop_value(queue, back) * pop_value(queue, back);
        case repeat:  return pop_value(queue, back);
        case dupe:    return get_value(queue, back);
        case sub:     { value lhs = pop_value(queue, back); return lhs - pop_value(queue, back); }
        case frac:    { value lhs = pop_value(queue, back); return lhs / pop_value(queue, back); }
        case idiv:    { value lhs = pop_value(queue, back); return lhs.idiv(pop_value(queue, back)); }
        case greater: { value lhs = pop_value(queue, back); return lhs > pop_value(queue, back); }
        case lesser:  { value lhs = pop_value(queue, back); return lhs < pop_value(queue, back); }
        case end:     exit(0);
        case swap: {
            value old = pop_value(queue, back);
            push_value(queue, pop_value(queue, back), !back);
            return old;
        }
        case del: {
            pop_value(queue, back);
            return skip_v;
        }
        case print: {
            value val;
            while (!queue.empty() && (val = pop_value(queue, back)).to_string()[0] != '\0') {
                std::cout << val.to_string();
            }
            return skip_v;
        }
        case cast: {
            value type_to  = pop_value(queue, back);
            value cast_val = pop_value(queue, back);
            if (type_to.type != typ_t) return nul_v;
            switch (type_to.val.typ_v) {
                case int_t: return {{.int_v = cast_val.get_int() }, int_t};
                case dbl_t: return {{.dbl_v = cast_val.get_dbl() }, dbl_t};
                case chr_t: return {{.chr_v = cast_val.get_chr()}, int_t};
                default: return nul_v;
            }
        }
        case jump: {
            value amt = pop_value(queue, back);
            if (amt.type != int_t) return nul_v;
            exec_pos = amt.val.int_v - 1; // so it doesn't skip over the jumped-to instr
            return skip_v;
        }
        default: return skip_v;
    }
}

void dump_queue(std::ostream& s, std::deque<value> queue) { // copies it
    while (!queue.empty()) {
        value v = pop_value(queue, false);
        s << v.to_string(true) << " ";
    }
    s << std::endl;
}

int main(int argc, char** argv) {
    bool debug = false;
    std::string filename = "";
    auto args = {
        make_argument("debug", "d", "Print information about the program while it's running", debug),
        make_argument("file", "f", "Execute program from file", filename)
    };
    parse_arguments(args, argc, argv);

    std::shared_ptr<std::istream> in_stream_p = nullptr;
    if (filename == "") in_stream_p = std::shared_ptr<std::istream>(&std::cin);
    else {
        std::shared_ptr<std::istream> stream = std::make_shared<std::ifstream>(filename);
        if (!*stream) {
            std::cerr << "Could not open input file " << filename;
            return 1;
        }
        in_stream_p = stream;
    }
    std::istream& in_s = *in_stream_p;

    std::deque<value> v_queue;
    std::vector<std::string> toks;
    std::vector<size_t> textpos; // textual position of beginning of each token
    size_t exec_pos = 0;
    try {
    while (true) {
        while (exec_pos >= toks.size()) {
            std::string tok;
            if (!(in_s >> tok)) {
                if (in_s.eof()) return 0;
                throw std::runtime_error("Could not read token");
            }
            textpos.push_back(textpos.size() == 0 ? 0 : textpos.back() + toks.back().size() + 1);
            toks.push_back(tok);
        }
        value val = parse_tok(toks[exec_pos], exec_pos);
        if (debug) {
            std::cerr << "\nq: ";
            dump_queue(std::cerr, v_queue);
            for (const std::string& s : toks) std::cerr << s << " ";
            std::string prepend(textpos[exec_pos], ' ');
            std::cerr << '\n' << prepend << "^" << exec_pos << '\n';
        }
        switch (val.type) {
            case nul_t: {
                throw std::runtime_error("Invalid token: " + toks[exec_pos]);
            }
            case fun_t: {
                for (int32_t i = 0; i < std::abs(val.amount); ++i) push_value(v_queue, execute_function(v_queue, val.val.fun_v, exec_pos, val.amount < 0), val.amount > 0);
                break;
            }
            default: {
                for (int32_t i = 0; i < std::abs(val.amount); ++i) push_value(v_queue, val, val.amount > 0);
                break;
            }
        }
        ++exec_pos;
    }
    } catch (std::runtime_error& e) {
        std::cerr << "Caught exception while executing program: " << e.what() << std::endl;
        std::cerr << "  While trying to execute instruction " << (exec_pos >= toks.size() ? "(OOB)" : toks[exec_pos]) << " at " << exec_pos << std::endl;
        std::cerr << "  Stack dump: ";
        dump_queue(std::cerr, v_queue);
        return 1;
    }
    return 0;
}
