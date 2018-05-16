#include "command.hpp"
#include <boost/program_options.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include "components/esm/loadmgef.hpp"
#include "components/esm/loadskil.hpp"
#include "components/misc/stringops.hpp"
#include "components/esm/defs.hpp"
#include <regex>

namespace ET
{

std::vector<ESM::NAME> Command::sTypes = {
	uintToNAME(ESM::REC_ACTI),
	uintToNAME(ESM::REC_ALCH),
	uintToNAME(ESM::REC_APPA),
	uintToNAME(ESM::REC_ARMO),
	uintToNAME(ESM::REC_BODY),
	uintToNAME(ESM::REC_BOOK),
	uintToNAME(ESM::REC_BSGN),
	uintToNAME(ESM::REC_CELL),
	uintToNAME(ESM::REC_CLAS),
	uintToNAME(ESM::REC_CLOT),
	//uintToNAME(ESM::REC_CNTC),
	uintToNAME(ESM::REC_CONT),
	uintToNAME(ESM::REC_CREA),
	//uintToNAME(ESM::REC_CREC),
	uintToNAME(ESM::REC_DIAL),
	uintToNAME(ESM::REC_DOOR),
	uintToNAME(ESM::REC_ENCH),
	uintToNAME(ESM::REC_FACT),
	uintToNAME(ESM::REC_GLOB),
	uintToNAME(ESM::REC_GMST),
	uintToNAME(ESM::REC_INFO),
	uintToNAME(ESM::REC_INGR),
	uintToNAME(ESM::REC_LAND),
	//uintToNAME(ESM::REC_LEVC),
	uintToNAME(ESM::REC_LEVI),
	uintToNAME(ESM::REC_LIGH),
	uintToNAME(ESM::REC_LOCK),
	uintToNAME(ESM::REC_LTEX),
	uintToNAME(ESM::REC_MGEF),
	uintToNAME(ESM::REC_MISC),
	uintToNAME(ESM::REC_NPC_),
	//uintToNAME(ESM::REC_NPCC),
	uintToNAME(ESM::REC_PGRD),
	uintToNAME(ESM::REC_PROB),
	uintToNAME(ESM::REC_RACE),
	uintToNAME(ESM::REC_REGN),
	uintToNAME(ESM::REC_REPA),
	uintToNAME(ESM::REC_SCPT),
	uintToNAME(ESM::REC_SKIL),
	uintToNAME(ESM::REC_SNDG),
	uintToNAME(ESM::REC_SOUN),
	uintToNAME(ESM::REC_SPEL),
	uintToNAME(ESM::REC_SSCR),
	uintToNAME(ESM::REC_STAT),
	uintToNAME(ESM::REC_WEAP)
};

//--------------------------------------------------------------------
Command::Command( std::ostream& out, std::ostream& err):
    mOut(out),
    mError(err),
    mEncoder(0)
{
    namespace bpo = boost::program_options;

    mDesc.add_options()
    ("help", bpo::bool_switch(&mArg.help), "print help")
    ("version", bpo::bool_switch(&mArg.version), "print version")

    ("verbose,v", bpo::value<int>(&mArg.verbose)->default_value(1),
         "verbose of records:"
         "  0 : \n"
         "  1 : id\n"
         "  2 : id + content\n"
         "  3 : id + content + {dialog script | book content }"
     )
    ("cellref-verbose,V", bpo::value<int>(&mArg.cellRefVerbose)->default_value(0),
         "verbose of cell references:"
         "  0 : \n"
         "  1 : id\n"
         "  2 : id + content\n"
     )

    ("type,t", bpo::value<std::vector<std::string>>(&mArg.stringTypes)->multitoken(), "constrain type")
    ("id,i", bpo::value<std::string>(&mArg.id), "contrain record id")

    ("cellref-type", bpo::value<std::vector<std::string>>(&mArg.stringCellRefTypes)->multitoken(),
        "constrain cellref-type, will be ignored if cellref-verbose is 0")
    ("cellref-id", bpo::value<std::string>(&mArg.cellRefId), "contrain cell reference id")

    ("cell-include-id,I", bpo::value<std::string>(&mArg.cellIncludeId),
         "find all cells that contain this referenceable")
    ("cell-include-type,T", bpo::value<std::vector<std::string>>(&mArg.stringCellIncludeTypes)->multitoken(),
        "find all cells that contain this referenceable type")

    ("list-type", bpo::bool_switch(&mArg.listType), "list record types")
    ("list-mgef", bpo::bool_switch(&mArg.listMgef), "list magic effects")
    ("list-skil", bpo::bool_switch(&mArg.listSkil), "list skills")
    ("list-property-name", bpo::bool_switch(&mArg.listPropertyName), "list property name")
    ("list-property", bpo::value<std::string>(&mArg.listProperty), "list property values")
    ("list-inventory", bpo::bool_switch(&mArg.listInventory), "list inventory")

    ("include-property", bpo::value<std::string>(&mArg.includeProperty),
        "constrain record with include property")
    ("property-value", bpo::value<TextNumber>(&mArg.propertyValue),
        "value of include-property:\n"
        "  text without ':'   plain text\n"
        "  :max               upper bounded number\n"
        "  min:               lower bounded number\n"
        "  min:max            bounded number"
    )

    ("include-inventory", bpo::value<std::string>(&mArg.includeInventory),
        "constrain record with include inventory")
    ("include-inventory-count", bpo::value<TextNumber>(&mArg.includeInventoryCount),
        "constrain record with include inventory count, use the same pattern as property-value, except it must be a number")

    ("include-spell", bpo::value<std::string>(&mArg.includeSpell),
        "constrain record with include spell")

    ("quiet,q", bpo::bool_switch(&mArg.quiet), "supress error message")
    ("hide-type,h", bpo::bool_switch(&mArg.hideType), "hide type before id")
    ("regex,r", bpo::bool_switch(&mArg.regex), "apply regex match to all id and plain property value related compare")
    ("file,f", bpo::value<std::string>(&mArg.file), "esm data file, otherwise read from $ESM_DATA")
    ("encoding,e", bpo::value<std::string>(&mArg.encoding)->default_value("win1252"), "encoding")
    ;

    mPod.add("file", 1);
}

//--------------------------------------------------------------------
Command::~Command()
{
    if (mEncoder) {
        delete mEncoder;
    }
}

//--------------------------------------------------------------------
void Command::execute(int argc, char** argv)
{
    mIdTypeMap.clear();
    if(!parse(argc, argv))
        return;

    if (mArg.help){
        outputHelp();
        return;
    }

    if (mArg.version){
        outputVersion();
        return;
    }

    if (mArg.listType){
        outputTypes();
        return;
    }

    if (mArg.listMgef){
        outputMagicEffects();
        return;
    }

    if (mArg.listSkil){
        outputSkills();
        return;
    }

    if (mArg.listPropertyName){
        outputPropertyNames();
        return;
    }

    if (!open()) {
        return;
    }

    if (mArg.listInventory) {
        outputInventories();
        return;
    }

    // all operations below need esm file
    if (!mArg.listProperty.empty()) {
        outputProperty();
        return;
    }

    if (!mArg.cellIncludeId.empty() || !mArg.cellIncludeTypes.empty()) {
        outputCellIncludeReference();
        return;
    }

    outputRecords();
}

//--------------------------------------------------------------------
void Command::outputHelp()
{
    mOut << mDesc << std::endl;
}

//--------------------------------------------------------------------
void Command::outputVersion()
{
    mOut << "et 0.00" << std::endl;
}

//--------------------------------------------------------------------
void Command::outputTypes()
{
    for(auto& type : sTypes)
    {
        if (!mArg.includeProperty.empty()) {
            RecordBase* record = RecordBase::create(type);
            if (!record)
                continue;

            auto& properties = record->getPropertyMap();
            if (properties.find(mArg.includeProperty) == properties.end()) {
               continue;
            }
        }
        std::cout << Misc::StringUtils::lowerCase(type.toString()) << std::endl;
    }
}

//--------------------------------------------------------------------
void Command::outputMagicEffects()
{
    for (int i = 0; i < ESM::MagicEffect::Length; ++i) {
        mOut << ESM::MagicEffect::indexToId(i).substr(4) << std::endl;
    }
}

//--------------------------------------------------------------------
void Command::outputSkills()
{
    for (int i = 0; i < ESM::Skill::Length; ++i) {
        mOut << ESM::Skill::indexToId(i).substr(3) << std::endl;
    }
}

//--------------------------------------------------------------------
void Command::outputPropertyNames()
{
    try {
        std::set<std::string> names;
        const std::vector<ESM::NAME>& types = mArg.types.empty() ? sTypes : mArg.types;
        for(auto type : types)
        {
            RecordBase* record = RecordBase::create(type);
            PropertyMap& pm = record->getPropertyMap();

            for(const auto& pair : pm)
            {
                names.insert(pair.first);
            }
        }

        for(const auto& name : names)
        {
            mOut << name << std::endl;
        }

    }catch(const std::exception& e) {
        if(!mArg.quiet)
            mError << "error:\n" << e.what() << std::endl;
    }
}

//--------------------------------------------------------------------
void Command::outputProperty()
{
    try {
        std::set<std::string> properties;
        while(mReader.hasMoreRecs()) {
            ESM::NAME type = mReader.getRecName();
            uint32_t flags;
            mReader.getRecHeader(flags);

            if(!testType(mArg.types, type)){
                mReader.skipRecord();
                continue;
            }

            ET::RecordBase* record = ET::RecordBase::create(type);
            if (record == 0) {
                mOut << "skipping " << type.toString() << std::endl;
                mReader.skipRecord();
                continue;
            }

            if (!record->hasProperty(mArg.listProperty)){
                mReader.skipRecord();
                continue;
            }

            record->load(mReader);
            const std::string&& value = record->getProperty(mArg.listProperty);
            if (!value.empty()) {
               properties.insert(value);
            }
        }

        for(const auto& value : properties)
        {
            mOut << value << std::endl;
        }

    }catch(const std::exception& e) {
        if(!mArg.quiet)
            mError << "error:\n" << e.what() << std::endl;
    }
}

//--------------------------------------------------------------------
void Command::outputInventories()
{
    try {
        std::set<std::string> names;
        while(mReader.hasMoreRecs()) {
            ESM::NAME type = mReader.getRecName();
            uint32_t flags;
            mReader.getRecHeader(flags);

            if(!hasInventory(type)){
                mReader.skipRecord();
                continue;
            }

            if(!testType(mArg.types, type)){
                mReader.skipRecord();
                continue;
            }

            ET::RecordBase* record = ET::RecordBase::create(type);

            record->load(mReader);
            const ESM::InventoryList* il = record->getInventoryList();
            for(const auto& item : il->mList)
            {
                names.insert(item.mItem.toString());
            }
        }

        for(const auto& item : names)
        {
           mOut << item << std::endl;
        }

    }catch(const std::exception& e) {
        if(!mArg.quiet)
            mError << "error:\n" << e.what() << std::endl;
    }

}

//--------------------------------------------------------------------
void Command::outputRecords()
{
    try {

        std::regex idPattern(mArg.id);
        ESM::ESM_Context context;

        while(mReader.hasMoreRecs()) {
            ESM::NAME type = mReader.getRecName();
            uint32_t flags;
            mReader.getRecHeader(flags);

            if (!testType(mArg.types, type)) {
                mReader.skipRecord();
                continue;
            }

            if (!mArg.includeInventory.empty() || !mArg.includeInventoryCount.empty()) {
                if(!hasInventory(type)){
                    mReader.skipRecord();
                    continue;
                }
            }

            if (!mArg.includeSpell.empty() && !hasSpell(type)) {
                mReader.skipRecord();
                continue;
            }

            ET::RecordBase* record = ET::RecordBase::create(type);
            if (record == 0) {
                mOut << "skipping " << type.toString() << std::endl;
                mReader.skipRecord();
                continue;
            }

            record->setFlags(flags);

            if (!mArg.includeProperty.empty() && !record->hasProperty(mArg.includeProperty)){
                mReader.skipRecord();
                continue;
            }

            bool loaded = false;
            bool loadRequire = mArg.verbose > 1
                            || (type == ESM::REC_CELL && mArg.cellRefVerbose > 0)
                            || !mArg.includeProperty.empty()
                            || !mArg.includeInventory.empty()
                            || !mArg.includeInventoryCount.empty()
                            || !mArg.includeSpell.empty()
                            ;

            if (loadRequire)
                context = mReader.getContext();

            // load id only if possible
            if (!mArg.id.empty() || mArg.verbose == 1)
                loaded = record->loadId(mReader);

            if (!testId(record->getLowerCaseId(), mArg.id, idPattern)) {
                if (!loaded)
                    mReader.skipRecord();
                continue;
            }

            // load if necessary
            if (!loaded && loadRequire) {
                mReader.restoreContext(context);
                record->load(mReader);
                loaded = true;
            }else{
                mReader.skipRecord();
            }

            if (!mArg.includeProperty.empty() && !mArg.propertyValue.empty()){
                if(!mArg.propertyValue.compare(
                            Misc::StringUtils::lowerCase(record->getProperty(mArg.includeProperty)))) {
                    continue;
                }
            }

            if (!mArg.includeInventory.empty()) {
                const ESM::InventoryList& il = *record->getInventoryList();

                if( std::find_if(il.mList.begin(), il.mList.end(),
                    [&](const ESM::ContItem& item)->bool{

                        if(Misc::StringUtils::lowerCase(item.mItem.toString()) != mArg.includeInventory)
                            return false;

                        if(!mArg.includeInventoryCount.empty()
                                && !mArg.includeInventoryCount.compare(item.mCount))
                                return false;

                        return true;
                    }) == il.mList.end() ){
                    continue;
                }
            }

            if (!mArg.includeSpell.empty()) {
                if (!record->getSpellList()->exists(mArg.includeSpell)) {
                    continue;
                }
            }

            if (mArg.verbose >= 1) {
                if (!mArg.hideType)
                    mOut << type.toString() << " : ";
                mOut << record->getId() << std::endl;
            }

            if(mArg.verbose >= 2)
                record->print(mOut, mArg.verbose);

            if (type == ESM::REC_CELL && mArg.cellRefVerbose > 0)
                outputCellReferences(static_cast<ESM::Cell&>(record->cast<ESM::Cell>()->get()));

        }

    }catch(const std::exception& e) {
        if(!mArg.quiet)
            mError << "error:\n" << e.what() << std::endl;
    }
}

//--------------------------------------------------------------------
void Command::outputCellReferences(ESM::Cell& cell)
{

    if (mIdTypeMap.empty())
        buildIdMap();

    std::regex idPattern(mArg.cellRefId);

    ESM::CellRef ref;
    try {

        cell.restore(mReader, 0);
        while(getNextCellRefId(ref)) {

            auto iter = mIdTypeMap.find(ref.mRefID);
            if (iter == mIdTypeMap.end())
                throw std::runtime_error("missing " + ref.mRefID);

            // constrain type
            if (!testType(mArg.cellRefTypes, iter->second)) {
                skipCellRefData();
                continue;
            }

            // constrain id, only load data if it's a valid match
            if(!testId(Misc::StringUtils::lowerCase(ref.mRefID), mArg.cellRefId, idPattern)){
                skipCellRefData();
                continue;
            }

            bool deleted = false;
            ref.loadData(mReader, deleted);

            if (mArg.cellRefVerbose == 0) {
               continue;
            }

            if(mArg.cellRefVerbose >= 1){
                if (!mArg.hideType) {
                    mOut << "  " << iter->second.toString() << " : ";
                }
                mOut << ref.mRefID << std::endl;
            }

            if (mArg.cellRefVerbose < 2)
                continue;

            mOut << "    Refnum: "               << ref.mRefNum.mIndex << std::endl;
            mOut << "    ID: '"                  << ref.mRefID << "'\n";
            mOut << "    Owner: '"               << ref.mOwner << "'\n";
            mOut << "    Global: '"              << ref.mGlobalVariable << "'\n";
            mOut << "    Faction: '"             << ref.mFaction << "'\n";
            mOut << "    Faction rank: '"        << ref.mFactionRank << "'\n";
            mOut << "    Enchantment charge: '"  << ref.mEnchantmentCharge << "'\n";
            mOut << "    Uses/health: '"         << ref.mChargeInt << "'\n";
            mOut << "    Gold value: '"          << ref.mGoldValue << "'\n";
            mOut << "    Blocked: '"             << static_cast<int>(ref.mReferenceBlocked) << "'\n";
            mOut << "    Deleted: "              << deleted << std::endl;
            if (!ref.mKey.empty())
                 mOut << "Key: '"            << ref.mKey << "'" << std::endl;
        }

    }catch(const std::exception& e) {
        if(!mArg.quiet)
            mError << "error:\n" << e.what() << std::endl;
    }
}

//--------------------------------------------------------------------
void Command::outputCellIncludeReference()
{

    std::regex idPattern(mArg.id);
    std::regex includeIdPattern(mArg.cellIncludeId);

    if (mIdTypeMap.empty())
       buildIdMap();

    try{
        while(mReader.hasMoreRecs()) {
            ESM::NAME type = mReader.getRecName();
            uint32_t flags;
            mReader.getRecHeader(flags);

            if (type != ESM::REC_CELL) {
                mReader.skipRecord();
                continue;
            }

            ET::RecordBase* record = ET::RecordBase::create(type);
            record->load(mReader);

            if(!testId(record->getLowerCaseId(), mArg.id, idPattern)){
                continue;
            }

            bool found = false;
            ESM::CellRef ref;
            ESM::Cell& cell = record->cast<ESM::Cell>()->get();
            cell.restore(mReader, 0);

            while(getNextCellRefId(ref)) {

                auto iter = mIdTypeMap.find(ref.mRefID);
                if (iter == mIdTypeMap.end())
                    throw std::runtime_error("missing " + ref.mRefID);

                if (!testType(mArg.cellIncludeTypes, iter->second)) {
                    skipCellRefData();
                    continue;
                }

                if (!testId(Misc::StringUtils::lowerCase(ref.mRefID), mArg.cellIncludeId, includeIdPattern)) {
                    skipCellRefData();
                    continue;
                }

                skipCellRefData();
                found = true;
                break;
            }

            mReader.skipRecord();

            if (!found)
                continue;

            if (mArg.verbose >= 1) {
                if (!mArg.hideType)
                    mOut << type.toString() << " : ";
                mOut << record->getId() << std::endl;
            }

            if(mArg.verbose >= 2)
                record->print(mOut, mArg.verbose);

            if (mArg.cellRefVerbose > 0)
                outputCellReferences(cell);

        }
    }catch(const std::exception& e) {
        if(!mArg.quiet)
            mError << "error:\n" << e.what() << std::endl;
    }

}

//--------------------------------------------------------------------
bool Command::open()
{
    try {

        if (mArg.file.empty())
            throw "error:\n    missing es file";

        mEncoder = new ToUTF8::Utf8Encoder (ToUTF8::calculateEncoding(mArg.encoding));
        mReader.setEncoder(mEncoder);
        mReader.open(mArg.file);

        mStartContext = mReader.getContext();

        return true;
    }catch(const std::exception& e) {
        if(!mArg.quiet)
            mError << "error:\n" << e.what() << std::endl;
        return false;
    }
}

//--------------------------------------------------------------------
bool Command::parse(int argc, char** argv)
{
    try {
        namespace bpo = boost::program_options;
        bpo::store(bpo::command_line_parser(argc, argv).options(mDesc).positional(mPod).run(), mVariables);
        bpo::notify(mVariables);

        mArg.types.clear();
        mArg.cellRefTypes.clear();
        mArg.cellIncludeTypes.clear();
        for(const auto& s : mArg.stringTypes)
            mArg.types.push_back(stringToType(Misc::StringUtils::lowerCase(s)));

        for(const auto& s : mArg.stringCellRefTypes)
            mArg.cellRefTypes.push_back(stringToType(Misc::StringUtils::lowerCase(s)));

        for(const auto& s : mArg.stringCellIncludeTypes)
            mArg.cellIncludeTypes.push_back(stringToType(Misc::StringUtils::lowerCase(s)));

        Misc::StringUtils::lowerCaseInPlace(mArg.id);
        Misc::StringUtils::lowerCaseInPlace(mArg.cellRefId);
        Misc::StringUtils::lowerCaseInPlace(mArg.cellIncludeId);
        Misc::StringUtils::lowerCaseInPlace(mArg.listProperty);
        Misc::StringUtils::lowerCaseInPlace(mArg.includeProperty);
        Misc::StringUtils::lowerCaseInPlace(mArg.includeInventory);
        Misc::StringUtils::lowerCaseInPlace(mArg.includeSpell);

        mArg.propertyValue.setRegexCompare(mArg.regex);
        mArg.includeInventoryCount.setRegexCompare(mArg.regex);

        if(mArg.file.empty()){
            char* file = std::getenv("ESM_DATA");
            if (file)
                mArg.file = file;
        }

        return true;
    }
    catch(const boost::exception& e){
        if(!mArg.quiet)
            mError << "error:\n" << diagnostic_information(e) << "'" << std::endl;
        return false;
    }
    catch(const std::exception& e) {
        if(!mArg.quiet)
            mError << "error:\n" << e.what() << "'" << std::endl;
        return false;
    }
}

//--------------------------------------------------------------------
void Command::buildIdMap()
{
    mIdTypeMap.clear();
    auto context = mReader.getContext();

    try {
        mReader.restoreContext(mStartContext);
        while(mReader.hasMoreRecs()) {
            ESM::NAME type = mReader.getRecName();
            uint32_t flags;
            mReader.getRecHeader(flags);

            if (!isReferenceable(type)) {
                mReader.skipRecord();
                continue;
            }

            std::string name = mReader.getHNString("NAME");
            mIdTypeMap[name] = type;
            mReader.skipRecord();
        }

    }catch(const std::exception& e) {
        if(!mArg.quiet)
            mError << "error:\n" << e.what() << std::endl;
    }

    mReader.restoreContext(context);
}

//--------------------------------------------------------------------
bool Command::getNextCellRefId(ESM::CellRef &ref)
{
    if(!mReader.hasMoreSubs())
        return false;

    //@Ques should i check MVRF ?
    if (mReader.peekNextSub("FRMR")) {
        ref.loadId(mReader);
        return true;
    }

    return false;
}

//--------------------------------------------------------------------
void Command::skipCellRefData()
{
    // @Ques should i handle MVRF?
    mReader.skipHSubUntil("FRMR");
}

//--------------------------------------------------------------------
bool Command::testId(const std::string& id, const std::string& text, const std::regex& pattern)
{
    if (text.empty())
        return true;

    return mArg.regex ? std::regex_match(id, pattern) : id == text;
}

//--------------------------------------------------------------------
bool Command::testType(const std::vector<ESM::NAME> types, ESM::NAME type)
{
    if(types.empty())
        return true;

    return std::find(types.begin(), types.end(), type.intval) !=  types.end();
}

//--------------------------------------------------------------------
ESM::NAME Command::stringToNAME(const std::string& s)
{
    if (s.size() != 4) {
        throw std::runtime_error("\"" + s + "\" is not size of 4" );
    }
    ESM::NAME n;
    n.data[0] = s[0];
    n.data[1] = s[1];
    n.data[2] = s[2];
    n.data[3] = s[3];

    return n;
}

//--------------------------------------------------------------------
ESM::NAME Command::stringToType(const std::string& s)
{
    std::string us(s);
    us[0] &= ~0x20;
    us[1] &= ~0x20;
    us[2] &= ~0x20;
    us[3] &= ~0x20;

    auto iter = std::find(sTypes.begin(), sTypes.end(), us);
    if (iter == sTypes.end()) {
        throw std::runtime_error("\"" + s + "\" is not a valid type");
    }
    return *iter;
}

//--------------------------------------------------------------------
ESM::NAME Command::uintToNAME(uint32_t u)
{
    ESM::NAME n;
    n.intval = u;
    return n;
}

//--------------------------------------------------------------------
bool Command::isReferenceable(ESM::NAME type)
{
    uint32_t i = type.intval;
    return
        i == ESM::REC_BOOK ||
        i == ESM::REC_ACTI ||
        i == ESM::REC_ALCH ||
        i == ESM::REC_APPA ||
        i == ESM::REC_ARMO ||
        i == ESM::REC_CLOT ||
        i == ESM::REC_CONT ||
        i == ESM::REC_CREA ||
        i == ESM::REC_DOOR ||
        i == ESM::REC_INGR ||
        i == ESM::REC_LEVC ||
        i == ESM::REC_LIGH ||
        i == ESM::REC_LOCK ||
        i == ESM::REC_MISC ||
        i == ESM::REC_NPC_ ||
        i == ESM::REC_WEAP ||
        i == ESM::REC_PROB ||
        i == ESM::REC_REPA ||
        i == ESM::REC_STAT
    ;
}

//--------------------------------------------------------------------
bool Command::hasInventory(ESM::NAME type)
{
    uint32_t i = type.intval;
    return  i == ESM::REC_CONT || i == ESM::REC_CREA || i == ESM::REC_NPC_;
}

//--------------------------------------------------------------------
bool Command::hasSpell(ESM::NAME type)
{
    uint32_t i = type.intval;
    return  i == ESM::REC_CREA || i == ESM::REC_NPC_ || i == ESM::REC_BSGN;
}

}
