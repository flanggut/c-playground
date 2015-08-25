#include <iostream>
#include <sstream>

int main(int argc, char **argv)
{
    std::stringstream ss("10.2345asdf");
    double ret1;
    ss >> ret1;
    double ret2 = std::stod("10.2345asdf");

    std::cout << "std::stringstream Double: " << ret1 << std::endl;
    std::cout << "std::stod         Double: " << ret2 << std::endl;
    return 0;
}
