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

const string keywords[] = {"COMPILER", "CHARACTERS", "KEYWORDS", "TOKENS", "IGNORE", "PRODUCTIONS", "END"};


int isKeyWord(string line, int cont, const string keywords[]){
  //change the 6
  for (int i = 0; i < 6; i++){
    if (line.substr(cont,keywords[i].size()).compare(keywords[i]) == 0){
      return i;
    }
  }
  return -1;
}

string replaceTerm(string line, char patron, string newPatron){
  bool readingString = false;
  bool readingChar = false;
  string result = "";
  for (int i = 0; i < line.size(); i++){
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
    }
    else if (line[i] == '"'){
      readingString = true;
      result = result + line[i];
    }
    else if (line[i] == '\''){
      readingString = true;
      result = result + line[i];
    }
    else if (line[i] == patron){
      result = result + newPatron;
    } else {
      result = result + line[i];
    }
  }
  return result;
}


//Add Validation
string sustituteChar(string line){
  size_t firstPosition = line.find("CHR(");
  while (firstPosition != string::npos) {
    size_t lastPosition = line.find(")");
    string snum = line.substr(firstPosition + 4 , lastPosition - (firstPosition + 4));
    char character = static_cast<char>(stoi( snum ));
    string cString(1, character);
    line = line.substr(0, firstPosition) + "'" + cString + "'" + line.substr(lastPosition + 1, line.size() - lastPosition + 1 );
    firstPosition = line.find("CHR(");
  }
  return line;
}

//Add Validation
string sustituteRange(string line){
  size_t firstPosition = line.find("..");
  while (firstPosition != string::npos) {
    //cout << line << " " << line[firstPosition - 2] << line[firstPosition + 3] <<  endl;
    if (line[firstPosition - 2] < line[firstPosition + 3]) {
      string range = "";
      for (int i = line[firstPosition - 2]; i < line[firstPosition + 3] + 1; i++ ){
        char character = static_cast<char>(i);
        string cString(1, character);
        range = range + cString;
      }
      line = line.substr(0, firstPosition - 3) + "\"" + range + "\"" +  line.substr(firstPosition + 5, line.size() - firstPosition + 5);
    }
    firstPosition = line.find("..");
  }
  return line;
}

bool isString(string line){
  return (line[0] == '"'  && line[line.size() - 1] == '"');
}

bool isChar(string line){
  return (line[0] == '\''  && line[line.size() - 1] == '\'');
}

string formatToString(string term, map<string, string>& references){
  if (isChar(term)){
    return "\"" + term.substr(1, term.size() - 2) + "\"" ;
  } else if (!isString(term)){
    return references[term];
  } else {
    return term;
  }
}

string join(string s1, string s2, map<string, string>& references){
  s1 = formatToString(s1, references);
  s2 = formatToString(s2, references);
  return s1.substr(0, s1.size() - 1) + s2.substr(1, s2.size() - 1);
}

string remove(string s1, string s2, map<string, string>& references){
  s1 = formatToString(s1, references);
  s1 = s1.substr(1, s1.size()-2);
  s2 = formatToString(s2, references);
  for (int i = 1; i < s2.size() - 1; i++){
    size_t firstPosition = s1.find(s2[i]);
    while (firstPosition != string::npos) {
      s1.replace(firstPosition,1,"");
      firstPosition = s1.find(s2[i]);
    }
  }
  return "\"" + s1 + "\"";
}

int main () {
  string line;
  ifstream myfile;

  map<string,string> savedKeywords;
  map<string,string> savedCharacters;
  map<string,string> savedTokens;
  vector<string> whiteSpaces;

  int currentKeyword = 0;
  bool readingString = false;
  bool readingChar = false;
  bool range = true;
  

  
  myfile.open("cocolEjemplo.txt");
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      //Remove "CHR" and ".."
      line = sustituteRange(sustituteChar(line));
      int cont = 0;
      string acum = "";

      vector<string> terms;
      vector<char> operands;
      //cout << line << endl;
      while (line.size() > cont){
        // check for key word
        int tempCurrentKeyword = isKeyWord(line, cont, keywords);
        if (tempCurrentKeyword > 0){
          //check if the temp is lower than the actual, if that is the case, return errror
          currentKeyword = tempCurrentKeyword;
          cont = cont + keywords[currentKeyword].size() - 1;
          //currentKeyword++;
        } else if (readingChar && line[cont] != '\''){
          acum = acum + line[cont];
        } else if (readingString && line[cont] != '"'){
          acum = acum + line[cont];
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
          terms.push_back(acum);
          acum = "";
        } else if (line[cont] == '+' || line[cont] == '-' || line[cont] == '|'){
          terms.push_back(acum);
          operands.push_back(line[cont]);
          acum = "";
        } else if (line[cont] == '.'){
          terms.push_back(acum);
          acum = "";
          switch(currentKeyword) {
              case 1:
              case 4:
                while (operands.size() > 0){
                  if (operands[operands.size() - 1] == '+'){
                    terms[terms.size() - 2] = join(terms[terms.size() - 2], terms[terms.size() - 1], savedCharacters);
                    terms.pop_back();
                    operands.pop_back();
                  } else if (operands[operands.size() - 1] == '-'){
                    terms[terms.size() - 2] = remove(terms[terms.size() - 2], terms[terms.size() - 1], savedCharacters);
                    terms.pop_back();
                    operands.pop_back();
                  }
                  else {
                    //error
                  }
                }
                if (currentKeyword == 1) {
                  if (terms.size() == 2){
                    savedCharacters[terms[0]] = terms[1];
                  } else {
                    cout << "Error" << endl;
                  } 
                } else {
                  if (terms.size() == 1){
                    whiteSpaces.push_back(terms[0]);
                  }  else {
                    cout << "Error" << endl;
                  } 
                }
                break;
              case 2:
                if (terms.size() == 2){
                  savedKeywords[terms[0]] = terms[1];
                } 
                break;
              case 3:
                while (operands.size() > 0){
                  if (operands[operands.size() - 1] == '|'){
                    terms[terms.size() - 2] =  "(" + terms[terms.size() - 2] + ")|(" + terms[terms.size() - 1] + ")";
                    terms.pop_back();
                    operands.pop_back();
                  } else {
                    //error
                  }
                }
                if (terms.size() == 2){
                  size_t firstPosition = terms[1].find("{");
                  cout << terms[1] << endl;
                  if (firstPosition != string::npos) {
                    terms[1] = replaceTerm(terms[1], '{', "(");
                    terms[1] = replaceTerm(terms[1], '}', ")*");
                  }
                  firstPosition = terms[1].find("[");
                  if (firstPosition != string::npos) {
                    terms[1] = replaceTerm(terms[1], '[', "(");
                    terms[1] = replaceTerm(terms[1], ']', ")+");
                  }
                  savedTokens[terms[0]] = terms[1];

                }
                break;
          }
          terms.clear();
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
    myfile.close();
  }
  else cout << "Unable to open file"; 

  /* Print generated maps */
  for(auto it = savedKeywords.cbegin(); it != savedKeywords.cend(); ++it)
  {
      std::cout << it->first << " " << it->second << "\n";
  }
  for(auto it = savedCharacters.cbegin(); it != savedCharacters.cend(); ++it)
  {
      std::cout << it->first << " " << it->second << "\n";
  }
  for(auto it = savedTokens.cbegin(); it != savedTokens.cend(); ++it)
  {
      std::cout << it->first << " " << it->second << "\n";
  }
   for(int i = 0; i < whiteSpaces.size(); i++)
  {
      cout << whiteSpaces[i] << endl;
  }

  return 0;
}