#include <fstream>
#include <string>
#include <iostream>

using namespace std;

int main()
{
    const int LINES_TO_WRITE = 5;
    
    ofstream out_fs;
    out_fs.open("basics2.txt");

    for (int i = 0; i < LINES_TO_WRITE; i++)
    {
        cout << "Write a line to a file (" << i + 1 << "/" << LINES_TO_WRITE << "): \n";
        string in_line;
        getline(cin, in_line);
        out_fs << in_line << endl;
    }

    out_fs.close();
}
