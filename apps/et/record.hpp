#ifndef OPENMW_ESMTOOL_RECORD_H
#define OPENMW_ESMTOOL_RECORD_H

#include <string>

#include <components/esm/records.hpp>
#include <components/esm/esmreader.hpp>

namespace ESM
{
    class ESMReader;
    class ESMWriter;
}

namespace ET
{
    template <class T> class Record;
    typedef std::map<std::string, std::function<std::string(const void*) > > PropertyMap;

    class RecordBase
    {
    protected:
        std::string mId;
        uint32_t mFlags;
        ESM::NAME mType;

    public:
        RecordBase ()
          : mFlags(0)
        {
        }

        virtual ~RecordBase() {}

        uint32_t getFlags() const {
            return mFlags;
        }

        void setFlags(uint32_t flags) {
            mFlags = flags;
        }

        ESM::NAME getType() const {
            return mType;
        }

        virtual void load(ESM::ESMReader &esm) = 0;
        // return whether this record is loaded
        virtual bool loadId(ESM::ESMReader &esm) = 0;
        virtual void print(std::ostream& os, int verbose) = 0;
        virtual std::string getId() const = 0;
        virtual std::string getLowerCaseId();

        static RecordBase *create(ESM::NAME type);

        // just make it a bit shorter
        template <class T>
        Record<T> *cast() {
            return static_cast<Record<T> *>(this);
        }

        virtual bool hasProperty(const std::string& name) const = 0;
        virtual std::string getProperty(const std::string& name) const = 0;
        virtual PropertyMap& getPropertyMap() = 0;
        virtual const ESM::InventoryList* getInventoryList() = 0;
        virtual const ESM::SpellList* getSpellList() = 0;

    };

    template <class T>
    class Record : public RecordBase
    {
        typedef T DataType;
        T mData;
        bool mIsDeleted;

        static PropertyMap sProperties;

    public:
        Record()
            : mIsDeleted(false)
        {}

        virtual std::string getId() const {
            return mId;
        }

        T &get() {
            return mData;
        }

        void load(ESM::ESMReader &esm) {
            mData.load(esm, mIsDeleted);
            mId = mData.mId;
        }

        virtual bool loadId(ESM::ESMReader &esm){
            if (!esm.hasMoreSubs())
                throw std::runtime_error("missing id subrecord");

            esm.getSubName();
            uint32_t type = esm.retSubName().intval;

            if (type != ESM::SREC_NAME)
                throw std::runtime_error("record should use id as first sub record");

            mId = esm.getHString();

            return false;
        }

        virtual const ESM::InventoryList* getInventoryList(){return 0;}
        virtual const ESM::SpellList* getSpellList(){return 0;}

        void print(std::ostream& os, int verbose);

        bool hasProperty(const std::string& name) const {
            return sProperties.find(Misc::StringUtils::lowerCase(name)) != sProperties.end();
        }

        std::string getProperty(const std::string& name) const{
            static std::string s;
            auto iter = sProperties.find(Misc::StringUtils::lowerCase(name));
            if (iter == sProperties.end()) {
                std::stringstream ss;
                ss << "error:\nmissing " << name << " field";
                throw std::runtime_error(ss.str());
            }
            return iter != sProperties.end() ? iter->second(&mData) : s;
        }

        virtual PropertyMap& getPropertyMap(){return sProperties;}
    };

    template<> void Record<ESM::Cell>::load(ESM::ESMReader &esm);
    template<> void Record<ESM::Land>::load(ESM::ESMReader &esm);
    template<> void Record<ESM::MagicEffect>::load(ESM::ESMReader &esm);
    template<> void Record<ESM::Pathgrid>::load(ESM::ESMReader &esm);
    template<> void Record<ESM::Skill>::load(ESM::ESMReader &esm);

    template<> bool Record<ESM::Cell>::loadId(ESM::ESMReader &esm);
    template<> bool Record<ESM::Land>::loadId(ESM::ESMReader &esm);
    template<> bool Record<ESM::MagicEffect>::loadId(ESM::ESMReader &esm);
    template<> bool Record<ESM::Pathgrid>::loadId(ESM::ESMReader &esm);
    template<> bool Record<ESM::Skill>::loadId(ESM::ESMReader &esm);
    template<> bool Record<ESM::Script>::loadId(ESM::ESMReader &esm);
    template<> bool Record<ESM::DialInfo>::loadId(ESM::ESMReader &esm);

    template<> const ESM::InventoryList* Record<ESM::Creature>::getInventoryList();
    template<> const ESM::InventoryList* Record<ESM::Container>::getInventoryList();
    template<> const ESM::InventoryList* Record<ESM::NPC>::getInventoryList();

    template<> const ESM::SpellList* Record<ESM::Creature>::getSpellList();
    template<> const ESM::SpellList* Record<ESM::NPC>::getSpellList();

    template<> void Record<ESM::Activator>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Potion>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Armor>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Apparatus>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::BodyPart>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Book>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::BirthSign>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Cell>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Class>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Clothing>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Container>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Creature>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Dialogue>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Door>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Enchantment>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Faction>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Global>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::GameSetting>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::DialInfo>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Ingredient>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Land>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::CreatureLevList>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::ItemLevList>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Light>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Lockpick>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Probe>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Repair>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::LandTexture>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::MagicEffect>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Miscellaneous>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::NPC>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Pathgrid>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Race>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Region>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Script>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Skill>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::SoundGenerator>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Sound>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Spell>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::StartScript>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Static>::print(std::ostream& os, int verbose);
    template<> void Record<ESM::Weapon>::print(std::ostream& os, int verbose);
}

#endif
