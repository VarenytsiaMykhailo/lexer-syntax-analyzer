//
// Created by MIKE on 10.02.2021.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <set>

#include "token.hpp"


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
            program = smatchComments.suffix(); // Оставшаяся часть строки, которую regex_search еще не проверил
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


tuple<vector<token>, set<token>, vector<token>> tokenizeProgram(const string &program) {
    vector<string> lexemes;

    regex regex_tokens(R"(<=|>=|==|!=|[(){};=+\-*/<>=!,]|[^ \n\t(){};=+\-*/<>=!,]+)");

    sregex_iterator sregex_iterator(program.begin(), program.end(), regex_tokens);
    for (auto it = sregex_iterator, sregex_iterator_end = std::sregex_iterator(); it != sregex_iterator_end; it++) {
        lexemes.emplace_back(it->str());
    }

    map<string, token> tokens_instances;
    tokens_instances.emplace("+", token(tokenType::plus, "+"));
    tokens_instances.emplace("-", token(tokenType::minus, "-"));
    tokens_instances.emplace("*", token(tokenType::multiply, "*"));
    tokens_instances.emplace("/", token(tokenType::divide, "/"));
    tokens_instances.emplace("=", token(tokenType::assignment, "="));
    tokens_instances.emplace("==", token(tokenType::equal, "=="));
    tokens_instances.emplace("!=", token(tokenType::not_equal, "!="));
    tokens_instances.emplace(">", token(tokenType::greater, ">"));
    tokens_instances.emplace("<", token(tokenType::less, "<"));
    tokens_instances.emplace(">=", token(tokenType::greater_or_equal, ">="));
    tokens_instances.emplace("<=", token(tokenType::less_or_equal, "<="));
    tokens_instances.emplace(";", token(tokenType::semicolon, ";"));
    tokens_instances.emplace("(", token(tokenType::bracket_left, "("));
    tokens_instances.emplace(")", token(tokenType::bracket_right, ")"));
    tokens_instances.emplace("{", token(tokenType::bracket_curly_left, "{"));
    tokens_instances.emplace("}", token(tokenType::bracket_curly_right, "}"));
    tokens_instances.emplace("if", token(tokenType::if_, "if"));
    tokens_instances.emplace("else", token(tokenType::else_, "else"));
    tokens_instances.emplace("while", token(tokenType::while_, "while"));
    tokens_instances.emplace("for", token(tokenType::for_, "for"));
    tokens_instances.emplace("do", token(tokenType::do_, "do"));
    tokens_instances.emplace("return", token(tokenType::return_, "return"));
    tokens_instances.emplace(",", token(tokenType::comma, ","));

    set<string> tokens_datatypes{"bool",
                                  "signed char", "unsigned char", "char",
                                  "signed short", "unsigned short", "short",
                                  "signed int", "unsigned int", "int",
                                  "signed long", "unsigned long", "long",
                                  "float", "double", "void"
    };

    regex regex_number(R"(^[0-9]+$)");
    regex regex_id(R"(^[a-zA-Z_][a-zA-Z0-9_]*$)");

    vector<token> tokens;
    set<token> ids;
    vector<token> numbers;

    for (auto it = lexemes.begin(); it != lexemes.end(); it++) {
        string lexeme = *it;
        auto tokens_instances_it = tokens_instances.find(lexeme);
        if (tokens_instances_it != tokens_instances.end()) {
            tokens.emplace_back(tokens_instances_it->second);
            continue;
        }
        if (lexeme == "signed" || lexeme == "unsigned") {
            it++;
            lexeme += ' ' + *it;
        }
        if (tokens_datatypes.find(lexeme) != tokens_datatypes.end()) {
            tokens.emplace_back(token(datatype, lexeme));
            continue;
        }
        if (regex_match(lexeme, regex_number)) {
            token numberToken = token(number, lexeme);
            tokens.emplace_back(numberToken);
            numbers.emplace_back(numberToken);
            continue;
        }
        if (regex_match(lexeme, regex_id)) {
            token idToken = token(id, lexeme);
            tokens.emplace_back(idToken);
            ids.emplace(idToken);
            continue;
        }
        throw "Can not understand the lexeme: " + lexeme;
    }

    return make_tuple(tokens, ids, numbers);
}


string parse(const vector<token>&);
bool check_expected(const vector<token>&, size_t, const tokenType&);
string getErrorMessage(const vector<token>&, size_t, string);

