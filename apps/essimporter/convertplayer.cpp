#include "convertplayer.hpp"

#include <components/misc/stringops.hpp>

namespace ESSImport
{

    void convertPCDT(const PCDT& pcdt, ESM::Player& out, std::vector<std::string>& outDialogueTopics, bool& firstPersonCam, ESM::ControlsState& controls)
    {
        out.mBirthsign = pcdt.mBirthsign;
        out.mObject.mNpcStats.mBounty = pcdt.mBounty;
        for (std::vector<PCDT::FNAM>::const_iterator it = pcdt.mFactions.begin(); it != pcdt.mFactions.end(); ++it)
        {
            ESM::NpcStats::Faction faction;
            faction.mExpelled = (it->mFlags & 0x2) != 0;
            faction.mRank = it->mRank;
            faction.mReputation = it->mReputation;
            out.mObject.mNpcStats.mFactions[Misc::StringUtils::lowerCase(it->mFactionName.toString())] = faction;
        }
        for (int i=0; i<3; ++i)
            out.mObject.mNpcStats.mSpecIncreases[i] = pcdt.mPNAM.mSpecIncreases[i];
        for (int i=0; i<8; ++i)
            out.mObject.mNpcStats.mSkillIncrease[i] = pcdt.mPNAM.mSkillIncreases[i];
        for (int i=0; i<27; ++i)
            out.mObject.mNpcStats.mSkills[i].mProgress = pcdt.mPNAM.mSkillProgress[i];
        out.mObject.mNpcStats.mLevelProgress = pcdt.mPNAM.mLevelProgress;

        if (pcdt.mPNAM.mPlayerFlags & PCDT::PlayerFlags_WeaponDrawn)
            out.mObject.mCreatureStats.mDrawState = 1;
        if (pcdt.mPNAM.mPlayerFlags & PCDT::PlayerFlags_SpellDrawn)
            out.mObject.mCreatureStats.mDrawState = 2;

        firstPersonCam = !(pcdt.mPNAM.mPlayerFlags & PCDT::PlayerFlags_ThirdPerson);

        for (std::vector<std::string>::const_iterator it = pcdt.mKnownDialogueTopics.begin();
             it != pcdt.mKnownDialogueTopics.end(); ++it)
        {
            outDialogueTopics.push_back(Misc::StringUtils::lowerCase(*it));
        }

        controls.mViewSwitchDisabled = pcdt.mPNAM.mPlayerFlags & PCDT::PlayerFlags_ViewSwitchDisabled;
        controls.mControlsDisabled = pcdt.mPNAM.mPlayerFlags & PCDT::PlayerFlags_ControlsDisabled;
        controls.mJumpingDisabled = pcdt.mPNAM.mPlayerFlags & PCDT::PlayerFlags_JumpingDisabled;
        controls.mLookingDisabled = pcdt.mPNAM.mPlayerFlags & PCDT::PlayerFlags_LookingDisabled;
        controls.mVanityModeDisabled = pcdt.mPNAM.mPlayerFlags & PCDT::PlayerFlags_VanityModeDisabled;
        controls.mWeaponDrawingDisabled = pcdt.mPNAM.mPlayerFlags & PCDT::PlayerFlags_WeaponDrawingDisabled;
        controls.mSpellDrawingDisabled = pcdt.mPNAM.mPlayerFlags & PCDT::PlayerFlags_SpellDrawingDisabled;
    }

}
