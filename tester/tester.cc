#include <iostream>
#include <sstream>

int main(int argc, char **argv)
{
    std::string str("10.2345asdf");
    std::stringstream ss(str);
    double ret = double();
    ss >> ret;

    std::cout << "String: " << str << std::endl;
    std::cout << "Double: " << ret << std::endl;

    return 0;
}
