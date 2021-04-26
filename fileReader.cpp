// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <cctype>
#include <algorithm>
#include <map>
using namespace std;

/* File:     
 *     main.cpp
 *
 * Purpose:
 *
 * Input:
 *
 * Output:
 *     
 * Compile:  
 *    c++ fileReader.cpp -o main.run
 * 
 * Usage:
 *   main.run <'regular expresion'> <'string'>
 *
 */


const string flag = "    //INSERT EXPRESSIONS";
const string keywords[] = {"COMPILER", "CHARACTERS", "KEYWORDS", "TOKENS", "IGNORE", "PRODUCTIONS", "END"};
const string keywordsFlag = {"EXCEPT KEYWORDS"} ;
const string tokenOperator = "()|*+";

int isKeyWord(string line, int cont, const string keywords[]){
  //change the 6
  for (int i = 0; i < 6; i++){
    if (line.substr(cont,keywords[i].size()).compare(keywords[i]) == 0){
      return i;
    }
  }
  return -1;
}

bool isString(string line){
  return (line[0] == '"'  && line[line.size() - 1] == '"');
}

bool isChar(string line){
  return (line[0] == '\''  && line[line.size() - 1] == '\'');
}

string sustituteRange(string line){
  size_t firstPosition = line.find("$");
  if (firstPosition != string::npos) {
    int num1 = std::stoi(line.substr(1, firstPosition - 2));
    int num2 = std::stoi(line.substr(firstPosition + 2, line.size() - firstPosition - 3));
    line = "";
    for (int i = num1; i < num2 + 1; i++ ){
        line = line + "'" + to_string(i) + "'";
      }
  }
  return line;
}

string replaceString(string line, string patron, string newPatron, bool considerChains){
  bool readingString = false;
  bool readingChar = false;
  string result = "";
  int i = 0;
  while (i < line.size()){
    if (readingString){
      if (line[i] == '"'){
        readingString = false;
      }
      result = result + line[i];
    }
    else if (readingChar){
      if (line[i] == '\''){
        readingChar= false;
      }
      result = result + line[i];
    } else if ((line.size() - i >= patron.size() ) && line.substr(i,patron.size()).compare(patron) == 0){
      result = result + newPatron;
      i = i + patron.size() - 1;
    }
    else if (line[i] == '"' && !considerChains){
      readingString = true;
      result = result + line[i];
    }
    else if (line[i] == '\'' && !considerChains){
      readingString = true;
      result = result + line[i];
    } else {
      result = result + line[i];
    }
    i++;
  }
  return result;
}

string passToParentesis(string line){
  string result = "";
  int p = 0;
  for (int i = 0; i < line.size(); i++){
    if (line[i] == '\''){
      if (p % 2 == 0){
        result = result + "(";
      } else {
        result = result + ")";
      }
      p++;
    } else {
      result = result + line[i];
    }
  }
  return result;
}

string sustituteChar(string line){
  size_t firstPosition = line.find("CHR(");
  while (firstPosition != string::npos) {
    size_t lastPosition = line.find(")");
    string snum = line.substr(firstPosition + 4 , lastPosition - (firstPosition + 4));
    line = line.substr(0, firstPosition) + "'" + snum + "'" + line.substr(lastPosition + 1, line.size() - lastPosition + 1 );
    firstPosition = line.find("CHR(");
  }
  return line;
}

string lettersToNums(string line){
  bool readingString = false;
  bool readingChar = false;
  string result = "";
  int i = 0;
  /*
  cout << "letters to numbers" << endl;
  cout << "line: " << line << endl;*/
  while (i < line.size()){
    if (readingString){
      if (line[i] == '"'){
        readingString = false;
      } else {
        //cout << to_string( (int)line[i] ) << endl;
        result = result + '\'' + to_string( (int)line[i] ) + '\'';
      }
    } else if (readingChar){
      if (line[i] == '\''){
        readingChar= false;
      } else {
        //cout << to_string( (int)line[i] ) << endl;
        result = result + '\'' + to_string( (int)line[i] ) + '\'';
      }
    } else if (line[i] == '"'){
      readingString = true;
    }
    else if (line[i] == '\''){
      readingChar = true;
    } else {
      result = result + line[i];
    }
    i++;
  }
  return result;
}

