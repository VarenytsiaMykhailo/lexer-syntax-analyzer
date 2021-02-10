//
// Created by MIKE on 10.02.2021.
//

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

const string inputFilePath = "D:\\github-reps\\syntax-analyzer\\testFragment.cpp";

int main() {

    // Открываем файл
    ifstream input(inputFilePath);
    if (!input) {
        cerr << "Can not open the file" << endl;
        exit(1);
    }

    // Считываем файл
    stringstream input_ss;
    for (string line; getline(input, line); input_ss << line << endl);
    input.close();

    string program = input_ss.str();
    cout << "Stage 0. Input program code:" << endl << program << endl;
    cout << "===========================================================" << endl;





    return 0;
}
