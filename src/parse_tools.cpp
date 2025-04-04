#include "ParseArgs.h"
#include <stdexcept>
#include <cctype>

namespace {
FilesPaths ParsePaths(int argc, char** argv) {
    if (argc < 3) {
        throw std::invalid_argument("The path to the input and/or output file is not specified");
    }
    return FilesPaths{argv[1], argv[2]};
}
bool IsFilterName(std::string& s) {
    if (s.starts_with("-") && s.size() >= 2 && std::isalpha(s[1])) {
        return true;
    }
    return false;
}
}  // namespace

Args ParseArgs(int argc, char** argv) {
    Args result;
    result.files = ParsePaths(argc, argv);
    FilterArgs cur_arg{"", {}};
    for (int i = 3; i < argc; i++) {
        std::string cur = argv[i];
        if (IsFilterName(cur)) {
            if (!cur_arg.name.empty()) {
                result.args.push_back(cur_arg);
                cur_arg.name = "";
                cur_arg.params.clear();
            }
            cur_arg.name = cur.substr(1);
        } else {
            if (cur_arg.name.empty()) {
                throw std::invalid_argument("Parametr " + cur + " without parents filter");
            }
            cur_arg.params.push_back(cur);
        }
    }
    if (!cur_arg.name.empty()) {
        result.args.push_back(cur_arg);
    }
    return result;
}