string join(string s1, string s2, map<string, string>& references){
  //cout << "sum s1:" << s1 << " s2:" << s2 <<  " result:";
  if (!isChar(s1)){
    s1 = references[s1];
  }
  if (!isChar(s2)){
    s2 = references[s2];
  }
  string result;
  bool readingNumber = false;
  string number;
  for (int i = 0; i < s2.size(); i++){
    if (s2[i] == '\'') {
      if (readingNumber){
        readingNumber = false;
        if (s1.find(number) == string::npos){
          s1 = s1 + "'" + number + "'";
        }
        number = "";
      } else {
        readingNumber = true;
      }
    } else {
      number = number + s2[i];
    }
  }
  //cout << s1 << endl;
  return s1;
}

string remove(string s1, string s2, map<string, string>& references){
  //cout << "res s1:" << s1 << " s2:" << s2 <<  " result:";
  if (!isChar(s1)){
    s1 = references[s1];
  }
  if (!isChar(s2)){
    s2 = references[s2];
  }
  string result;
  bool readingNumber = false;
  string number;
  for (int i = 0; i < s2.size(); i++){
    if (s2[i] == '\'') {
      if (readingNumber){
        readingNumber = false;
        s1 = replaceString(s1, '\'' + number + '\'', "", true);
        number = "";
      } else {
        readingNumber = true;
      }
    } else {
      number = number + s2[i];
    }
  }
  //cout << s1 << endl;
  return s1;
}

string format(string term){
  return sustituteRange(sustituteChar(lettersToNums(term)));
}

string replaceTokens(string line, map<string,string>& map){
  //cout << line << endl;
  string acum = "";
  string result = "";
  for (int i = 0; i < line.size(); i++){
    if (tokenOperator.find(line[i]) == string::npos){
      acum = acum + line[i];
    } else {
      //check the acum content
      if (acum.size() > 0){
        if (map[acum].size() > 0){
          result = result  + map[acum];
        } else {
          result = result  + acum;
        }
        acum = "";
      }
      //operators
      result = result + line[i];
    }
  }
  if (acum.size() > 0){
    if (map[acum].size() > 0){
      result = result  + map[acum];
    } else {
      result = result  + acum;
    }
  }
  return result;
}

void makeUnique(map<string,string>& savedCharacters){
  for(auto it = savedCharacters.cbegin(); it != savedCharacters.cend(); ++it){
    string acum = "";
    set<string> readNumbers;
    for (int i = 1; i < it->second.size() - 2; i++){
      if (it->second[i] == '|'){
        if (acum .size() > 0){
          readNumbers.insert(acum);
          acum = "";
        } 
      } else {
        acum = acum + it->second[i];
      }
    }
    readNumbers.insert(acum);
    for (auto const &h: readNumbers) {
      //cout << h << endl;
    }

  }
};

void passToOrs(map<string,string>& savedCharacters){
  for(auto it = savedCharacters.cbegin(); it != savedCharacters.cend(); ++it){
    savedCharacters[it->first] = "(" + replaceString(it->second.substr(1, it->second.size() - 2), "''", "|", true ) + ")";
  }
};

void replace(map<string,string>& savedCharacters, map<string,string>& savedTokens, vector<string>& tokenIds){
  for(int i = 0; i < tokenIds.size(); i++){
    bool isReady = false;
    while (!isReady) {
      string resultString = replaceTokens(savedTokens[tokenIds[i]], savedTokens);
      isReady = equal( resultString.begin(), resultString.end(), savedTokens[tokenIds[i]].begin(), savedTokens[tokenIds[i]].end());
      if (!isReady){
        savedTokens[tokenIds[i]] = resultString;
      }
    }
    isReady = false;
    while (!isReady) {
      string resultString = replaceTokens(savedTokens[tokenIds[i]], savedCharacters);
      isReady = equal( resultString.begin(), resultString.end(), savedTokens[tokenIds[i]].begin(), savedTokens[tokenIds[i]].end());
      if (!isReady){
        savedTokens[tokenIds[i]] = resultString;
      }
    }
  }
}

