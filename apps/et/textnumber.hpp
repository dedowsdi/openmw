#ifndef TEXTNUMBER_HPP
#define TEXTNUMBER_HPP

#include <string>
#include <regex>

namespace ET{

class TextNumber{

private:

    bool mIsNumber;
    bool mIsRange;
    bool mRegexCompare; // only when !mIsNumber && !mIsRange
    bool mMinCompare;
    bool mMaxCompare;
    float mMin;
    float mMax;
    float mValue;
    std::string mText;
    std::regex mPattern;

public:

    TextNumber(){}

    TextNumber(const std::string& text);

    bool empty();

    bool compare(const std::string& value);

    bool compare(float value);

    const std::string& getText() const { return mText; }
    void setText(const std::string& v);

    bool getRegexCompare() const { return mRegexCompare; }
    void setRegexCompare(bool v){ mRegexCompare = v; }

};

std::istream& operator>>(std::istream& is , TextNumber& tn);

}

#endif /* TEXTNUMBER_HPP */
