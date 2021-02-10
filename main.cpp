//
// Created by MIKE on 10.02.2021.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>


using namespace std;


const string inputFilePath = "D:\\github-reps\\syntax-analyzer\\testFragment.cpp";


string normalizeProgram(string program) {
    /*
        R - отключает ескейп последовательности. \n? - отступ строки 0 ил и1 раз.
        //.* - однострочные комментарии | - или многострочные комментарии (* нужно экранировать т.к. служебный символ)
    */
    regex regexComments(R"(\n?//.*|/\*[\W\w]*?\*/)");
    regex regexSpaces(R"([ \t]*(\W)[ \t]*)");

    stringstream result;

    if (regex_search(program, regexComments)) {
        smatch smatchComments;
        while (regex_search(program, smatchComments, regexComments)) {
            result << smatchComments.prefix(); // Все начало строки, до первого сопоставления регулярки
            program = smatchComments.suffix(); // Оставшаяся строка, которую regex_search еще не проверил
        }
    }
    result << program; // Дописываем остольную часть строки, в которой regex_search ничего не находит

    program = result.str();
    result.clear();

    if (regex_search(program, regexSpaces)) {
        program = regex_replace(program, regexSpaces, "$1"); // $n - ссылка на n-ую скобочную группу. Т.е. здесь меняем на (\W) если она есть
    }

    return program;
}


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
    cout << "Stage 1. Input program code:" << endl << program << endl;
    cout << "===========================================================" << endl;

    program = normalizeProgram(program);
    cout << "Stage 2. Normalized program code:" << endl << program << endl;
    cout << "===========================================================" << endl;


    return 0;
}
