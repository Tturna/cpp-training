#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main()
{
    cout << "Write something to a file: \n";

    string user_input;
    getline(cin, user_input);

    ofstream my_filestream;
    my_filestream.open("outputfile.txt");
    my_filestream << user_input;
    my_filestream.close();

    ifstream my_inputfs;
    my_inputfs.open("outputfile.txt");
    string contents;
    my_inputfs >> contents;
    my_inputfs.close();

    cout << "Wrote and read: " << contents << endl;
    cout << "Notice how just like with regular io, file io also terminates on white space\n";

    my_inputfs.open("outputfile.txt");
    string line;
    getline(my_inputfs, line);

    cout << "Read: " << line << endl;
    
    return 0;
}
