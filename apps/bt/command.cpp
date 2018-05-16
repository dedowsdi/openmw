#include "command.hpp"
#include <regex>

namespace BT
{

//--------------------------------------------------------------------
Command::Command(std::ostream& out/* = std::cout */, std::ostream& err/* = std::cerr*/):
    mOut(out),
    mError(err)
{
    namespace bpo = boost::program_options;
    mDesc.add_options()
        ("help", bpo::bool_switch(&mArg.help), "output help")
        ("version", bpo::bool_switch(&mArg.version), "output version")
        ("quiet,q", bpo::bool_switch(&mArg.quiet), "supress error message")
        ("file,f", bpo::value<std::string>(&mArg.file), "bsa file, otherwise use $BSA_DATA")
        ("name,n", bpo::value<std::string>(&mArg.name), "record name")
        ("regex", bpo::bool_switch(&mArg.regex), "record name")
        ;

    mPod.add("file", 1);
}

//--------------------------------------------------------------------
bool Command::parse(int argc, char** argv)
{
    namespace bpo = boost::program_options;
    bpo::variables_map vm;
    bpo::store(bpo::command_line_parser(argc, argv).options(mDesc).positional(mPod).run(), vm);
    notify(vm);

    if (mArg.file.empty()) {
        char* data = std::getenv("BSA_DATA");
        mArg.file = data;
    }

    return true;
}

//--------------------------------------------------------------------
void Command::execute(int argc, char** argv)
{
    if(!parse(argc, argv))
        return;

    if(!open())
        return;

    outputRecords();
}

//--------------------------------------------------------------------
void Command::outputHelp()
{
    mOut << mDesc;
}

//--------------------------------------------------------------------
void Command::outputVersion()
{
    mOut << "bt 0.00" << std::endl;
}

//--------------------------------------------------------------------
void Command::outputRecords()
{
    const Bsa::BSAFile::FileList& fl = mBsa.getList();
    std::regex namePattern(mArg.name);

    for(const Bsa::BSAFile::FileStruct& item : fl)
    {
        if (!mArg.name.empty()) {
           if ( (!mArg.regex && mArg.name != Misc::StringUtils::lowerCase(item.name)) ||
                (mArg.regex && !std::regex_match(Misc::StringUtils::lowerCase(item.name), namePattern)) ) {
               continue;
           } 
        }
        
        mOut << item.name << std::endl;
    }
}

//--------------------------------------------------------------------
bool Command::open()
{
    if (mArg.file.empty()) {
        if(!mArg.quiet)
            mError << "need bsa file" << std::endl;
       return false; 
    }

    mBsa.open(mArg.file);
    return true;
}

}
