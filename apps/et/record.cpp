#include "record.hpp"
#include "labels.hpp"

#include <iostream>
#include <sstream>

#include <boost/format.hpp>

namespace
{

void printAIPackage(std::ostream& os, ESM::AIPackage p)
{
    os << "  AI Type: " << aiTypeLabel(p.mType)
              << " (" << boost::format("0x%08X") % p.mType << ")" << std::endl;
    if (p.mType == ESM::AI_Wander)
    {
        os << "    Distance: " << p.mWander.mDistance << std::endl;
        os << "    Duration: " << p.mWander.mDuration << std::endl;
        os << "    Time of Day: " << (int)p.mWander.mTimeOfDay << std::endl;
        if (p.mWander.mShouldRepeat != 1)
            os << "    Should repeat: " << (bool)(p.mWander.mShouldRepeat != 0) << std::endl;

        os << "    Idle: ";
        for (int i = 0; i != 8; i++)
            os << (int)p.mWander.mIdle[i] << " ";
        os << std::endl;
    }
    else if (p.mType == ESM::AI_Travel)
    {
        os << "    Travel Coordinates: (" << p.mTravel.mX << ","
                  << p.mTravel.mY << "," << p.mTravel.mZ << ")" << std::endl;
        os << "    Travel Unknown: " << p.mTravel.mUnk << std::endl;
    }
    else if (p.mType == ESM::AI_Follow || p.mType == ESM::AI_Escort)
    {
        os << "    Follow Coordinates: (" << p.mTarget.mX << ","
                  << p.mTarget.mY << "," << p.mTarget.mZ << ")" << std::endl;
        os << "    Duration: " << p.mTarget.mDuration << std::endl;
        os << "    Target ID: " << p.mTarget.mId.toString() << std::endl;
        os << "    Unknown: " << p.mTarget.mUnk << std::endl;
    }
    else if (p.mType == ESM::AI_Activate)
    {
        os << "    Name: " << p.mActivate.mName.toString() << std::endl;
        os << "    Activate Unknown: " << p.mActivate.mUnk << std::endl;
    }
    else {
        os << "    BadPackage: " << boost::format("0x%08x") % p.mType << std::endl;
    }

    if (p.mCellName != "")
        os << "    Cell Name: " << p.mCellName << std::endl;
}

std::string ruleString(ESM::DialInfo::SelectStruct ss)
{
    std::string rule = ss.mSelectRule;

    if (rule.length() < 5)
        return "INVALID";

    char type = rule[1];
    char indicator = rule[2];

    std::string type_str = "INVALID";
    std::string func_str = str(boost::format("INVALID=%s") % rule.substr(1,3));
    int func;
    std::istringstream iss(rule.substr(2,2));
    iss >> func;

    switch(type)
    {
    case '1':
        type_str = "Function";
        func_str = ruleFunction(func);
        break;
    case '2':
        if (indicator == 's') type_str = "Global short";
        else if (indicator == 'l') type_str = "Global long";
        else if (indicator == 'f') type_str = "Global float";
        break;
    case '3':
        if (indicator == 's') type_str = "Local short";
        else if (indicator == 'l') type_str = "Local long";
        else if (indicator == 'f') type_str = "Local float";
        break;
    case '4': if (indicator == 'J') type_str = "Journal"; break;
    case '5': if (indicator == 'I') type_str = "Item type"; break;
    case '6': if (indicator == 'D') type_str = "NPC Dead"; break;
    case '7': if (indicator == 'X') type_str = "Not ID"; break;
    case '8': if (indicator == 'F') type_str = "Not Faction"; break;
    case '9': if (indicator == 'C') type_str = "Not Class"; break;
    case 'A': if (indicator == 'R') type_str = "Not Race"; break;
    case 'B': if (indicator == 'L') type_str = "Not Cell"; break;
    case 'C': if (indicator == 's') type_str = "Not Local"; break;
    default: break;
    }

    // Append the variable name to the function string if any.
    if (type != '1') func_str = rule.substr(5);

    // In the previous switch, we assumed that the second char was X
    // for all types not qual to one.  If this wasn't true, go back to
    // the error message.
    if (type != '1' && rule[3] != 'X')
        func_str = str(boost::format("INVALID=%s") % rule.substr(1,3));

    char oper = rule[4];
    std::string oper_str = "??";
    switch (oper)
    {
    case '0': oper_str = "=="; break;
    case '1': oper_str = "!="; break;
    case '2': oper_str = "> "; break;
    case '3': oper_str = ">="; break;
    case '4': oper_str = "< "; break;
    case '5': oper_str = "<="; break;
    default: break;
    }

    std::ostringstream stream;
    stream << ss.mValue;

    std::string result = str(boost::format("%-12s %-32s %2s %s")
                             % type_str % func_str % oper_str % stream.str());
    return result;
}

void printEffectList(std::ostream& os, ESM::EffectList effects)
{
    int i = 0;
    std::vector<ESM::ENAMstruct>::iterator eit;
    for (eit = effects.mList.begin(); eit != effects.mList.end(); ++eit)
    {
        os << "  Effect[" << i << "]: " << magicEffectLabel(eit->mEffectID)
                  << " (" << eit->mEffectID << ")" << std::endl;
        if (eit->mSkill != -1)
            os << "    Skill: " << skillLabel(eit->mSkill)
                      << " (" << (int)eit->mSkill << ")" << std::endl;
        if (eit->mAttribute != -1)
            os << "    Attribute: " << attributeLabel(eit->mAttribute)
                      << " (" << (int)eit->mAttribute << ")" << std::endl;
        os << "    Range: " << rangeTypeLabel(eit->mRange)
                  << " (" << eit->mRange << ")" << std::endl;
        // Area is always zero if range type is "Self"
        if (eit->mRange != ESM::RT_Self)
            os << "    Area: " << eit->mArea << std::endl;
        os << "    Duration: " << eit->mDuration << std::endl;
        os << "    Magnitude: " << eit->mMagnMin << "-" << eit->mMagnMax << std::endl;
        i++;
    }
}

void printTransport(std::ostream& os, const std::vector<ESM::Transport::Dest>& transport)
{
    std::vector<ESM::Transport::Dest>::const_iterator dit;
    for (dit = transport.begin(); dit != transport.end(); ++dit)
    {
        os << "  Destination Position: "
                  << boost::format("%12.3f") % dit->mPos.pos[0] << ","
                  << boost::format("%12.3f") % dit->mPos.pos[1] << ","
                  << boost::format("%12.3f") % dit->mPos.pos[2] << ")" << std::endl;
        os << "  Destination Rotation: "
                  << boost::format("%9.6f") % dit->mPos.rot[0] << ","
                  << boost::format("%9.6f") % dit->mPos.rot[1] << ","
                  << boost::format("%9.6f") % dit->mPos.rot[2] << ")" << std::endl;
        if (dit->mCellName != "")
            os << "  Destination Cell: " << dit->mCellName << std::endl;
    }
}

}

