#include <iostream>
using namespace std;

int main()
{
    // streams, including the iostream are buffered. This text will not be output
    // until there is a \n or std::endl
    cout << "one ";
    cout << "two ";
    cout << "three\n";

    cout << "buffered text";
    cout << ". More text";
    cout.flush();

    // std::cerr is unbuffered
    cout << endl;
    cerr << "shit hit the fan";
}
