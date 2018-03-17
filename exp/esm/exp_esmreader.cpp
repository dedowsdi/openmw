#include <iostream>
#include "components/esm/esmreader.hpp"
#include "components/to_utf8/to_utf8.hpp"

#include <iomanip>

int main(int argc, char *argv[])
{
    ESM::ESMReader esm;
    
    ToUTF8::Utf8Encoder encoder (ToUTF8::calculateEncoding("win1252"));
    esm.setEncoder(&encoder);

    esm.open("/mnt/green/steam/steamapps/common/Morrowind/Data Files/Morrowind.esm");

    if (esm.hasMoreRecs()) {
        //ESM::NAME n = esm.getRecName();
        //uint32_t flags;
        //esm.getRecHeader(flags);

        std::cout << "Author: " << esm.getAuthor() << std::endl
             << "Description: " << esm.getDesc() << std::endl
             << "File format version: " << esm.getFVer() << std::endl;

        //std::cout << n << std::endl;
        
    }

    return 0;
}
