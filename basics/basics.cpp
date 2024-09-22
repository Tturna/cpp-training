#include <cstdio>
#include <ios>
#include <iostream>
#include <limits>
#include <string>
//using namespace std;

int main()
{
    std::cout << "Hello. What's good?" << std::endl;
    char* input = new char[50];
    std::cin >> input;
    std::cout << "Aight. " << input << std::endl;
    std::cout << "Notice how a whitespace in your input is considered a terminating character" << std::endl;
    
    std::cout << "Go again. Give at least two words\n";
    std::string str_input;
    getline(std::cin, str_input);
    std::cout << str_input << std::endl;

    std::cout << "Oops. The remainded of your first input was still in the input buffer!\n";
    std::cout << "Let's call std::cin.ignore()\n";

    // This clears x number of characters from the input buffer until a new line character,
    // where x is streamsize::max().
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // std::numeric_limits<> is a template class from the <limits> header.
    // It can give info about the min and max values of given types.
    // std::numeric_limits<int>::max() gives the max value of int.
    //
    // std::streamsize is a type alias often defines as long long or int depending on the system.
    // It represents the size of input/output streams.

    std::cout << "What's poppin?\n";
    std::string actual_input;
    getline(std::cin, actual_input);
    std::cout << actual_input << std::endl;

    return 0;
}
