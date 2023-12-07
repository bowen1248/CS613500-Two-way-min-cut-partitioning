#include "Parser/ArgumentParser.hpp"
#include "Parser/Parser.hpp"
#include "Partitioner/Partitioner.hpp"

int main(int argc, char *argv[])
{
    ArgumentParser argParser;
    if (!argParser.parse(argc, argv))
        return 1;

    Parser parser;
    auto input = parser.parse(argParser.inputFile);
    Partitioner partitioner(input.get());
    auto result = partitioner.solve();
    result->write(argParser.outputFile);
    
    return 0;
}
