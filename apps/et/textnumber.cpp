#include "textnumber.hpp"
#include "components/misc/stringops.hpp"

namespace ET
{

//--------------------------------------------------------------------
TextNumber::TextNumber(const std::string& text) :
    mIsNumber(false),
    mIsRange(false),
    mRegexCompare(false),
    mMinCompare(false),
    mMaxCompare(false),
    mMin(0),
    mMax(0),
    mValue(0)
{
    setText(text);
}

//--------------------------------------------------------------------
bool TextNumber::empty()
{
    return mText.empty();
}

//--------------------------------------------------------------------
bool TextNumber::compare(const std::string& value)
{
    if (mText.empty())
        throw std::runtime_error("empty text number");

    if (!mIsRange && !mIsNumber){
        return mRegexCompare ? std::regex_match(value, mPattern) : value == mText;
    }else{
        try {
            float f = std::stof(value);
            return compare(f);
        }catch(...) {
            return false;
        }
    }
}

//--------------------------------------------------------------------
bool TextNumber::compare(float value)
{
    if (mText.empty())
        throw std::runtime_error("empty text number");

    if (!mIsRange && !mIsNumber)
        return false;

    if(mIsNumber)
        return value == mValue;

    if(mMinCompare && value < mMin)
        return false;

    if(mMaxCompare && value > mMax)
        return false;

    return true;
}

//--------------------------------------------------------------------
void TextNumber::setText(const std::string& v)
{
    mText = Misc::StringUtils::lowerCase(v);
    if (mText.empty())
        return;

    mMinCompare = false;
    mMaxCompare = false;
    mIsNumber = false;
    mIsRange = false;

    mPattern.assign(v);

    std::string::size_type index = mText.find(':');
    mIsRange = index != std::string::npos;

    if (!mIsRange){
        try {
           mValue = std::stof(mText);
           mIsNumber = true;
        }catch(...) {
            mIsNumber = false;
        }
        return;
    }

    if (index != 0){
       mMin = std::stof(mText.substr(0, index));
       mMinCompare = true;
    }

    if (index != mText.size() - 1){
        mMax = std::stof(mText.substr(index+1));
        mMaxCompare = true;
    }
}

//--------------------------------------------------------------------
std::istream& operator>>(std::istream& is , TextNumber& tn)
{
    std::string s;
    is >> s;
    tn.setText(s);
    return is;
}

}
