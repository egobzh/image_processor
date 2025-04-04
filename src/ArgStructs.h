#include <string>
#include <vector>

struct FilterArgs {
    std::string name;
    std::vector<std::string> params;
};

struct FilesPaths {
    std::string input;
    std::string output;
};

struct Args {
    FilesPaths files;
    std::vector<FilterArgs> args;
};
