#include "src/ParseArgs.h"
#include "src/FileWorking.h"
#include "src/Filters.h"
#include "src/Help.h"
#include <iostream>
#include <exception>
#include <stdexcept>
#include <format>

int main(int argc, char** argv) {
    if (argc == 1) {
        WriteHelp();
        return 0;
    }
    try {
        Args parsed_args = ParseArgs(argc, argv);
        Image image = ReadBMP(parsed_args.files.input);
        auto filters_map = GetFilters();
        for (const FilterArgs& cur_arg : parsed_args.args) {
            if (!filters_map.contains(cur_arg.name)) {
                throw std::invalid_argument(std::format("Cant find filter with name {}", cur_arg.name));
            }
            std::unique_ptr<Filter> cur_filter = filters_map[cur_arg.name](cur_arg.params);
            image = cur_filter->Apply(image);
        }
        WriteBMP(image, parsed_args.files.output);

    } catch (const std::exception& exception) {
        std::cerr << exception.what();
        return 1;
    }
    return 0;
}