namespace ET {

//--------------------------------------------------------------------
std::string RecordBase::getLowerCaseId()
{
    return Misc::StringUtils::lowerCase(getId());
}

RecordBase *
RecordBase::create(ESM::NAME type)
{
    RecordBase *record = 0;

    switch (type.intval) {
    case ESM::REC_ACTI:
    {
        record = new ET::Record<ESM::Activator>;
        break;
    }
    case ESM::REC_ALCH:
    {
        record = new ET::Record<ESM::Potion>;
        break;
    }
    case ESM::REC_APPA:
    {
        record = new ET::Record<ESM::Apparatus>;
        break;
    }
    case ESM::REC_ARMO:
    {
        record = new ET::Record<ESM::Armor>;
        break;
    }
    case ESM::REC_BODY:
    {
        record = new ET::Record<ESM::BodyPart>;
        break;
    }
    case ESM::REC_BOOK:
    {
        record = new ET::Record<ESM::Book>;
        break;
    }
    case ESM::REC_BSGN:
    {
        record = new ET::Record<ESM::BirthSign>;
        break;
    }
    case ESM::REC_CELL:
    {
        record = new ET::Record<ESM::Cell>;
        break;
    }
    case ESM::REC_CLAS:
    {
        record = new ET::Record<ESM::Class>;
        break;
    }
    case ESM::REC_CLOT:
    {
        record = new ET::Record<ESM::Clothing>;
        break;
    }
    case ESM::REC_CONT:
    {
        record = new ET::Record<ESM::Container>;
        break;
    }
    case ESM::REC_CREA:
    {
        record = new ET::Record<ESM::Creature>;
        break;
    }
    case ESM::REC_DIAL:
    {
        record = new ET::Record<ESM::Dialogue>;
        break;
    }
    case ESM::REC_DOOR:
    {
        record = new ET::Record<ESM::Door>;
        break;
    }
    case ESM::REC_ENCH:
    {
        record = new ET::Record<ESM::Enchantment>;
        break;
    }
    case ESM::REC_FACT:
    {
        record = new ET::Record<ESM::Faction>;
        break;
    }
    case ESM::REC_GLOB:
    {
        record = new ET::Record<ESM::Global>;
        break;
    }
    case ESM::REC_GMST:
    {
        record = new ET::Record<ESM::GameSetting>;
        break;
    }
    case ESM::REC_INFO:
    {
        record = new ET::Record<ESM::DialInfo>;
        break;
    }
    case ESM::REC_INGR:
    {
        record = new ET::Record<ESM::Ingredient>;
        break;
    }
    case ESM::REC_LAND:
    {
        record = new ET::Record<ESM::Land>;
        break;
    }
    case ESM::REC_LEVI:
    {
        record = new ET::Record<ESM::ItemLevList>;
        break;
    }
    case ESM::REC_LEVC:
    {
        record = new ET::Record<ESM::CreatureLevList>;
        break;
    }
    case ESM::REC_LIGH:
    {
        record = new ET::Record<ESM::Light>;
        break;
    }
    case ESM::REC_LOCK:
    {
        record = new ET::Record<ESM::Lockpick>;
        break;
    }
    case ESM::REC_LTEX:
    {
        record = new ET::Record<ESM::LandTexture>;
        break;
    }
    case ESM::REC_MISC:
    {
        record = new ET::Record<ESM::Miscellaneous>;
        break;
    }
    case ESM::REC_MGEF:
    {
        record = new ET::Record<ESM::MagicEffect>;
        break;
    }
    case ESM::REC_NPC_:
    {
        record = new ET::Record<ESM::NPC>;
        break;
    }
    case ESM::REC_PGRD:
    {
        record = new ET::Record<ESM::Pathgrid>;
        break;
    }
    case ESM::REC_PROB:
    {
        record = new ET::Record<ESM::Probe>;
        break;
    }
    case ESM::REC_RACE:
    {
        record = new ET::Record<ESM::Race>;
        break;
    }
    case ESM::REC_REGN:
    {
        record = new ET::Record<ESM::Region>;
        break;
    }
    case ESM::REC_REPA:
    {
        record = new ET::Record<ESM::Repair>;
        break;
    }
    case ESM::REC_SCPT:
    {
        record = new ET::Record<ESM::Script>;
        break;
    }
    case ESM::REC_SKIL:
    {
        record = new ET::Record<ESM::Skill>;
        break;
    }
    case ESM::REC_SNDG:
    {
        record = new ET::Record<ESM::SoundGenerator>;
        break;
    }
    case ESM::REC_SOUN:
    {
        record = new ET::Record<ESM::Sound>;
        break;
    }
    case ESM::REC_SPEL:
    {
        record = new ET::Record<ESM::Spell>;
        break;
    }
    case ESM::REC_STAT:
    {
        record = new ET::Record<ESM::Static>;
        break;
    }
    case ESM::REC_WEAP:
    {
        record = new ET::Record<ESM::Weapon>;
        break;
    }
    case ESM::REC_SSCR:
    {
        record = new ET::Record<ESM::StartScript>;
        break;
    }
    default:
        record = 0;
    }
    if (record) {
        record->mType = type;
    }
    return record;
}

template<>
void Record<ESM::Activator>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    os << "  Script: " << mData.mScript << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Potion>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    os << "  Icon: " << mData.mIcon << std::endl;
    if (mData.mScript != "")
        os << "  Script: " << mData.mScript << std::endl;
    os << "  Weight: " << mData.mData.mWeight << std::endl;
    os << "  Value: " << mData.mData.mValue << std::endl;
    os << "  AutoCalc: " << mData.mData.mAutoCalc << std::endl;
    printEffectList(os, mData.mEffects);
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Armor>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    os << "  Icon: " << mData.mIcon << std::endl;
    if (mData.mScript != "")
        os << "  Script: " << mData.mScript << std::endl;
    if (mData.mEnchant != "")
        os << "  Enchantment: " << mData.mEnchant << std::endl;
    os << "  Type: " << armorTypeLabel(mData.mData.mType)
              << " (" << mData.mData.mType << ")" << std::endl;
    os << "  Weight: " << mData.mData.mWeight << std::endl;
    os << "  Value: " << mData.mData.mValue << std::endl;
    os << "  Health: " << mData.mData.mHealth << std::endl;
    os << "  Armor: " << mData.mData.mArmor << std::endl;
    os << "  Enchantment Points: " << mData.mData.mEnchant << std::endl;
    std::vector<ESM::PartReference>::iterator pit;
    for (pit = mData.mParts.mParts.begin(); pit != mData.mParts.mParts.end(); ++pit)
    {
        os << "  Body Part: " << bodyPartLabel(pit->mPart)
                  << " (" << (int)(pit->mPart) << ")" << std::endl;
        os << "    Male Name: " << pit->mMale << std::endl;
        if (pit->mFemale != "")
            os << "    Female Name: " << pit->mFemale << std::endl;
    }
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Apparatus>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    os << "  Icon: " << mData.mIcon << std::endl;
    os << "  Script: " << mData.mScript << std::endl;
    os << "  Type: " << apparatusTypeLabel(mData.mData.mType)
              << " (" << mData.mData.mType << ")" << std::endl;
    os << "  Weight: " << mData.mData.mWeight << std::endl;
    os << "  Value: " << mData.mData.mValue << std::endl;
    os << "  Quality: " << mData.mData.mQuality << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::BodyPart>::print(std::ostream& os, int verbose)
{
    os << "  Race: " << mData.mRace << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    os << "  Type: " << meshTypeLabel(mData.mData.mType)
              << " (" << (int)mData.mData.mType << ")" << std::endl;
    os << "  Flags: " << bodyPartFlags(mData.mData.mFlags) << std::endl;
    os << "  Part: " << meshPartLabel(mData.mData.mPart)
              << " (" << (int)mData.mData.mPart << ")" << std::endl;
    os << "  Vampire: " << (int)mData.mData.mVampire << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Book>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    os << "  Icon: " << mData.mIcon << std::endl;
    if (mData.mScript != "")
        os << "  Script: " << mData.mScript << std::endl;
    if (mData.mEnchant != "")
        os << "  Enchantment: " << mData.mEnchant << std::endl;
    os << "  Weight: " << mData.mData.mWeight << std::endl;
    os << "  Value: " << mData.mData.mValue << std::endl;
    os << "  IsScroll: " << mData.mData.mIsScroll << std::endl;
    os << "  SkillId: " << mData.mData.mSkillId << std::endl;
    os << "  Enchantment Points: " << mData.mData.mEnchant << std::endl;
    if (verbose >= 3)
    {
        os << "  Text:" << std::endl;
        os << "START--------------------------------------" << std::endl;
        os << mData.mText << std::endl;
        os << "END----------------------------------------" << std::endl;
    }
    else
    {
        os << "  Text: [skipped]" << std::endl;
    }
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::BirthSign>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Texture: " << mData.mTexture << std::endl;
    os << "  Description: " << mData.mDescription << std::endl;
    std::vector<std::string>::iterator pit;
    for (pit = mData.mPowers.mList.begin(); pit != mData.mPowers.mList.end(); ++pit)
        os << "  Power: " << *pit << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Cell>::print(std::ostream& os, int verbose)
{
    // None of the cells have names...
    if (mData.mName != "")
        os << "  Name: " << mData.mName << std::endl;
    if (mData.mRegion != "")
        os << "  Region: " << mData.mRegion << std::endl;
    os << "  Flags: " << cellFlags(mData.mData.mFlags) << std::endl;

    os << "  Coordinates: " << " (" << mData.getGridX() << ","
              << mData.getGridY() << ")" << std::endl;

    if (mData.mData.mFlags & ESM::Cell::Interior &&
        !(mData.mData.mFlags & ESM::Cell::QuasiEx))
    {
        os << "  Ambient Light Color: " << mData.mAmbi.mAmbient << std::endl;
        os << "  Sunlight Color: " << mData.mAmbi.mSunlight << std::endl;
        os << "  Fog Color: " << mData.mAmbi.mFog << std::endl;
        os << "  Fog Density: " << mData.mAmbi.mFogDensity << std::endl;
        os << "  Water Level: " << mData.mWater << std::endl;
    }
    else
        os << "  Map Color: " << boost::format("0x%08X") % mData.mMapColor << std::endl;
    os << "  Water Level Int: " << mData.mWaterInt << std::endl;
    os << "  RefId counter: " << mData.mRefNumCounter << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;

}

template<>
void Record<ESM::Class>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Description: " << mData.mDescription << std::endl;
    os << "  Playable: " << mData.mData.mIsPlayable << std::endl;
    os << "  AutoCalc: " << mData.mData.mCalc << std::endl;
    os << "  Attribute1: " << attributeLabel(mData.mData.mAttribute[0])
              << " (" << mData.mData.mAttribute[0] << ")" << std::endl;
    os << "  Attribute2: " << attributeLabel(mData.mData.mAttribute[1])
              << " (" << mData.mData.mAttribute[1] << ")" << std::endl;
    os << "  Specialization: " << specializationLabel(mData.mData.mSpecialization)
              << " (" << mData.mData.mSpecialization << ")" << std::endl;
    for (int i = 0; i != 5; i++)
        os << "  Minor Skill: " << skillLabel(mData.mData.mSkills[i][0])
                  << " (" << mData.mData.mSkills[i][0] << ")" << std::endl;
    for (int i = 0; i != 5; i++)
        os << "  Major Skill: " << skillLabel(mData.mData.mSkills[i][1])
                  << " (" << mData.mData.mSkills[i][1] << ")" << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Clothing>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    os << "  Icon: " << mData.mIcon << std::endl;
    if (mData.mScript != "")
        os << "  Script: " << mData.mScript << std::endl;
    if (mData.mEnchant != "")
        os << "  Enchantment: " << mData.mEnchant << std::endl;
    os << "  Type: " << clothingTypeLabel(mData.mData.mType)
              << " (" << mData.mData.mType << ")" << std::endl;
    os << "  Weight: " << mData.mData.mWeight << std::endl;
    os << "  Value: " << mData.mData.mValue << std::endl;
    os << "  Enchantment Points: " << mData.mData.mEnchant << std::endl;
    std::vector<ESM::PartReference>::iterator pit;
    for (pit = mData.mParts.mParts.begin(); pit != mData.mParts.mParts.end(); ++pit)
    {
        os << "  Body Part: " << bodyPartLabel(pit->mPart)
                  << " (" << (int)(pit->mPart) << ")" << std::endl;
        os << "    Male Name: " << pit->mMale << std::endl;
        if (pit->mFemale != "")
            os << "    Female Name: " << pit->mFemale << std::endl;
    }
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Container>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    if (mData.mScript != "")
        os << "  Script: " << mData.mScript << std::endl;
    os << "  Flags: " << containerFlags(mData.mFlags) << std::endl;
    os << "  Weight: " << mData.mWeight << std::endl;
    std::vector<ESM::ContItem>::iterator cit;
    for (cit = mData.mInventory.mList.begin(); cit != mData.mInventory.mList.end(); ++cit)
        os << "  Inventory: Count: " << boost::format("%4d") % cit->mCount
                  << " Item: " << cit->mItem.toString() << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Creature>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    os << "  Script: " << mData.mScript << std::endl;
    os << "  Flags: " << creatureFlags(mData.mFlags) << std::endl;
    os << "  Original: " << mData.mOriginal << std::endl;
    os << "  Scale: " << mData.mScale << std::endl;

    os << "  Type: " << creatureTypeLabel(mData.mData.mType)
              << " (" << mData.mData.mType << ")" << std::endl;
    os << "  Level: " << mData.mData.mLevel << std::endl;

    os << "  Attributes:" << std::endl;
    os << "    Strength: " << mData.mData.mStrength << std::endl;
    os << "    Intelligence: " << mData.mData.mIntelligence << std::endl;
    os << "    Willpower: " << mData.mData.mWillpower << std::endl;
    os << "    Agility: " << mData.mData.mAgility << std::endl;
    os << "    Speed: " << mData.mData.mSpeed << std::endl;
    os << "    Endurance: " << mData.mData.mEndurance << std::endl;
    os << "    Personality: " << mData.mData.mPersonality << std::endl;
    os << "    Luck: " << mData.mData.mLuck << std::endl;

    os << "  Health: " << mData.mData.mHealth << std::endl;
    os << "  Magicka: " << mData.mData.mMana << std::endl;
    os << "  Fatigue: " << mData.mData.mFatigue << std::endl;
    os << "  Soul: " << mData.mData.mSoul << std::endl;
    os << "  Combat: " << mData.mData.mCombat << std::endl;
    os << "  Magic: " << mData.mData.mMagic << std::endl;
    os << "  Stealth: " << mData.mData.mStealth << std::endl;
    os << "  Attack1: " << mData.mData.mAttack[0]
              << "-" <<  mData.mData.mAttack[1] << std::endl;
    os << "  Attack2: " << mData.mData.mAttack[2]
              << "-" <<  mData.mData.mAttack[3] << std::endl;
    os << "  Attack3: " << mData.mData.mAttack[4]
              << "-" <<  mData.mData.mAttack[5] << std::endl;
    os << "  Gold: " << mData.mData.mGold << std::endl;

    std::vector<ESM::ContItem>::iterator cit;
    for (cit = mData.mInventory.mList.begin(); cit != mData.mInventory.mList.end(); ++cit)
        os << "  Inventory: Count: " << boost::format("%4d") % cit->mCount
                  << " Item: " << cit->mItem.toString() << std::endl;

    std::vector<std::string>::iterator sit;
    for (sit = mData.mSpells.mList.begin(); sit != mData.mSpells.mList.end(); ++sit)
        os << "  Spell: " << *sit << std::endl;

    printTransport(os, mData.getTransport());

    os << "  Artifical Intelligence: " << mData.mHasAI << std::endl;
    os << "    AI Hello:" << (int)mData.mAiData.mHello << std::endl;
    os << "    AI Fight:" << (int)mData.mAiData.mFight << std::endl;
    os << "    AI Flee:" << (int)mData.mAiData.mFlee << std::endl;
    os << "    AI Alarm:" << (int)mData.mAiData.mAlarm << std::endl;
    os << "    AI U1:" << (int)mData.mAiData.mU1 << std::endl;
    os << "    AI U2:" << (int)mData.mAiData.mU2 << std::endl;
    os << "    AI U3:" << (int)mData.mAiData.mU3 << std::endl;
    os << "    AI U4:" << (int)mData.mAiData.mU4 << std::endl;
    os << "    AI Services:" << boost::format("0x%08X") % mData.mAiData.mServices << std::endl;

    std::vector<ESM::AIPackage>::iterator pit;
    for (pit = mData.mAiPackage.mList.begin(); pit != mData.mAiPackage.mList.end(); ++pit)
        printAIPackage(os, *pit);
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Dialogue>::print(std::ostream& os, int verbose)
{
    os << "  Type: " << dialogTypeLabel(mData.mType)
              << " (" << (int)mData.mType << ")" << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
    // Sadly, there are no DialInfos, because the loader dumps as it
    // loads, rather than loading and then dumping. :-( Anyone mind if
    // I change this?
    ESM::Dialogue::InfoContainer::iterator iit;
    for (iit = mData.mInfo.begin(); iit != mData.mInfo.end(); iit++)
        os << "INFO!" << iit->mId << std::endl;
}

template<>
void Record<ESM::Door>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    os << "  Script: " << mData.mScript << std::endl;
    os << "  OpenSound: " << mData.mOpenSound << std::endl;
    os << "  CloseSound: " << mData.mCloseSound << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Enchantment>::print(std::ostream& os, int verbose)
{
    os << "  Type: " << enchantTypeLabel(mData.mData.mType)
              << " (" << mData.mData.mType << ")" << std::endl;
    os << "  Cost: " << mData.mData.mCost << std::endl;
    os << "  Charge: " << mData.mData.mCharge << std::endl;
    os << "  AutoCalc: " << mData.mData.mAutocalc << std::endl;
    printEffectList(os, mData.mEffects);
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Faction>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Hidden: " << mData.mData.mIsHidden << std::endl;
    os << "  Attribute1: " << attributeLabel(mData.mData.mAttribute[0])
              << " (" << mData.mData.mAttribute[0] << ")" << std::endl;
    os << "  Attribute2: " << attributeLabel(mData.mData.mAttribute[1])
              << " (" << mData.mData.mAttribute[1] << ")" << std::endl;
    for (int i = 0; i < 7; i++)
        if (mData.mData.mSkills[i] != -1)
            os << "  Skill: " << skillLabel(mData.mData.mSkills[i])
                      << " (" << mData.mData.mSkills[i] << ")" << std::endl;
    for (int i = 0; i != 10; i++)
        if (mData.mRanks[i] != "")
        {
            os << "  Rank: " << mData.mRanks[i] << std::endl;
            os << "    Attribute1 Requirement: "
                      << mData.mData.mRankData[i].mAttribute1 << std::endl;
            os << "    Attribute2 Requirement: "
                      << mData.mData.mRankData[i].mAttribute2 << std::endl;
            os << "    One Skill at Level: "
                      << mData.mData.mRankData[i].mSkill1 << std::endl;
            os << "    Two Skills at Level: "
                      << mData.mData.mRankData[i].mSkill2 << std::endl;
            os << "    Faction Reaction: "
                      << mData.mData.mRankData[i].mFactReaction << std::endl;
        }
    std::map<std::string, int>::iterator rit;
    for (rit = mData.mReactions.begin(); rit != mData.mReactions.end(); ++rit)
        os << "  Reaction: " << rit->second << " = " << rit->first << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Global>::print(std::ostream& os, int verbose)
{
    os << "  " << mData.mValue << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::GameSetting>::print(std::ostream& os, int verbose)
{
    os << "  " << mData.mValue << std::endl;
}

template<>
void Record<ESM::DialInfo>::print(std::ostream& os, int verbose)
{
    os << "  Id: " << mData.mId << std::endl;
    if (mData.mPrev != "")
        os << "  Previous ID: " << mData.mPrev << std::endl;
    if (mData.mNext != "")
        os << "  Next ID: " << mData.mNext << std::endl;
    os << "  Text: " << mData.mResponse << std::endl;
    if (mData.mActor != "")
        os << "  Actor: " << mData.mActor << std::endl;
    if (mData.mRace != "")
        os << "  Race: " << mData.mRace << std::endl;
    if (mData.mClass != "")
        os << "  Class: " << mData.mClass << std::endl;
    os << "  Factionless: " << mData.mFactionLess << std::endl;
    if (mData.mFaction != "")
        os << "  NPC Faction: " << mData.mFaction << std::endl;
    if (mData.mData.mRank != -1)
        os << "  NPC Rank: " << (int)mData.mData.mRank << std::endl;
    if (mData.mPcFaction != "")
        os << "  PC Faction: " << mData.mPcFaction << std::endl;
    // CHANGE? non-standard capitalization mPCrank -> mPCRank (mPcRank?)
    if (mData.mData.mPCrank != -1)
        os << "  PC Rank: " << (int)mData.mData.mPCrank << std::endl;
    if (mData.mCell != "")
        os << "  Cell: " << mData.mCell << std::endl;
    if (mData.mData.mDisposition > 0)
        os << "  Disposition/Journal index: " << mData.mData.mDisposition << std::endl;
    if (mData.mData.mGender != ESM::DialInfo::NA)
        os << "  Gender: " << mData.mData.mGender << std::endl;
    if (mData.mSound != "")
        os << "  Sound File: " << mData.mSound << std::endl;

    os << "  Quest Status: " << questStatusLabel(mData.mQuestStatus)
              << " (" << mData.mQuestStatus << ")" << std::endl;
    os << "  Unknown1: " << mData.mData.mUnknown1 << std::endl;
    os << "  Unknown2: " << (int)mData.mData.mUnknown2 << std::endl;

    std::vector<ESM::DialInfo::SelectStruct>::iterator sit;
    for (sit = mData.mSelects.begin(); sit != mData.mSelects.end(); ++sit)
        os << "  Select Rule: " << ruleString(*sit) << std::endl;

    if (mData.mResultScript != "")
    {
        if (verbose >= 3)
        {
            os << "  Result Script:" << std::endl;
            os << "START--------------------------------------" << std::endl;
            os << mData.mResultScript << std::endl;
            os << "END----------------------------------------" << std::endl;
        }
        else
        {
            os << "  Result Script: [skipped]" << std::endl;
        }
    }
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Ingredient>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    os << "  Icon: " << mData.mIcon << std::endl;
    if (mData.mScript != "")
        os << "  Script: " << mData.mScript << std::endl;
    os << "  Weight: " << mData.mData.mWeight << std::endl;
    os << "  Value: " << mData.mData.mValue << std::endl;
    for (int i = 0; i !=4; i++)
    {
        // A value of -1 means no effect
        if (mData.mData.mEffectID[i] == -1) continue;
        os << "  Effect: " << magicEffectLabel(mData.mData.mEffectID[i])
                  << " (" << mData.mData.mEffectID[i] << ")" << std::endl;
        os << "  Skill: " << skillLabel(mData.mData.mSkills[i])
                  << " (" << mData.mData.mSkills[i] << ")" << std::endl;
        os << "  Attribute: " << attributeLabel(mData.mData.mAttributes[i])
                  << " (" << mData.mData.mAttributes[i] << ")" << std::endl;
    }
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Land>::print(std::ostream& os, int verbose)
{
    os << "  Coordinates: (" << mData.mX << "," << mData.mY << ")" << std::endl;
    os << "  Flags: " << landFlags(mData.mFlags) << std::endl;
    os << "  DataTypes: " << mData.mDataTypes << std::endl;

    if (const ESM::Land::LandData *data = mData.getLandData (mData.mDataTypes))
    {
        os << "  Height Offset: " << data->mHeightOffset << std::endl;
        // Lots of missing members.
        os << "  Unknown1: " << data->mUnk1 << std::endl;
        os << "  Unknown2: " << data->mUnk2 << std::endl;
    }
    mData.unloadData();
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::CreatureLevList>::print(std::ostream& os, int verbose)
{
    os << "  Chance for None: " << (int)mData.mChanceNone << std::endl;
    os << "  Flags: " << creatureListFlags(mData.mFlags) << std::endl;
    os << "  Number of items: " << mData.mList.size() << std::endl;
    std::vector<ESM::LevelledListBase::LevelItem>::iterator iit;
    for (iit = mData.mList.begin(); iit != mData.mList.end(); ++iit)
        os << "  Creature: Level: " << iit->mLevel
                  << " Creature: " << iit->mId << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::ItemLevList>::print(std::ostream& os, int verbose)
{
    os << "  Chance for None: " << (int)mData.mChanceNone << std::endl;
    os << "  Flags: " << itemListFlags(mData.mFlags) << std::endl;
    os << "  Number of items: " << mData.mList.size() << std::endl;
    std::vector<ESM::LevelledListBase::LevelItem>::iterator iit;
    for (iit = mData.mList.begin(); iit != mData.mList.end(); ++iit)
        os << "  Inventory: Level: " << iit->mLevel
                  << " Item: " << iit->mId << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Light>::print(std::ostream& os, int verbose)
{
    if (mData.mName != "")
        os << "  Name: " << mData.mName << std::endl;
    if (mData.mModel != "")
        os << "  Model: " << mData.mModel << std::endl;
    if (mData.mIcon != "")
        os << "  Icon: " << mData.mIcon << std::endl;
    if (mData.mScript != "")
        os << "  Script: " << mData.mScript << std::endl;
    os << "  Flags: " << lightFlags(mData.mData.mFlags) << std::endl;
    os << "  Weight: " << mData.mData.mWeight << std::endl;
    os << "  Value: " << mData.mData.mValue << std::endl;
    os << "  Sound: " << mData.mSound << std::endl;
    os << "  Duration: " << mData.mData.mTime << std::endl;
    os << "  Radius: " << mData.mData.mRadius << std::endl;
    os << "  Color: " << mData.mData.mColor << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Lockpick>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    os << "  Icon: " << mData.mIcon << std::endl;
    if (mData.mScript != "")
        os << "  Script: " << mData.mScript << std::endl;
    os << "  Weight: " << mData.mData.mWeight << std::endl;
    os << "  Value: " << mData.mData.mValue << std::endl;
    os << "  Quality: " << mData.mData.mQuality << std::endl;
    os << "  Uses: " << mData.mData.mUses << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Probe>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    os << "  Icon: " << mData.mIcon << std::endl;
    if (mData.mScript != "")
        os << "  Script: " << mData.mScript << std::endl;
    os << "  Weight: " << mData.mData.mWeight << std::endl;
    os << "  Value: " << mData.mData.mValue << std::endl;
    os << "  Quality: " << mData.mData.mQuality << std::endl;
    os << "  Uses: " << mData.mData.mUses << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Repair>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    os << "  Icon: " << mData.mIcon << std::endl;
    if (mData.mScript != "")
        os << "  Script: " << mData.mScript << std::endl;
    os << "  Weight: " << mData.mData.mWeight << std::endl;
    os << "  Value: " << mData.mData.mValue << std::endl;
    os << "  Quality: " << mData.mData.mQuality << std::endl;
    os << "  Uses: " << mData.mData.mUses << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::LandTexture>::print(std::ostream& os, int verbose)
{
    os << "  Id: " << mData.mId << std::endl;
    os << "  Index: " << mData.mIndex << std::endl;
    os << "  Texture: " << mData.mTexture << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::MagicEffect>::print(std::ostream& os, int verbose)
{
    os << "  Index: " << magicEffectLabel(mData.mIndex)
              << " (" << mData.mIndex << ")" << std::endl;
    os << "  Description: " << mData.mDescription << std::endl;
    os << "  Icon: " << mData.mIcon << std::endl;
    os << "  Flags: " << magicEffectFlags(mData.mData.mFlags) << std::endl;
    os << "  Particle Texture: " << mData.mParticle << std::endl;
    if (mData.mCasting != "")
        os << "  Casting Static: " << mData.mCasting << std::endl;
    if (mData.mCastSound != "")
        os << "  Casting Sound: " << mData.mCastSound << std::endl;
    if (mData.mBolt != "")
        os << "  Bolt Static: " << mData.mBolt << std::endl;
    if (mData.mBoltSound != "")
        os << "  Bolt Sound: " << mData.mBoltSound << std::endl;
    if (mData.mHit != "")
        os << "  Hit Static: " << mData.mHit << std::endl;
    if (mData.mHitSound != "")
        os << "  Hit Sound: " << mData.mHitSound << std::endl;
    if (mData.mArea != "")
        os << "  Area Static: " << mData.mArea << std::endl;
    if (mData.mAreaSound != "")
        os << "  Area Sound: " << mData.mAreaSound << std::endl;
    os << "  School: " << schoolLabel(mData.mData.mSchool)
              << " (" << mData.mData.mSchool << ")" << std::endl;
    os << "  Base Cost: " << mData.mData.mBaseCost << std::endl;
    os << "  Unknown 1: " << mData.mData.mUnknown1 << std::endl;
    os << "  Speed: " << mData.mData.mSpeed << std::endl;
    os << "  Unknown 2: " << mData.mData.mUnknown2 << std::endl;
    os << "  RGB Color: " << "("
              << mData.mData.mRed << ","
              << mData.mData.mGreen << ","
              << mData.mData.mBlue << ")" << std::endl;
}

template<>
void Record<ESM::Miscellaneous>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    os << "  Icon: " << mData.mIcon << std::endl;
    if (mData.mScript != "")
        os << "  Script: " << mData.mScript << std::endl;
    os << "  Weight: " << mData.mData.mWeight << std::endl;
    os << "  Value: " << mData.mData.mValue << std::endl;
    os << "  Is Key: " << mData.mData.mIsKey << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::NPC>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Animation: " << mData.mModel << std::endl;
    os << "  Hair Model: " << mData.mHair << std::endl;
    os << "  Head Model: " << mData.mHead << std::endl;
    os << "  Race: " << mData.mRace << std::endl;
    os << "  Class: " << mData.mClass << std::endl;
    if (mData.mScript != "")
        os << "  Script: " << mData.mScript << std::endl;
    if (mData.mFaction != "")
        os << "  Faction: " << mData.mFaction << std::endl;
    os << "  Flags: " << npcFlags(mData.mFlags) << std::endl;

    if (mData.mNpdtType == ESM::NPC::NPC_WITH_AUTOCALCULATED_STATS)
    {
        os << "  Level: " << mData.mNpdt12.mLevel << std::endl;
        os << "  Reputation: " << (int)mData.mNpdt12.mReputation << std::endl;
        os << "  Disposition: " << (int)mData.mNpdt12.mDisposition << std::endl;
        os << "  Rank: " << (int)mData.mNpdt12.mRank << std::endl;
        os << "  Unknown1: "
                  << (unsigned int)((unsigned char)mData.mNpdt12.mUnknown1) << std::endl;
        os << "  Unknown2: "
                  << (unsigned int)((unsigned char)mData.mNpdt12.mUnknown2) << std::endl;
        os << "  Unknown3: "
                  << (unsigned int)((unsigned char)mData.mNpdt12.mUnknown3) << std::endl;
        os << "  Gold: " << mData.mNpdt12.mGold << std::endl;
    }
    else {
        os << "  Level: " << mData.mNpdt52.mLevel << std::endl;
        os << "  Reputation: " << (int)mData.mNpdt52.mReputation << std::endl;
        os << "  Disposition: " << (int)mData.mNpdt52.mDisposition << std::endl;
        os << "  Rank: " << (int)mData.mNpdt52.mRank << std::endl;
        os << "  FactionID: " << (int)mData.mNpdt52.mFactionID << std::endl;

        os << "  Attributes:" << std::endl;
        os << "    Strength: " << (int)mData.mNpdt52.mStrength << std::endl;
        os << "    Intelligence: " << (int)mData.mNpdt52.mIntelligence << std::endl;
        os << "    Willpower: " << (int)mData.mNpdt52.mWillpower << std::endl;
        os << "    Agility: " << (int)mData.mNpdt52.mAgility << std::endl;
        os << "    Speed: " << (int)mData.mNpdt52.mSpeed << std::endl;
        os << "    Endurance: " << (int)mData.mNpdt52.mEndurance << std::endl;
        os << "    Personality: " << (int)mData.mNpdt52.mPersonality << std::endl;
        os << "    Luck: " << (int)mData.mNpdt52.mLuck << std::endl;

        os << "  Skills:" << std::endl;
        for (int i = 0; i != ESM::Skill::Length; i++)
            os << "    " << skillLabel(i) << ": "
                      << (int)(mData.mNpdt52.mSkills[i]) << std::endl;

        os << "  Health: " << mData.mNpdt52.mHealth << std::endl;
        os << "  Magicka: " << mData.mNpdt52.mMana << std::endl;
        os << "  Fatigue: " << mData.mNpdt52.mFatigue << std::endl;
        os << "  Unknown: " << (int)mData.mNpdt52.mUnknown << std::endl;
        os << "  Gold: " << mData.mNpdt52.mGold << std::endl;
    }

    std::vector<ESM::ContItem>::iterator cit;
    for (cit = mData.mInventory.mList.begin(); cit != mData.mInventory.mList.end(); ++cit)
        os << "  Inventory: Count: " << boost::format("%4d") % cit->mCount
                  << " Item: " << cit->mItem.toString() << std::endl;

    std::vector<std::string>::iterator sit;
    for (sit = mData.mSpells.mList.begin(); sit != mData.mSpells.mList.end(); ++sit)
        os << "  Spell: " << *sit << std::endl;

    printTransport(os, mData.getTransport());

    os << "  Artifical Intelligence: " << mData.mHasAI << std::endl;
    os << "    AI Hello:" << (int)mData.mAiData.mHello << std::endl;
    os << "    AI Fight:" << (int)mData.mAiData.mFight << std::endl;
    os << "    AI Flee:" << (int)mData.mAiData.mFlee << std::endl;
    os << "    AI Alarm:" << (int)mData.mAiData.mAlarm << std::endl;
    os << "    AI U1:" << (int)mData.mAiData.mU1 << std::endl;
    os << "    AI U2:" << (int)mData.mAiData.mU2 << std::endl;
    os << "    AI U3:" << (int)mData.mAiData.mU3 << std::endl;
    os << "    AI U4:" << (int)mData.mAiData.mU4 << std::endl;
    os << "    AI Services:" << boost::format("0x%08X") % mData.mAiData.mServices << std::endl;

    std::vector<ESM::AIPackage>::iterator pit;
    for (pit = mData.mAiPackage.mList.begin(); pit != mData.mAiPackage.mList.end(); ++pit)
        printAIPackage(os, *pit);

    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Pathgrid>::print(std::ostream& os, int verbose)
{
    os << "  Cell: " << mData.mCell << std::endl;
    os << "  Coordinates: (" << mData.mData.mX << "," << mData.mData.mY << ")" << std::endl;
    os << "  Unknown S1: " << mData.mData.mS1 << std::endl;
    if ((unsigned int)mData.mData.mS2 != mData.mPoints.size())
        os << "  Reported Point Count: " << mData.mData.mS2 << std::endl;
    os << "  Point Count: " << mData.mPoints.size() << std::endl;
    os << "  Edge Count: " << mData.mEdges.size() << std::endl;

    int i = 0;
    ESM::Pathgrid::PointList::iterator pit;
    for (pit = mData.mPoints.begin(); pit != mData.mPoints.end(); pit++)
    {
        os << "  Point[" << i << "]:" << std::endl;
        os << "    Coordinates: (" << pit->mX << ","
             << pit->mY << "," << pit->mZ << ")" << std::endl;
        os << "    Auto-Generated: " << (int)pit->mAutogenerated << std::endl;
        os << "    Connections: " << (int)pit->mConnectionNum << std::endl;
        os << "    Unknown: " << pit->mUnknown << std::endl;
        i++;
    }
    i = 0;
    ESM::Pathgrid::EdgeList::iterator eit;
    for (eit = mData.mEdges.begin(); eit != mData.mEdges.end(); eit++)
    {
        os << "  Edge[" << i << "]: " << eit->mV0 << " -> " << eit->mV1 << std::endl;
        if (eit->mV0 >= mData.mData.mS2 || eit->mV1 >= mData.mData.mS2)
            os << "  BAD POINT IN EDGE!" << std::endl;
        i++;
    }

    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Race>::print(std::ostream& os, int verbose)
{
    static const char *sAttributeNames[8] =
    {
        "Strength", "Intelligence", "Willpower", "Agility",
        "Speed", "Endurance", "Personality", "Luck"
    };

    os << "  Name: " << mData.mName << std::endl;
    os << "  Description: " << mData.mDescription << std::endl;
    os << "  Flags: " << raceFlags(mData.mData.mFlags) << std::endl;

    for (int i=0; i<2; ++i)
    {
        bool male = i==0;

        os << (male ? "  Male:" : "  Female:") << std::endl;

        for (int j=0; j<8; ++j)
            os << "    " << sAttributeNames[j] << ": "
                << mData.mData.mAttributeValues[j].getValue (male) << std::endl;

        os << "    Height: " << mData.mData.mHeight.getValue (male) << std::endl;
        os << "    Weight: " << mData.mData.mWeight.getValue (male) << std::endl;
    }

    for (int i = 0; i != 7; i++)
        // Not all races have 7 skills.
        if (mData.mData.mBonus[i].mSkill != -1)
            os << "  Skill: "
                      << skillLabel(mData.mData.mBonus[i].mSkill)
                      << " (" << mData.mData.mBonus[i].mSkill << ") = "
                      << mData.mData.mBonus[i].mBonus << std::endl;

    std::vector<std::string>::iterator sit;
    for (sit = mData.mPowers.mList.begin(); sit != mData.mPowers.mList.end(); ++sit)
        os << "  Power: " << *sit << std::endl;

    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Region>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;

    os << "  Weather:" << std::endl;
    os << "    Clear: " << (int)mData.mData.mClear << std::endl;
    os << "    Cloudy: " << (int)mData.mData.mCloudy << std::endl;
    os << "    Foggy: " << (int)mData.mData.mFoggy << std::endl;
    os << "    Overcast: " << (int)mData.mData.mOvercast << std::endl;
    os << "    Rain: " << (int)mData.mData.mOvercast << std::endl;
    os << "    Thunder: " << (int)mData.mData.mThunder << std::endl;
    os << "    Ash: " << (int)mData.mData.mAsh << std::endl;
    os << "    Blight: " << (int)mData.mData.mBlight << std::endl;
    os << "    UnknownA: " << (int)mData.mData.mA << std::endl;
    os << "    UnknownB: " << (int)mData.mData.mB << std::endl;
    os << "  Map Color: " << mData.mMapColor << std::endl;
    if (mData.mSleepList != "")
        os << "  Sleep List: " << mData.mSleepList << std::endl;
    std::vector<ESM::Region::SoundRef>::iterator sit;
    for (sit = mData.mSoundList.begin(); sit != mData.mSoundList.end(); ++sit)
        os << "  Sound: " << (int)sit->mChance << " = " << sit->mSound.toString() << std::endl;
}

template<>
void Record<ESM::Script>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mId << std::endl;

    os << "  Num Shorts: " << mData.mData.mNumShorts << std::endl;
    os << "  Num Longs: " << mData.mData.mNumLongs << std::endl;
    os << "  Num Floats: " << mData.mData.mNumFloats << std::endl;
    os << "  Script Data Size: " << mData.mData.mScriptDataSize << std::endl;
    os << "  Table Size: " << mData.mData.mStringTableSize << std::endl;

    std::vector<std::string>::iterator vit;
    for (vit = mData.mVarNames.begin(); vit != mData.mVarNames.end(); ++vit)
        os << "  Variable: " << *vit << std::endl;

    os << "  ByteCode: ";
    std::vector<unsigned char>::iterator cit;
    for (cit = mData.mScriptData.begin(); cit != mData.mScriptData.end(); ++cit)
        os << boost::format("%02X") % (int)(*cit);
    os << std::endl;

    if (verbose >= 3)
    {
        os << "  Script:" << std::endl;
        os << "START--------------------------------------" << std::endl;
        os << mData.mScriptText << std::endl;
        os << "END----------------------------------------" << std::endl;
    }
    else
    {
        os << "  Script: [skipped]" << std::endl;
    }

    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Skill>::print(std::ostream& os, int verbose)
{
    os << "  ID: " << skillLabel(mData.mIndex)
              << " (" << mData.mIndex << ")" << std::endl;
    os << "  Description: " << mData.mDescription << std::endl;
    os << "  Governing Attribute: " << attributeLabel(mData.mData.mAttribute)
              << " (" << mData.mData.mAttribute << ")" << std::endl;
    os << "  Specialization: " << specializationLabel(mData.mData.mSpecialization)
              << " (" << mData.mData.mSpecialization << ")" << std::endl;
    for (int i = 0; i != 4; i++)
        os << "  UseValue[" << i << "]:" << mData.mData.mUseValue[i] << std::endl;
}

template<>
void Record<ESM::SoundGenerator>::print(std::ostream& os, int verbose)
{
    os << "  Creature: " << mData.mCreature << std::endl;
    os << "  Sound: " << mData.mSound << std::endl;
    os << "  Type: " << soundTypeLabel(mData.mType)
              << " (" << mData.mType << ")" << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Sound>::print(std::ostream& os, int verbose)
{
    os << "  Sound: " << mData.mSound << std::endl;
    os << "  Volume: " << (int)mData.mData.mVolume << std::endl;
    if (mData.mData.mMinRange != 0 && mData.mData.mMaxRange != 0)
        os << "  Range: " << (int)mData.mData.mMinRange << " - "
                  << (int)mData.mData.mMaxRange << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Spell>::print(std::ostream& os, int verbose)
{
    os << "  Name: " << mData.mName << std::endl;
    os << "  Type: " << spellTypeLabel(mData.mData.mType)
              << " (" << mData.mData.mType << ")" << std::endl;
    os << "  Flags: " << spellFlags(mData.mData.mFlags) << std::endl;
    os << "  Cost: " << mData.mData.mCost << std::endl;
    printEffectList(os, mData.mEffects);
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::StartScript>::print(std::ostream& os, int verbose)
{
    os << "  Start Script: " << mData.mId << std::endl;
    os << "  Start Data: " << mData.mData << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<>
void Record<ESM::Static>::print(std::ostream& os, int verbose)
{
    os << "  Model: " << mData.mModel << std::endl;
}

template<>
void Record<ESM::Weapon>::print(std::ostream& os, int verbose)
{
    // No names on VFX bolts
    if (mData.mName != "")
        os << "  Name: " << mData.mName << std::endl;
    os << "  Model: " << mData.mModel << std::endl;
    // No icons on VFX bolts or magic bolts
    if (mData.mIcon != "")
        os << "  Icon: " << mData.mIcon << std::endl;
    if (mData.mScript != "")
        os << "  Script: " << mData.mScript << std::endl;
    if (mData.mEnchant != "")
        os << "  Enchantment: " << mData.mEnchant << std::endl;
    os << "  Type: " << weaponTypeLabel(mData.mData.mType)
              << " (" << mData.mData.mType << ")" << std::endl;
    os << "  Flags: " << weaponFlags(mData.mData.mFlags) << std::endl;
    os << "  Weight: " << mData.mData.mWeight << std::endl;
    os << "  Value: " << mData.mData.mValue << std::endl;
    os << "  Health: " << mData.mData.mHealth << std::endl;
    os << "  Speed: " << mData.mData.mSpeed << std::endl;
    os << "  Reach: " << mData.mData.mReach << std::endl;
    os << "  Enchantment Points: " << mData.mData.mEnchant << std::endl;
    if (mData.mData.mChop[0] != 0 && mData.mData.mChop[1] != 0)
        os << "  Chop: " << (int)mData.mData.mChop[0] << "-"
                  << (int)mData.mData.mChop[1] << std::endl;
    if (mData.mData.mSlash[0] != 0 && mData.mData.mSlash[1] != 0)
        os << "  Slash: " << (int)mData.mData.mSlash[0] << "-"
                  << (int)mData.mData.mSlash[1] << std::endl;
    if (mData.mData.mThrust[0] != 0 && mData.mData.mThrust[1] != 0)
        os << "  Thrust: " << (int)mData.mData.mThrust[0] << "-"
                  << (int)mData.mData.mThrust[1] << std::endl;
    os << "  Deleted: " << mIsDeleted << std::endl;
}

template<> void Record<ESM::Cell>::load(ESM::ESMReader &esm)
{
    mData.load(esm, mIsDeleted);
    if (mData.isExterior()) {
        std::stringstream ss;
        ss << "cell_" << mData.getGridX() << "_" << mData.getGridY();
        mId = ss.str();
    }else{
        mId = mData.mName;
    }
}

template<> void Record<ESM::Land>::load(ESM::ESMReader &esm)
{
    mData.load(esm, mIsDeleted);
    std::stringstream ss;
    ss << "land_" << mData.mX << "_" << mData.mY;
    mId = ss.str();
}

template<> void Record<ESM::MagicEffect>::load(ESM::ESMReader &esm)
{
    mData.load(esm, mIsDeleted);
    mId = "mgef_" + mData.mId.substr(4);
}

template<> void Record<ESM::Pathgrid>::load(ESM::ESMReader &esm)
{
    mData.load(esm, mIsDeleted);
    if (mData.mData.mX == 0 && mData.mData.mY == 0) {
        mId = mData.mCell;
    }else{
        std::stringstream ss;
        ss << "pgrd_" << mData.mData.mX << "_" << mData.mData.mY;
        mId = ss.str();
    }
}

template<> void Record<ESM::Skill>::load(ESM::ESMReader &esm)
{
    mData.load(esm, mIsDeleted);
    mId = "skil_" +  mData.mId.substr(3);
}

template<> bool Record<ESM::Cell>::loadId(ESM::ESMReader &esm)
{
    ESM::Cell::DATAstruct data;
    std::string name;

    bool foundName = false, foundData = false;

    while (esm.hasMoreSubs())
    {
        esm.getSubName();
        switch (esm.retSubName().intval)
        {
            case ESM::SREC_NAME:
                name = esm.getHString();
                foundName = true;
                break;
            case ESM::FourCC<'D','A','T','A'>::value:
                esm.getHT(data, 12);
                foundData = true;
                break;
            default:
                esm.skipHSub();
                break;
        }

        if(foundName && foundData)
            break;
    }

    if (! (data.mFlags & ESM::Cell::Interior)) {
        std::stringstream ss;
        ss << "cell_" << data.mX << "_" << data.mY;
        mId = ss.str();
    }else{
        mId = name;
    }

    return false;
}

template<> bool Record<ESM::Land>::loadId(ESM::ESMReader &esm)
{
    if(!esm.hasMoreSubs())
        throw std::runtime_error("land missing intv subrecord");

    esm.getSubNameIs("INTV");
    esm.getSubHeaderIs(8);
    int x;
    int y;
    esm.getT<int>(x);
    esm.getT<int>(y);

    std::stringstream ss;
    ss << "land_" << x << "_" << y;
    mId = ss.str();

    return false;
}

template<> bool Record<ESM::MagicEffect>::loadId(ESM::ESMReader &esm)
{
    int index;
    esm.getHNT(index, "INDX");
    mId = "mgef_" + ESM::MagicEffect::indexToId(index).substr(4);
    return false;
}

template<> bool Record<ESM::Pathgrid>::loadId(ESM::ESMReader &esm)
{
    ESM::Pathgrid::DATAstruct data;
    std::string cell;
    bool foundData = false;
    bool foundCell = false;
    while (esm.hasMoreSubs())
    {
        esm.getSubName();
        switch (esm.retSubName().intval)
        {
            case ESM::SREC_NAME:
                cell = esm.getHString();
                foundCell = true;
                break;
            case ESM::FourCC<'D','A','T','A'>::value:
                esm.getHT(data, 12);
                foundData = true;
                break;
            default:
                esm.skipHSub();
        }

        if(foundData && foundCell)
            break;
    }

    if(!foundData)
        throw std::runtime_error("pgrd missing data subrecord");

    if (data.mX == 0 && data.mY == 0) { // @Ques all inner cell has pgrd coord 0,0?
        mId = cell;
    }else{
        std::stringstream ss;
        ss << "pgrd_" << data.mX << "_" << data.mY;
        mId = ss.str();
    }

    return false;
}

template<> bool Record<ESM::Skill>::loadId(ESM::ESMReader &esm)
{
    int index;
    esm.getHNT(index, "INDX");
    mId = "skil_" + ESM::Skill::indexToId(index).substr(3);
    return false;
}

template<> bool Record<ESM::Script>::loadId(ESM::ESMReader &esm)
{
    ESM::Script::SCHD data;
    esm.getHNT(data, "SCHD", 52);
    mId = data.mName.toString();
    return false;
}

template<> bool Record<ESM::DialInfo>::loadId(ESM::ESMReader &esm)
{
    mId = esm.getHNString("INAM");
    return false;
}

template<> const ESM::InventoryList* Record<ESM::Creature>::getInventoryList(){
    return &mData.mInventory;
}
template<> const ESM::InventoryList* Record<ESM::Container>::getInventoryList(){
    return &mData.mInventory;
}
template<> const ESM::InventoryList* Record<ESM::NPC>::getInventoryList(){
    return &mData.mInventory;
}
template<> const ESM::SpellList* Record<ESM::Creature>::getSpellList(){
    return &mData.mSpells;
}
template<> const ESM::SpellList* Record<ESM::NPC>::getSpellList(){
    return &mData.mSpells;
}

#define addRecordProperty(property, varName)                     \
    {                                                            \
        #property, [](const void* d) -> std::string {            \
         const DataType* data = static_cast<const DataType*>(d); \
         std::stringstream ss;                                   \
         ss << data->varName;                                    \
         return ss.str();                                        \
       }                                                         \
    }

// force int convert
#define addIntRecordProperty(property, varName)                  \
    {                                                            \
        #property, [](const void* d) -> std::string {            \
         const DataType* data = static_cast<const DataType*>(d); \
         std::stringstream ss;                                   \
         ss << static_cast<int>(data->varName);                  \
         return ss.str();                                        \
       }                                                         \
    }

template<> PropertyMap Record<ESM::Activator>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(model, mModel)
};
template<> PropertyMap Record<ESM::Potion>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(icon, mIcon)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(weight, mData.mWeight)
    , addRecordProperty(value, mData.mValue)
    , addRecordProperty(autocalc, mData.mAutoCalc)
};
template<> PropertyMap Record<ESM::Armor>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(icon, mIcon)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(enchant, mEnchant)
    , addRecordProperty(type, mData.mType)
    , addRecordProperty(weight, mData.mWeight)
    , addRecordProperty(value, mData.mValue)
    , addRecordProperty(health, mData.mHealth)
    , addRecordProperty(data.enchant, mEnchant)
    , addRecordProperty(armor, mData.mArmor)
    // @TODO add parts?
};
template<> PropertyMap Record<ESM::Apparatus>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(icon, mIcon)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(type, mData.mType)
    , addRecordProperty(quality, mData.mQuality)
    , addRecordProperty(weight, mData.mWeight)
    , addRecordProperty(value, mData.mValue)
};
template<> PropertyMap Record<ESM::BodyPart>::sProperties = {
    addRecordProperty(model, mModel)
    , addRecordProperty(race, mRace)
    , addIntRecordProperty(part, mData.mPart)
    , addIntRecordProperty(vampire, mData.mVampire)
    , addIntRecordProperty(flags, mData.mFlags)
    , addRecordProperty(type, mData.mType)
};
template<> PropertyMap Record<ESM::Book>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(icon, mIcon)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(enchant, mEnchant)
    , addRecordProperty(weight, mData.mWeight)
    , addRecordProperty(value, mData.mValue)
    , addRecordProperty(isscroll, mData.mIsScroll)
    , addRecordProperty(skillid, mData.mSkillId)
    , addRecordProperty(data.enchant, mData.mEnchant)
};
template<> PropertyMap Record<ESM::BirthSign>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(texture, mTexture)
};
template<> PropertyMap Record<ESM::Cell>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(region, mRegion)
    , addRecordProperty(flags, mData.mFlags)
    , addRecordProperty(x, mData.mX)
    , addRecordProperty(y, mData.mY)
    , addRecordProperty(water, mWater)
    , addRecordProperty(waterint, mWaterInt)
    , addRecordProperty(mapcolor, mMapColor)
    , addRecordProperty(ambient, mAmbi.mAmbient)
    , addRecordProperty(sunlight, mAmbi.mSunlight)
    , addRecordProperty(fog, mAmbi.mFog)
    , addRecordProperty(fogdensity, mAmbi.mFogDensity)
};
template<> PropertyMap Record<ESM::Class>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(attribute0, mData.mAttribute[0])
    , addRecordProperty(attribute1, mData.mAttribute[1])
    , addRecordProperty(specialization, mData.mSpecialization)
    , addRecordProperty(skill00, mData.mSkills[0][0])
    , addRecordProperty(skill10, mData.mSkills[1][0])
    , addRecordProperty(skill20, mData.mSkills[2][0])
    , addRecordProperty(skill30, mData.mSkills[3][0])
    , addRecordProperty(skill40, mData.mSkills[4][0])
    , addRecordProperty(skill01, mData.mSkills[0][1])
    , addRecordProperty(skill11, mData.mSkills[1][1])
    , addRecordProperty(skill21, mData.mSkills[2][1])
    , addRecordProperty(skill31, mData.mSkills[3][1])
    , addRecordProperty(skill41, mData.mSkills[4][1])
    , addRecordProperty(isplayable, mData.mIsPlayable)
    , addRecordProperty(calc, mData.mCalc)
};
template<> PropertyMap Record<ESM::Clothing>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(icon, mIcon)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(enchant, mEnchant)
    , addRecordProperty(type, mData.mType)
    , addRecordProperty(weight, mData.mWeight)
    , addRecordProperty(value, mData.mValue)
    , addRecordProperty(data.enchant, mData.mEnchant)
};
template<> PropertyMap Record<ESM::Container>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(weight, mWeight)
    , addRecordProperty(flags, mFlags)
};
template<> PropertyMap Record<ESM::Creature>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(original, mOriginal)
    , addRecordProperty(hasai, mHasAI)
    , addIntRecordProperty(ai.hello, mAiData.mHello)
    , addIntRecordProperty(ai.u1, mAiData.mU1)
    , addIntRecordProperty(ai.fight, mAiData.mFight)
    , addIntRecordProperty(ai.flee, mAiData.mFlee)
    , addIntRecordProperty(ai.alarm, mAiData.mAlarm)
    , addIntRecordProperty(ai.u2, mAiData.mU2)
    , addIntRecordProperty(ai.u3, mAiData.mU3)
    , addIntRecordProperty(ai.u4, mAiData.mU4)
    , addIntRecordProperty(ai.services, mAiData.mServices)
    , addRecordProperty(type, mData.mType)
    , addRecordProperty(level, mData.mLevel)
    , addRecordProperty(stength, mData.mStrength)
    , addRecordProperty(intelligence, mData.mIntelligence)
    , addRecordProperty(willpower, mData.mWillpower)
    , addRecordProperty(agility, mData.mAgility)
    , addRecordProperty(speed, mData.mSpeed)
    , addRecordProperty(endurance, mData.mEndurance)
    , addRecordProperty(personality, mData.mPersonality)
    , addRecordProperty(luck, mData.mLuck)
    , addRecordProperty(health, mData.mHealth)
    , addRecordProperty(mana, mData.mMana)
    , addRecordProperty(fatigue, mData.mFatigue)
    , addRecordProperty(soul, mData.mSoul)
    , addRecordProperty(combat, mData.mCombat)
    , addRecordProperty(magic, mData.mMagic)
    , addRecordProperty(stealth, mData.mStealth)
    , addRecordProperty(minattack1, mData.mAttack[0])
    , addRecordProperty(maxattack1, mData.mAttack[1])
    , addRecordProperty(minattack2, mData.mAttack[2])
    , addRecordProperty(maxattack2, mData.mAttack[3])
    , addRecordProperty(minattack3, mData.mAttack[4])
    , addRecordProperty(maxattack3, mData.mAttack[5])
    , addRecordProperty(gold, mData.mGold)
    , addRecordProperty(flags, mFlags)
    , addRecordProperty(persistent, mPersistent)
    , addRecordProperty(scale, mScale)
    // TODO add inventory and spells
};
template<> PropertyMap Record<ESM::Dialogue>::sProperties = {
    addRecordProperty(type, mType)
};
template<> PropertyMap Record<ESM::Door>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(opensound, mOpenSound)
    , addRecordProperty(closesound, mCloseSound)
};
template<> PropertyMap Record<ESM::Enchantment>::sProperties = {
    addRecordProperty(type, mData.mType)
    , addRecordProperty(cost, mData.mCost)
    , addRecordProperty(charge, mData.mCharge)
    , addRecordProperty(autocalc, mData.mAutocalc)
};
template<> PropertyMap Record<ESM::Faction>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(attribute0, mData.mAttribute[0])
    , addRecordProperty(attribute1, mData.mAttribute[1])
    , addRecordProperty(skill0, mData.mSkills[0])
    , addRecordProperty(skill1, mData.mSkills[1])
    , addRecordProperty(skill2, mData.mSkills[2])
    , addRecordProperty(skill3, mData.mSkills[3])
    , addRecordProperty(skill4, mData.mSkills[4])
    , addRecordProperty(skill5, mData.mSkills[5])
    , addRecordProperty(skill6, mData.mSkills[6])
    , addRecordProperty(ishidden, mData.mIsHidden)
    // TODO handle rand data
};
template<> PropertyMap Record<ESM::Global>::sProperties = {
    // TODO handle variant value
};
template<> PropertyMap Record<ESM::GameSetting>::sProperties = {
    // TODO handle variant value
};
template<> PropertyMap Record<ESM::DialInfo>::sProperties = {
    addRecordProperty(actor, mActor)
    , addRecordProperty(race, mRace)
    , addRecordProperty(class, mClass)
    , addRecordProperty(faction, mFaction)
    , addRecordProperty(pcfaction, mPcFaction)
    , addRecordProperty(cell, mCell)
    , addRecordProperty(sound, mSound)
    , addRecordProperty(response, mResponse)
    , addRecordProperty(resultscript, mResultScript)
    , addRecordProperty(factionless, mFactionLess)
    , addRecordProperty(queststatus, mQuestStatus)
    , addRecordProperty(unknown1, mData.mUnknown1)
    , addRecordProperty(disposition, mData.mDisposition)
    , addRecordProperty(journalindex, mData.mJournalIndex)
    , addRecordProperty(rank, mData.mRank)
    , addRecordProperty(gender, mData.mGender)
    , addRecordProperty(pcrank, mData.mPCrank)
    , addRecordProperty(unknown1, mData.mUnknown1)
};
template<> PropertyMap Record<ESM::Ingredient>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(icon, mIcon)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(weight, mData.mWeight)
    , addRecordProperty(value, mData.mValue)
    , addRecordProperty(effectid0, mData.mEffectID[0])
    , addRecordProperty(effectid1, mData.mEffectID[1])
    , addRecordProperty(effectid2, mData.mEffectID[2])
    , addRecordProperty(effectid3, mData.mEffectID[3])
    , addRecordProperty(skills0, mData.mSkills[0])
    , addRecordProperty(skills1, mData.mSkills[1])
    , addRecordProperty(skills2, mData.mSkills[2])
    , addRecordProperty(skills3, mData.mSkills[3])
    , addRecordProperty(attributes0, mData.mAttributes[0])
    , addRecordProperty(attributes1, mData.mAttributes[1])
    , addRecordProperty(attributes2, mData.mAttributes[2])
    , addRecordProperty(attributes3, mData.mAttributes[3])
};
template<> PropertyMap Record<ESM::Land>::sProperties = {
    addRecordProperty(flags, mFlags)
    , addRecordProperty(x, mX)
    , addRecordProperty(y, mY)
    , addRecordProperty(plugin, mPlugin)
    , addRecordProperty(datatypes, mDataTypes)
};
template<> PropertyMap Record<ESM::CreatureLevList>::sProperties = {
    addRecordProperty(flags, mFlags)
    , addIntRecordProperty(chancenone, mChanceNone)
    // TODO handle levelitem?
};
template<> PropertyMap Record<ESM::ItemLevList>::sProperties = {
    addRecordProperty(flags, mFlags)
    , addIntRecordProperty(chancenone, mChanceNone)
};
template<> PropertyMap Record<ESM::Light>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(icon, mIcon)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(weight, mData.mWeight)
    , addRecordProperty(value, mData.mValue)
    , addRecordProperty(time, mData.mTime)
    , addRecordProperty(radius, mData.mRadius)
    , addRecordProperty(color, mData.mColor)
    , addRecordProperty(flags, mData.mFlags)
};
template<> PropertyMap Record<ESM::Lockpick>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(icon, mIcon)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(weight, mData.mWeight)
    , addRecordProperty(value, mData.mValue)
    , addRecordProperty(quality, mData.mQuality)
    , addRecordProperty(uses, mData.mUses)
};
template<> PropertyMap Record<ESM::Probe>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(icon, mIcon)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(weight, mData.mWeight)
    , addRecordProperty(value, mData.mValue)
    , addRecordProperty(quality, mData.mQuality)
    , addRecordProperty(uses, mData.mUses)
};
template<> PropertyMap Record<ESM::Repair>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(icon, mIcon)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(weight, mData.mWeight)
    , addRecordProperty(value, mData.mValue)
    , addRecordProperty(quality, mData.mQuality)
    , addRecordProperty(uses, mData.mUses)
};
template<> PropertyMap Record<ESM::LandTexture>::sProperties = {
    addRecordProperty(texture, mTexture)
    , addRecordProperty(index, mIndex)
};
template<> PropertyMap Record<ESM::MagicEffect>::sProperties = {
    addRecordProperty(icon, mIcon)
    , addRecordProperty(particle, mParticle)
    , addRecordProperty(casting, mCasting)
    , addRecordProperty(hit, mHit)
    , addRecordProperty(area, mArea)
    , addRecordProperty(bolt, mBolt)
    , addRecordProperty(castsound, mCastSound)
    , addRecordProperty(boltsound, mBoltSound)
    , addRecordProperty(hitsound, mHitSound)
    , addRecordProperty(areasound, mAreaSound)
    , addRecordProperty(index, mIndex)
    , addRecordProperty(school, mData.mSchool)
    , addRecordProperty(basecost, mData.mBaseCost)
    , addRecordProperty(flags, mData.mFlags)
    , addRecordProperty(red, mData.mRed)
    , addRecordProperty(green, mData.mGreen)
    , addRecordProperty(blue, mData.mBlue)
    , addRecordProperty(unknown1, mData.mUnknown1)
    , addRecordProperty(speed, mData.mSpeed)
    , addRecordProperty(unknown2, mData.mUnknown2)
};
template<> PropertyMap Record<ESM::Miscellaneous>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(icon, mIcon)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(weight, mData.mWeight)
    , addRecordProperty(value, mData.mValue)
    , addRecordProperty(iskey, mData.mIsKey)
};
template<> PropertyMap Record<ESM::NPC>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(race, mRace)
    , addRecordProperty(class, mClass)
    , addRecordProperty(faction, mFaction)
    , addRecordProperty(hair, mHair)
    , addRecordProperty(head, mHead)
    , addRecordProperty(flags, mFlags)
    , addRecordProperty(persistent, mPersistent)
    , addIntRecordProperty(npdttype, mNpdtType)
    // @TODO handle nptd
};
template<> PropertyMap Record<ESM::Pathgrid>::sProperties = {
    addRecordProperty(cell, mCell)
    , addRecordProperty(x, mData.mX)
    , addRecordProperty(y, mData.mY)
    , addRecordProperty(s1, mData.mS1)
    , addRecordProperty(s2, mData.mS2)
};
template<> PropertyMap Record<ESM::Race>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(flags, mData.mFlags)
};
template<> PropertyMap Record<ESM::Region>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(sleeplist, mSleepList)
    , addIntRecordProperty(clear, mData.mClear)
    , addIntRecordProperty(cloudy, mData.mCloudy)
    , addIntRecordProperty(foggy, mData.mFoggy)
    , addIntRecordProperty(overcast, mData.mOvercast)
    , addIntRecordProperty(rain, mData.mRain)
    , addIntRecordProperty(thunder, mData.mThunder)
    , addIntRecordProperty(ash, mData.mAsh)
    , addIntRecordProperty(blight, mData.mBlight)
    , addIntRecordProperty(a, mData.mA)
    , addIntRecordProperty(b, mData.mB)
};
template<> PropertyMap Record<ESM::Script>::sProperties = {
    addRecordProperty(numshorts, mData.mNumShorts)
    , addRecordProperty(numlongs, mData.mNumLongs)
    , addRecordProperty(numfloats, mData.mNumFloats)
    , addRecordProperty(scriptdatasize, mData.mScriptDataSize)
    , addRecordProperty(stringtablesize, mData.mStringTableSize)
};
template<> PropertyMap Record<ESM::Skill>::sProperties = {
    addRecordProperty(index, mIndex)
    , addRecordProperty(attribute, mData.mAttribute)
    , addRecordProperty(specialization, mData.mSpecialization)
    , addRecordProperty(usevalue, mData.mUseValue[0])
    , addRecordProperty(usevalue, mData.mUseValue[1])
    , addRecordProperty(usevalue, mData.mUseValue[2])
    , addRecordProperty(usevalue, mData.mUseValue[3])
};
template<> PropertyMap Record<ESM::SoundGenerator>::sProperties = {
    addRecordProperty(type, mType)
    , addRecordProperty(creature, mCreature)
    , addRecordProperty(sound, mSound)
};
template<> PropertyMap Record<ESM::Sound>::sProperties = {
    addRecordProperty(sound, mSound)
    , addIntRecordProperty(volume, mData.mVolume)
    , addIntRecordProperty(minrange, mData.mMinRange)
    , addIntRecordProperty(maxrange, mData.mMaxRange)
};
template<> PropertyMap Record<ESM::Spell>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(type, mData.mType)
    , addRecordProperty(cost, mData.mCost)
    , addRecordProperty(flags, mData.mFlags)
};
template<> PropertyMap Record<ESM::StartScript>::sProperties = {
    addRecordProperty(data, mData)
};
template<> PropertyMap Record<ESM::Static>::sProperties = {
    addRecordProperty(model, mModel)
};
template<> PropertyMap Record<ESM::Weapon>::sProperties = {
    addRecordProperty(name, mName)
    , addRecordProperty(model, mModel)
    , addRecordProperty(icon, mIcon)
    , addRecordProperty(scirpt, mScript)
    , addRecordProperty(enchant, mEnchant)
    , addRecordProperty(weight, mData.mWeight)
    , addRecordProperty(value, mData.mValue)
    , addRecordProperty(type, mData.mType)
    , addRecordProperty(health, mData.mHealth)
    , addRecordProperty(speed, mData.mSpeed)
    , addRecordProperty(reach, mData.mReach)
    , addRecordProperty(data.enchant, mData.mEnchant)
    , addIntRecordProperty(minchop, mData.mChop[0])
    , addIntRecordProperty(maxchop, mData.mChop[1])
    , addIntRecordProperty(minslash, mData.mSlash[0])
    , addIntRecordProperty(maxslash, mData.mSlash[1])
    , addIntRecordProperty(minthrust, mData.mThrust[0])
    , addIntRecordProperty(maxthrust, mData.mThrust[1])
    , addRecordProperty(flags, mData.mFlags)
};

} // end namespace