pair<string, size_t> rule_s(const vector<token>&, size_t);
pair<string, size_t> rule_i(const vector<token>&, size_t);
pair<string, size_t> rule_n(const vector<token>&, size_t);
pair<string, size_t> rule_c(const vector<token>&, size_t);
pair<string, size_t> subrule_c_1(const vector<token>&, size_t);
pair<string, size_t> subrule_c_2(const vector<token>&, size_t);
pair<string, size_t> subrule_c_3(const vector<token>&, size_t);
pair<string, size_t> subrule_c_4(const vector<token>&, size_t);
pair<string, size_t> subrule_c_5(const vector<token>&, size_t);
pair<string, size_t> subrule_c_6(const vector<token>&, size_t);
pair<string, size_t> subrule_c_7(const vector<token> &, size_t);
pair<string, size_t> subrule_c_8(const vector<token>&, size_t);
pair<string, size_t> rule_c1(const vector<token>&, size_t);
pair<string, size_t> subrule_c1_1(const vector<token>&, size_t);
pair<string, size_t> subrule_c1_2(const vector<token>&, size_t);
pair<string, size_t> subrule_c1_3(const vector<token>&, size_t);
pair<string, size_t> rule_e(const vector<token>&, size_t);
pair<string, size_t> subrule_e_1(const vector<token>&, size_t);


string parse(const vector<token> &tokens) {
    auto result = rule_s(tokens, 0);
    if (!result.first.empty()) {
        return result.first + " at index = " + to_string(result.second);
    }
    if (result.second < tokens.size()) {
        return "Error. Incorrect token at the end" + tokens[result.second].get_value();
    }
    return "Correct program";
}

bool check_expected(const vector<token> &tokens, size_t index, const tokenType &expected_type) {
    return index < tokens.size() && tokens[index].get_type() == expected_type;
}

string getErrorMessage(const vector<token> &tokens, size_t index, string expectedToken) {
    string errorMessage = "Error. " + (index < tokens.size() ? "Incorrect token " + tokens[index].get_value() + ", expected token " + expectedToken : "Wrong end of program");
    return errorMessage;
}

