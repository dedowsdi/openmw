#include <iostream>
#include <components/files/escape.hpp>

void testEscapeHashString(){
    std::cout << Files::EscapeHashString::processString("123@h123") << std::endl;
    std::cout << Files::EscapeHashString::processString("123@a123") << std::endl;
}

int main(int argc, char *argv[])
{
    testEscapeHashString();
    return 0;
}
