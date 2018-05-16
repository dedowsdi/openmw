#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <boost/program_options.hpp>
#include <ostream>
#include <iostream>
#include <regex>
#include <components/esm/records.hpp>
#include <components/esm/esmreader.hpp>
#include "components/to_utf8/to_utf8.hpp"
#include "record.hpp"
#include "textnumber.hpp"

namespace ET
{

struct Argument{

    bool listType;
    bool listMgef;
    bool listSkil;
    bool listPropertyName;
    bool listInventory;

    bool hideType;
    bool version;
    bool help;
    bool regex;
    bool quiet;

    int verbose;
    int cellRefVerbose;

    std::string file;
    std::string id;
    std::vector<ESM::NAME> types;
    std::string cellRefId;
    std::vector<ESM::NAME> cellRefTypes;
    std::string cellIncludeId;
    std::vector<ESM::NAME> cellIncludeTypes;
    std::string encoding;
    std::string listProperty;
    std::string includeProperty;
    TextNumber propertyValue;
    std::string includeInventory;
    TextNumber includeInventoryCount;
    std::string includeSpell;

    std::vector<std::string> stringTypes;
    std::vector<std::string> stringCellRefTypes;
    std::vector<std::string> stringCellIncludeTypes;
};

class Command
{
private:
    Argument mArg;
    std::ostream& mOut;
    std::ostream& mError;
    ESM::ESMReader mReader;
    ESM::ESM_Context mStartContext;
    ToUTF8::Utf8Encoder* mEncoder;

    std::map<std::string, ESM::NAME> mIdTypeMap;

    boost::program_options::options_description mDesc;
    boost::program_options::positional_options_description mPod;
    boost::program_options::variables_map mVariables;

    static std::vector<ESM::NAME> sTypes;

public:

    Command( std::ostream& out = std::cout, std::ostream& err = std::cerr );
    ~Command();

    void execute(int argc, char** argv);

    static ESM::NAME stringToNAME(const std::string& s);
    static ESM::NAME stringToType(const std::string& s);
    static ESM::NAME uintToNAME(uint32_t u);
    static bool isReferenceable(ESM::NAME type);
    static bool hasInventory(ESM::NAME type);
    static bool hasSpell(ESM::NAME type);

private:

    void outputHelp();
    void outputVersion();

    void outputTypes();
    void outputMagicEffects();
    void outputSkills();
    void outputPropertyNames();
    void outputProperty();
    void outputInventories();
    void outputRecords();
    void outputCellReferences(ESM::Cell& cell);
    void outputCellIncludeReference();

    bool open();
    bool parse(int argc, char** argv);

    void buildIdMap();

    bool getNextCellRefId(ESM::CellRef &ref);

    void skipCellRefData();

    bool testId(const std::string& id, const std::string& text, const std::regex& pattern);
    bool testType(const std::vector<ESM::NAME> types, ESM::NAME type);

};

}

#endif /* COMMAND_HPP */
