#include <iostream>
#include <sstream>

int main(int argc, char **argv)
{
    std::stringstream ss("10.2345asdf");
    double ret;
    ss >> ret;

    std::cout << "Double: " << ret << std::endl;
    return 0;
}
