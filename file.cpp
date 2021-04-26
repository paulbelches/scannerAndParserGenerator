#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using std::cout; using std::cerr;
using std::endl; using std::string;
using std::ifstream; using std::vector;

int generateStream(string& file){
    string filename("cocol.txt");
    vector<char> bytes;
    char byte = 0;

    ifstream input_file(filename);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '" << filename << "'" << endl;
        return -1;
    }

    while (input_file.get(byte)) {
        bytes.push_back(byte);
    }
    for (const auto &i : bytes) {
        file = file + i;
        //cout << i << " " << (int)i << " " << isspace(i) << endl;
    }
    cout << endl;
    input_file.close();

    return 1;
}

int main()
{
    string file;
    if (generateStream(file) > 0){
        cout << file << endl;
    }
}