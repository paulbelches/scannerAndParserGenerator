// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <cctype>
#include <algorithm>
using namespace std;

vector<string> split(string chain, string pattern){
  //string acum = "";
  vector<string> result;
  while (1)
  {
    string::size_type found = chain.find(pattern);
    if (found!=std::string::npos) {
      result.push_back(chain.substr(0, found));
      chain = chain.substr(found + pattern.size(), chain.size());
    } else {
      break;
    }
  }
  result.push_back(chain);
  return result;
}

string strip(string s) {
    s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
    return s;
}

int main () {
  string line;
  ifstream myfile;

  string keywords[] = {"COMPILER", "CHARACTERS", "KEYWORDS", "TOKENS", "PRODUCTIONS", "END"};
  int currentKeyword = 0;
  bool readingString = false;
  bool readingChar = false;
  vector<set<char>> characters;

  
  myfile.open("cocolEjemplo.txt");
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      int cont = 0;
      string acum = "";
      vector<string> terms;
      while (line.size() > cont){
        // check for key word
        if (line.substr(cont,keywords[currentKeyword].size()).compare(keywords[currentKeyword]) == 0){
          cout << "clave ";
          cout << line.substr(cont,keywords[currentKeyword].size()) << endl;
          cont = cont + keywords[currentKeyword].size() - 1;
          currentKeyword++;
        } else if (line[cont] == '\''){
          if (readingChar){
            readingChar = false;
          } else {
            readingChar = true;
          }
          acum = acum + line[cont];
        } else if (readingChar){
          acum = acum + line[cont];
        } else if (line[cont] == '"'){
          if (readingString){
            readingString = false;
          } else {
            readingString = true;
          }
          acum = acum + line[cont];
        } else if (readingString){
          acum = acum + line[cont];
        } else if (line[cont] == '='){
          terms.push_back(acum);
          acum = "";
        } else if (line[cont] == '+' || line[cont] == '-'){
          terms.push_back(acum);
          acum = "";
        } else if (line.substr(cont,2).compare("..") == 0){
          terms.push_back(acum);
          acum = "";
          cont++;
        } else if (line[cont] == '.'){
          terms.push_back(acum);
          acum = "";
          switch(currentKeyword) {
              case 2:
                cout << "Character \n";
                for(int i = 0; i < terms.size(); i++){
                  cout << "term No." << i << " " << terms[i] << endl;
                }
                break;
              case 3:
                cout << "Keyword \n";
                for(int i = 0; i < terms.size(); i++){
                  cout << "term No." << i << " " << terms[i] << endl;
                }
                break;
              case 4:
                cout << "Token \n";
                for(int i = 0; i < terms.size(); i++){
                  cout << "term No." << i << " " << terms[i] << endl;
                }
                break;
          }
          terms.clear();
        } else if (isspace(line[cont])){

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

  return 0;
}