pair<string, size_t> rule_s(const vector<token> &tokens, size_t index) {
    if (!check_expected(tokens, index, datatype)) {
        return make_pair("Error. " + (index < tokens.size() ? "Incorrect token " + tokens[index].get_value() + ", expected token <T>" : "Wrong end of program"), index);
    }
    index++;

    pair<string, size_t> resultOfSubRules;

    resultOfSubRules = rule_i(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, bracket_left)) {
        return make_pair("Error. " + (index < tokens.size() ? "Incorrect token " + tokens[index].get_value() + ", expected token <(>" : "Wrong end of program"), index);
    }
    index++;

    if (!check_expected(tokens, index, bracket_right)) {
        return make_pair("Error. " + (index < tokens.size() ? "Incorrect token " + tokens[index].get_value() + ", expected token <)>" : "Wrong end of program"), index);
    }
    index++;

    if (!check_expected(tokens, index, bracket_curly_left)) {
        return make_pair("Error. " + (index < tokens.size() ? "Incorrect token " + tokens[index].get_value() + ", expected token <{>" : "Wrong end of program"), index);
    }
    index++;

    resultOfSubRules = rule_c(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, bracket_curly_right)) {
        return make_pair("Error. " + (index < tokens.size() ? "Incorrect token " + tokens[index].get_value() + ", expected token <}>" : "Wrong end of program"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> rule_i(const vector<token> &tokens, size_t index) {
    if (!check_expected(tokens, index, id)) {
        return make_pair("Error. " + (index < tokens.size() ? "Incorrect token " + tokens[index].get_value() + ", expected token <ID, n>" : "Wrong end of program"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> rule_n(const vector<token> &tokens, size_t index) {
    if (!check_expected(tokens, index, number)) {
        return make_pair("Error. " + (index < tokens.size() ? "Incorrect token " + tokens[index].get_value() + ", expected token <D, n>" : "Wrong end of program"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> rule_c(const vector<token> &tokens, size_t index) {
    do {
        auto resultOfSubRules = subrule_c_1(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        resultOfSubRules = subrule_c_2(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        resultOfSubRules = subrule_c_3(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        resultOfSubRules = subrule_c_4(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        resultOfSubRules = subrule_c_5(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        resultOfSubRules = subrule_c_6(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        resultOfSubRules = subrule_c_7(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        resultOfSubRules = subrule_c_8(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        break;
    } while (true);

    return make_pair("", index);
}


pair<string, size_t> subrule_c_1(const vector<token> &tokens, size_t index) {
    if (!check_expected(tokens, index, datatype)) {
        return make_pair("Error. " + (index < tokens.size() ? "Incorrect token " + tokens[index].get_value() + ", expected token <T>" : "Wrong end of program"), index);
    }
    index++;

    auto resultOfSubRules = rule_c1(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, semicolon)) {
        return make_pair("Error. " + (index < tokens.size() ? "Incorrect token " + tokens[index].get_value() + ", expected token <;>" : "Wrong end of program"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> subrule_c_2(const vector<token> &tokens, size_t index) {
    auto resultOfSubRules = rule_c1(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, semicolon)) {
        return make_pair("Error. " + (index < tokens.size() ? "Incorrect token " + tokens[index].get_value() + ", expected token <;>" : "Wrong end of program"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> subrule_c_3(const vector<token> &tokens, size_t index) {
    if (!check_expected(tokens, index, for_)) {
        return make_pair(getErrorMessage(tokens, index, "<for>"), index);
    }
    index++;

    if (!check_expected(tokens, index, bracket_left)) {
        return make_pair(getErrorMessage(tokens, index, "<(>"), index);
    }
    index++;

    auto resultOfSubRules = rule_c1(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, semicolon)) {
        return make_pair(getErrorMessage(tokens, index, "<;>"), index);
    }
    index++;

    resultOfSubRules = rule_c2(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, semicolon)) {
        return make_pair(getErrorMessage(tokens, index, "<;>"), index);
    }
    index++;

    resultOfSubRules = rule_c1(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, bracket_right)) {
        return make_pair(getErrorMessage(tokens, index, "<)>"), index);
    }
    index++;

    if (!check_expected(tokens, index, bracket_curly_left)) {
        return make_pair(getErrorMessage(tokens, index, "<{>"), index);
    }
    index++;

    resultOfSubRules = rule_c(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, bracket_curly_right)) {
        return make_pair(getErrorMessage(tokens, index, "<}>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> subrule_c_4(const vector<token> &tokens, size_t index) {
    if (!check_expected(tokens, index, while_)) {
        return make_pair(getErrorMessage(tokens, index, "<while>"), index);
    }
    index++;

    if (!check_expected(tokens, index, bracket_left)) {
        return make_pair(getErrorMessage(tokens, index, "<(>"), index);
    }
    index++;

    auto resultOfSubRules = rule_c2(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, bracket_right)) {
        return make_pair(getErrorMessage(tokens, index, "<)>"), index);
    }
    index++;

    if (!check_expected(tokens, index, bracket_curly_left)) {
        return make_pair(getErrorMessage(tokens, index, "<{>"), index);
    }
    index++;

    resultOfSubRules = rule_c(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, bracket_curly_right)) {
        return make_pair(getErrorMessage(tokens, index, "<}>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> subrule_c_5(const vector<token> &tokens, size_t index) {
    if (!check_expected(tokens, index, do_)) {
        return make_pair(getErrorMessage(tokens, index, "<do>"), index);
    }
    index++;

    if (!check_expected(tokens, index, bracket_curly_left)) {
        return make_pair(getErrorMessage(tokens, index, "<{>"), index);
    }
    index++;

    auto resultOfSubRules = rule_c(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, bracket_curly_right)) {
        return make_pair(getErrorMessage(tokens, index, "<}>"), index);
    }
    index++;

    if (!check_expected(tokens, index, while_)) {
        return make_pair(getErrorMessage(tokens, index, "<while>"), index);
    }
    index++;

    if (!check_expected(tokens, index, bracket_left)) {
        return make_pair(getErrorMessage(tokens, index, "<(>"), index);
    }
    index++;

    resultOfSubRules = rule_c2(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, bracket_right)) {
        return make_pair(getErrorMessage(tokens, index, "<)>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> subrule_c_6(const vector<token> &tokens, size_t index) {
    if (!check_expected(tokens, index, if_)) {
        return make_pair(getErrorMessage(tokens, index, "<if>"), index);
    }
    index++;

    if (!check_expected(tokens, index, bracket_left)) {
        return make_pair(getErrorMessage(tokens, index, "<(>"), index);
    }
    index++;

    auto resultOfSubRules = rule_c2(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, bracket_right)) {
        return make_pair(getErrorMessage(tokens, index, "<)>"), index);
    }
    index++;

    if (!check_expected(tokens, index, bracket_curly_left)) {
        return make_pair(getErrorMessage(tokens, index, "<{>"), index);
    }
    index++;

    resultOfSubRules = rule_c(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, bracket_curly_right)) {
        return make_pair(getErrorMessage(tokens, index, "<}>"), index);
    }
    index++;

    resultOfSubRules = rule_c3(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}


pair<string, size_t> subrule_c_7(const vector<token> &tokens, size_t index) {
    if (!check_expected(tokens, index, return_)) {
        return make_pair(getErrorMessage(tokens, index, "<return>"), index);
    }
    index++;

    auto resultOfSubRules = rule_e(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, semicolon)) {
        return make_pair(getErrorMessage(tokens, index, "<;>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> subrule_c_8(const vector<token> &tokens, size_t index) {
    if (!check_expected(tokens, index, semicolon)) {
        return make_pair(getErrorMessage(tokens, index, "<;>"), index);
    }
    index++;

    return make_pair("", index);
}

pair<string, size_t> rule_c1(const vector<token> &tokens, size_t index) {
    auto resultOfSubRules = subrule_c1_1(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    resultOfSubRules = subrule_c1_2(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    resultOfSubRules = subrule_c1_3(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    resultOfSubRules = rule_i(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);

}


pair<string, size_t> subrule_c1_1(const vector<token> &tokens, size_t index) {
    auto resultOfSubRules = rule_i(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, comma)) {
        return make_pair( getErrorMessage(tokens, index, "<,>"), index);
    }
    index++;

    resultOfSubRules = rule_c1(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}


pair<string, size_t> subrule_c1_2(const vector<token> &tokens, size_t index) {
    auto resultOfSubRules = rule_i(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, assignment)) {
        return make_pair(getErrorMessage(tokens, index, "<=>"), index);
    }
    index++;

    resultOfSubRules = rule_e(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, comma)) {
        return make_pair( getErrorMessage(tokens, index, "<,>"), index);
    }
    index++;

    resultOfSubRules = rule_c1(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}


pair<string, size_t> subrule_c1_3(const vector<token> &tokens, size_t index) {
    auto resultOfSubRules = rule_i(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, assignment)) {
        return make_pair(getErrorMessage(tokens, index, "<=>"), index);
    }
    index++;

    resultOfSubRules = rule_e(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}


pair<string, size_t> rule_e(const vector<token> &tokens, size_t index) {
    auto resultOfSubRules = subrule_e_1(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    resultOfSubRules = subrule_e_2(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    resultOfSubRules = subrule_e_3(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    resultOfSubRules = rule_e1(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}


pair<string, size_t> subrule_e_1(const vector<token> &tokens, size_t index) {
    if (!check_expected(tokens, index, bracket_left)) {
        return make_pair(getErrorMessage(tokens, index, "<(>"), index);
    }
    index++;

    auto resultOfSubRules = rule_e(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!check_expected(tokens, index, bracket_right)) {
        return make_pair(getErrorMessage(tokens, index, "<)>"), index);
    }
    index++;

    resultOfSubRules = rule_e2(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    resultOfSubRules = rule_e(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
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
    cout << "Input program code:" << endl << program << endl;
    cout << "===========================================================" << endl;

    program = normalizeProgram(program);
    cout << "Stage 1. Normalized program code:" << endl << program << endl;
    cout << "===========================================================" << endl;

    tuple<vector<token>, set<token>, vector<token>> tokens;

    try {
        tokens = tokenizeProgram(program);
    } catch (const string &exception) {
        cerr << exception << endl;
        exit(1);
    }

    cout << "Stage 2. Lexer:" << endl;
    for (const auto &token : get<0>(tokens)) {
        cout << "<" << token.get_value() << "> ";
    }
    cout << endl;
    cout << "===========================================================" << endl;

    cout << "Stage 3. Tables:" << endl;
    cout << "Identifiers:" << endl;
    for (const auto &id : get<1>(tokens)) {
        cout << id.get_value() << ", ";
    }
    cout << endl;
    cout << "Numbers:" << endl;
    for (const auto &number : get<2>(tokens)) {
        cout << number.get_value() << ", ";
    }
    cout << endl;
    cout << "===========================================================" << endl;

    cout << "Stage 4. Errors:" << endl;

    cout << parse(std::get<0>(tokens));

    return 0;
}





token::token(tokenType type, std::string value): type(type), value(std::move(value)) {
}

bool token::operator< (const token &token1) const {
    return value < token1.get_value();
}

[[nodiscard]] const tokenType &token::get_type() const {
    return this->type;
}

[[nodiscard]] const std::string &token::get_value() const {
    return this->value;
}