int generateStream(string& file){
    string filename("cocolEjemplo.txt");
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

void fillmaps(map<string,string>& savedKeywords, map<string,string>& savedCharacters, map<string,string>& savedTokens, 
  vector<string>& whiteSpaces, map<string,bool>& exceptTokens, vector<string>& tokenIds){
  string line;
  
  int currentKeyword = 0;
  bool readingString = false;
  bool readingChar = false;
  bool range = true;
  bool exceptKeyWordsFlag = false;

  vector<string> terms;
  vector<char> operands;

  generateStream(line);
  line = replaceString(line, "..", "$", false);
  int cont = 0;
  string acum = "";
  while (line.size() > cont){
  // check for key word
    int tempCurrentKeyword = isKeyWord(line, cont, keywords);
    if (tempCurrentKeyword > 0){
      //check if the temp is lower than the actual, if that is the case, return errror
      currentKeyword = tempCurrentKeyword;
      cont = cont + keywords[currentKeyword].size() - 1;
      terms.clear();
      operands.clear();
      acum= "";
    } else if (readingChar && line[cont] != '\''){
      acum = acum + line[cont];
    } else if (readingString && line[cont] != '"'){
      acum = acum + line[cont];
    } else if (line.substr(cont,keywordsFlag.size()).compare(keywordsFlag) == 0){
      if (acum.size() > 0){
        if (currentKeyword == 1 ||currentKeyword == 4){
          acum = format(acum);
        }
        terms.push_back(acum);
        acum = "";
      }
      exceptKeyWordsFlag = true;
      cont = cont + keywordsFlag.size() - 1;
    } else if (line[cont] == '\''){
      if (readingChar){
        readingChar = false;
      } else {
        readingChar = true;
      }
      acum = acum + line[cont];
    } else if (line[cont] == '"'){
      if (readingString){
        readingString = false;
      } else {
        readingString = true;
      }
      acum = acum + line[cont];
    } else if (line[cont] == '='){
      if (acum.size() > 0){
        if (currentKeyword == 1 ||currentKeyword == 4){
          acum = format(acum);
        }
        terms.push_back(acum);
        acum = "";
      }
    } else if (line[cont] == '+' || line[cont] == '-'){
      if (acum.size() > 0){
        if (currentKeyword == 1 ||currentKeyword == 4){
          acum = format(acum);
        }
        terms.push_back(acum);
        acum = "";
      }
      operands.push_back(line[cont]);
      acum = "";
    } else if (line[cont] == '.'){
      if (acum.size() > 0){
        if (currentKeyword == 1 ||currentKeyword == 4){
          acum = format(acum);
        }
        terms.push_back(acum);
        acum = "";
      }
      switch(currentKeyword) {
          case 1:
          case 4:
            while (operands.size() > 0){
               if (operands[operands.size() - 1] == '+'){
                terms[terms.size() - 2] = join( (terms[terms.size() - 2]), (terms[terms.size() - 1]), savedCharacters);
                terms.pop_back();
                operands.pop_back();
              } else if (operands[operands.size() - 1] == '-'){
                terms[terms.size() - 2] = remove( (terms[terms.size() - 2]), (terms[terms.size() - 1]), savedCharacters);
                terms.pop_back();
                operands.pop_back();
              }
              else {
                //error
              }
            }
            //cout << terms[terms.size() - 2] << endl;
            if (currentKeyword == 1) {
              if (terms.size() == 2){
                //cout << terms[0] << " " << terms[1] << endl;
                savedCharacters[terms[0]] = (terms[1]);
                terms.clear();
                operands.clear();
              } else {
                cout << "Error 1" << endl;
              } 
            } else {
              if (terms.size() == 1){
                whiteSpaces.push_back((terms[0]));
                terms.clear();
                operands.clear();
              }  else {
                cout << "Error" << endl;
              } 
            }
            break;
          case 2:
            if (terms.size() == 2){
              savedKeywords[terms[0]] = terms[1];
              terms.clear();
              operands.clear();
            } 
            break;
          case 3:
            if (terms.size() == 2){
              terms[1] = format(terms[1]);
              terms[1] = passToParentesis(replaceString(terms[1], "''", ")(", true));
              size_t firstPosition = terms[1].find("{");
              //cout << terms[1] << endl;
              if (firstPosition != string::npos) {
                terms[1] = replaceString(terms[1], "{", "(", false);
                terms[1] = replaceString(terms[1], "}", ")*", false);
              }
              firstPosition = terms[1].find("[");
              if (firstPosition != string::npos) {
                terms[1] = replaceString(terms[1], "[", "(", false);
                terms[1] = replaceString(terms[1], "]", ")+", false);
              }
              savedTokens[terms[0]] = terms[1];
              tokenIds.push_back(terms[0]);
              exceptTokens[terms[0]] = exceptKeyWordsFlag;
              terms.clear();
              operands.clear();
            } else {
              //Error
            }
            break;
      }
    } else if (isspace(line[cont])){
      //terms.push_back(acum);
      //acum = "";
    } else {
      acum = acum + line[cont];
      //cout << line[cont];
    }
    cont++;
  }
}

string mergeWhiteSpaces(vector<string>& whiteSpaces){
  string result = "";
  for(int i = 0; i < whiteSpaces.size(); i++){
    result = result + whiteSpaces[i];
  }
  return result; 
}

int main () {
  map<string,string> savedKeywords;
  map<string,string> savedCharacters;
  map<string,string> savedTokens;
  map<string,bool> exceptTokens;
  vector<string> tokenIds;
  vector<string> whiteSpaces;
  string resultWhiteSpaces;

  fillmaps(savedKeywords, savedCharacters, savedTokens, whiteSpaces, exceptTokens, tokenIds);
  passToOrs(savedCharacters);
  //makeUnique(savedCharacters);
  replace(savedCharacters, savedTokens, tokenIds);
  //
  resultWhiteSpaces = replaceString(mergeWhiteSpaces(whiteSpaces), "''", ",", true);
  //Prints
  for(auto it = savedTokens.cbegin(); it != savedTokens.cend(); ++it){
    if (it->second.size() > 0) {
      cout << it->first << " " << it->second << endl;
      cout << endl;
    }
  }
  for(auto it = savedCharacters.cbegin(); it != savedCharacters.cend(); ++it){
    if (it->second.size() > 0) {
      cout << it->first << " " << it->second << endl;
    }
  }
  for(auto it = savedKeywords.cbegin(); it != savedKeywords.cend(); ++it){
    cout << it->first << " " << it->second << endl;
  }
  cout << resultWhiteSpaces << endl;
  //write file
  string line;
  ifstream basefile;
  basefile.open("scanner.cpp");
  ofstream newfile("main.cpp");
  if (basefile.is_open())
  {
    while ( getline (basefile,line) )
    {
      if (equal(line.begin(), line.end(), flag.begin(), flag.end())){
        string acum = "";
        for(int i = 1; i < resultWhiteSpaces.size() - 1; i++){
          if (resultWhiteSpaces[i] == ','){
            newfile <<  "    whitespaces.insert(" << acum << ");" << endl;
            acum = "";
          } else {
            acum = acum  + resultWhiteSpaces[i];
          }
        }
        newfile <<  "    whitespaces.insert(" << acum << ");" << endl;
        for(auto it = savedTokens.cbegin(); it != savedTokens.cend(); ++it){
          if (it->second.size() > 0) {
            //cout << it->second << endl;
            newfile <<  "    exceptTokens[\"" << it->first << "\"] = "<< exceptTokens[it->first] <<";\n";
            newfile <<  "    expressions.push_back(\"" << it->second <<"\");"<< endl;
            newfile <<  "    expressionsId.push_back(\"" << it->first <<"\");"<< endl;
          }
        }
        //cout << "Siiiii\n";
      } else {
        newfile << line << "\n";
      }
    }
   basefile.close();
   newfile.close();
  } else {
    cout << "Unable to open file";
  }

  return 0;
}