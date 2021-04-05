// reading a text file
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main () {
  string line;
  ifstream myfile;
  myfile.open("cocolEjemplo.txt");
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      if (line.size() > 0){

        //Verificar si es una palabra
        //Cambiar estado
        //Sino es estado
        //case para cada estado
      
        cout << line << '\n';
      }
    }
    myfile.close();
  }

  else cout << "Unable to open file"; 

  return 0;
}