#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <boost/program_options.hpp>
#include <iostream>
#include <components/bsa/bsa_file.hpp>

namespace BT
{

struct Argument{
    bool help;
    bool version;
    bool regex;
    bool quiet;

    std::string file;
    std::string name;
};

class Command{

private:
    Argument mArg;
    std::ostream& mOut;
    std::ostream& mError;
    boost::program_options::options_description mDesc;
    boost::program_options::positional_options_description mPod;
    Bsa::BSAFile mBsa;

public:

    Command(std::ostream& out = std::cout , std::ostream& err = std::cerr);
    bool parse(int argc, char** argv);
    void execute(int argc, char** argv);

private:

    void outputHelp();
    void outputVersion();

    void outputRecords();
    bool open();

};
}



#endif /* COMMAND_HPP */
