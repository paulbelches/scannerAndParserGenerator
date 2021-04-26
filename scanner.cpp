#include <iostream>
#include <bits/stdc++.h> 
#include <utility> 
#include <fstream>
#include <string>
#include <stack>
#include <queue>
#include <algorithm>
#include <set>
#include <chrono> 

/* File:     
 *     main.cpp
 *
 * Purpose:
 *     Using a input regular expression the code generates 3 automatas.
 *     A non/deterministic finite automata, a deterministic finite automata and 
 *     a deterministic finite automata using the direct method. After the automatas
 *     are generated, using a input string the program simulates the automatas to see whether
 *     they acept the string or not.
 *
 * Input:
 *      the regular expresion, and the string to be review
 *
 * Output:
 *     the process done to create an simulate the automatas, 5 text files with the 
 *      representation of the automatas generated, and the result of the simulation
 *     
 * Compile:  
 *    c++ main.cpp -o main.run
 * 
 * Usage:
 *   main.run <'regular expresion'> <'string'>
 *
 */
using namespace std::chrono; 
using namespace std;

/*---------------------------------------------------------------------
 * Function:      isOperator
 * Purpose:       Determine whether a character is a operator or not
 * In arg:        v, the character that is going to be review
 * Return val:    Whether a character is a operand or not
 */
bool isOperator(char v){
    string ops = "()|.*?+";
    for (int i = 0; i < ops.length(); i = i + 1){
        if (ops[i] == v)
            return 1;
    }
    return 0;
}

/*---------------------------------------------------------------------
 * Function:      prcedence
 * Purpose:       Optain operand precedence
 * In arg:        v, the character that is going to be review
 * Return val:    An integer representing the value of the precedence
 */
int precedence(char v){
    if (v == '?' || v == '+'){
        return 4;
    }
    string ops = "()|.*";
    for (int i = 0; i < ops.length(); i = i + 1){
        if (ops[i] == v)
            return i;
    }
    return -1;
}
/*---------------------------------------------------------------------
 * Function:      correctParentesis
 * Purpose:       Check if the amount of opening and closing parentesis is right 
 * In arg:        expr, the string to be checked
 * Return val:    Whether the amoun is right or not
 */
bool correctParentesis(string expr){
    int openParentesis = 0;
    int closingParentesis = 0;
    for (int i = 0; i < expr.size(); i = i + 1){
        if (closingParentesis > openParentesis) {
            return 0;
        }
        if (expr[i] == '('){
            openParentesis = openParentesis + 1;
        }
        if (expr[i] == ')'){
            closingParentesis = closingParentesis + 1;
        }
    }
    return openParentesis == closingParentesis;
}
/*---------------------------------------------------------------------
 * Class:         Node
 * Purpose:       Represent nodes of a syntax tree
 * Attributes:    
 *      id        The id of the node
 *      data      The character it represents
 *      nullable  Whether is nullable or not
 *      firstpos  Firstpos value
 *      lastpos   LastPos value
 *      followpos Followpos value
 *      left      Child node to the left
 *      right     Child node to the right
 */
class Node{
  public:
  int id;
  string data;
  bool nullable;
  set<Node*> firstpos;
  set<Node*> lastpos;
  set<Node*> followpos;
  Node* left;
  Node* right;
  /*---------------------------------------------------------------------
 * Contructor
 * In arg:        d value of the node, left child, right child
 */
  Node(string d, Node* left, Node* right){
    id = rand() % 5000 + 1;
    data=d;
    this->left=left;
    this->right=right;
  }
};

/*---------------------------------------------------------------------
 * Function:      expand
 * Purpose:       Expand a regular expresion to include characters for concatenation  
 * In arg:        expr, the regular expresion
 * Return val:    The expand expresion
 */
string expand(string expr){
    string r = "";
    int cont = 0;
    bool operand = 0;
    bool readingString = false;
    for (int i = 0; i < expr.length(); i = i + 1){
        if (readingString){
            if (expr[i] == '"'){
                readingString = false;
                cont = 1;
            }
        } else if (expr[i] == '|'){
            cont = 0;
            operand = 0;
        } else if (expr[i] == '('){
            if (cont == 1){
                r = r + '.';
                cont = 0;
                operand = 0;
            }
        } else if (expr[i] == ')' || expr[i] == '*' || expr[i] == '+' || expr[i] == '?'){
            operand = 0;
        } else if (expr[i] == '"'){
            readingString = true;
            cont = cont + 1;
        } else {
            if (!operand) {
                operand = 1;
                cont = cont + 1;
            }
        }
        if (cont == 2){
            r = r + '.' + expr[i];
            cont = 1;
        } else {
            r = r + expr[i];
        }
    }
    return r;
}
/*---------------------------------------------------------------------
 * Function:      nullable
 * Purpose:       Calculate nullable of a node of the syntax tree
 * In arg:        root, a node from the tree
 * Return val:    Whether is nullable or not
 */
bool nullable(Node* root){
    if (root->data[0] == '$'){
        return true;
    } else if (root->data[0] == '|'){
        return nullable(root->left) || nullable(root->right);
    } else if (root->data[0] == '.'){
        return nullable(root->left) && nullable(root->right);
    } else if (root->data[0] == '*'){
        return true;
    } else {
        return false;
    }
}
/*---------------------------------------------------------------------
 * Function:      lastpos
 * Purpose:       Calculate lastpos of a node of the syntax tree
 * In arg:        root, a node from the tree
 * Return val:    The result lastpos set of nodes
 */
set<Node*> lastpos(Node* root){
    set<Node*> result;
    set<Node*> tempSet;
    if (root->data[0] == '|'){
        tempSet = lastpos(root->right);
        result = lastpos(root->left);
        result.insert(tempSet.begin(), tempSet.end());
    } else if (root->data[0] == '.'){
        if (nullable(root->right)){
            tempSet = lastpos(root->right);
            result = lastpos(root->left);
            result.insert(tempSet.begin(), tempSet.end());
        } else {
            result = lastpos(root->right);
        }
    } else if (root->data[0] == '*'){
        result = lastpos(root->left);
    } else {
        result.insert(root);
    } 
    return result;
}
/*---------------------------------------------------------------------
 * Function:      firstpos
 * Purpose:       Calculate firstpos of a node of the syntax tree
 * In arg:        root, a node from the tree
 * Return val:    The result lastpos set of nodes
 */
set<Node*> firstpos(Node* root){
    set<Node*> result;
    set<Node*> tempSet;
    if (root->data[0] == '|'){
        tempSet = firstpos(root->right);
        result = firstpos(root->left);
        result.insert(tempSet.begin(), tempSet.end());
    } else if (root->data[0] == '.'){
        if (nullable(root->left)){
            tempSet = firstpos(root->right);
            result = firstpos(root->left);
            result.insert(tempSet.begin(), tempSet.end());
        } else {
            result = firstpos(root->left);
        }
    } else if (root->data[0] == '*'){
        result = firstpos(root->left);
    } else {
        result.insert(root);
    } 
    return result;
}

/*---------------------------------------------------------------------
 * Function:      fillFunctions
 * Purpose:       Calculate nullable, firstpos and lastpos of a node of the syntac tree
 * In arg:        root, a node from the tree
 * Return val:    -------
 */
void fillFunctions(Node* root){
    root->nullable = nullable(root);
    root->firstpos = firstpos(root);
    root->lastpos = lastpos(root);
    if (root->left != NULL) {
        fillFunctions(root->left);
    }
    if (root->right != NULL) {
        fillFunctions(root->right);
    }
}
/*---------------------------------------------------------------------
 * Function:      getAlphabet
 * Purpose:       Get the alphabet (operands) of a regular expresion
 * In arg:        s, the regular expresion
 * Return val:    -------
 */
set<string> getAlphabet(string s){
    //string nonAlphabet = '.*|()';
    set<string> result;
    string tempValue = "";
    bool readingString = false;
    for (int i=0; i<s.length();i++){
        if ((s[i] != '.' && s[i] != '*' && s[i] != '|' && s[i] != '(' && s[i] != ')' && s[i] != '$' && s[i] != '?' && s[i] != '+') || readingString) {
            if (s[i] == '"') {
                readingString = readingString ? false : true;
            }
            string t(1,s[i]);
            tempValue = tempValue + t;
        } else {
            if (tempValue.size()>0) {
                result.insert(tempValue);
            }
            tempValue = "";
        }
    }
    if (tempValue.size() > 0){
        result.insert(tempValue);
    }
    return result;
}
/*---------------------------------------------------------------------
 * Function:      setToString
 * Purpose:       Generate a string from a set of strings 
 * In arg:        s, Set of strings
 * Return val:    Resulting string
 */
string setToString(set<string> s){
    string result = "";
    for (auto const &e: s) {
        result = result + e + " ";
    }
    return result;
}

/*---------------------------------------------------------------------
 * Function:      setToStringVector
 * Purpose:       Generate a string vector from a set of strings 
 * In arg:        s, Set of strings
 * Return val:    Resulting string vecotr
 */
vector<string> setToStringVector(set<string> s){
    vector<string> result;
    for (auto const &e: s) {
        result.push_back(e);
    }
    return result;
}

/*---------------------------------------------------------------------
 * Class:           SyntaxTree
 * Purpose:         Represents a SyntaxTree
 * Attributes:    
 *         root     The root node
 */ 
class SyntaxTree{
  public:
  Node* root;
  void join(Node* tree);
/*---------------------------------------------------------------------
 * Contructor
 * In arg:        expr, the regular expresion
 */
  SyntaxTree(string expr){
    stack<char> opStack; 
    string tempValue = ""; 
    bool readingString = false;
    //Read the expression
    vector<string> result;
    for (int i = 0; i < expr.length(); i = i + 1){
        //cout << expr[i] << endl;
        if (readingString){
            if (expr[i] == '"'){
                readingString = false;
            }
            tempValue = tempValue + expr[i];
        } else if (isOperator(expr[i])){
            if(tempValue.size() > 0){
                result.push_back(tempValue);
                tempValue = "";
            }
            if (opStack.size() == 0){
                opStack.push(expr[i]);
            } else {
                if (expr[i] == '(') {
                    opStack.push(expr[i]);
                } else if (expr[i] == ')') {
                    while (opStack.top() != '('){
                        string s(1,opStack.top());
                        result.push_back(s);
                        opStack.pop();
                    }
                    opStack.pop();
                 }else if (precedence(expr[i]) > precedence(opStack.top())) {
                    opStack.push(expr[i]);
                } else {
                    while (precedence(expr[i]) <= precedence(opStack.top())){
                        string s(1,opStack.top());
                        result.push_back(s);
                        opStack.pop();
                        if (opStack.empty()) {
                            break;
                        } 
                    }
                    opStack.push(expr[i]);
                }
            }
        }  else if (expr[i] == '"'){
            readingString = true;
            tempValue = tempValue  + expr[i];
        } else {
            tempValue = tempValue + expr[i];
        }
    }
    if(tempValue.size() > 0){
        result.push_back(tempValue);
        tempValue = "";
    }
    //cout << result << endl;
    while (!opStack.empty()){
        string s(1,opStack.top());
        result.push_back(s);
        opStack.pop();
    }

    stack<Node*> tree;
    for (int i=0; i < result.size(); i=i+1){
        if (isOperator(result[i][0])){
            if (result[i][0] == '*'){
                //cout << tree.front()->data << "\n";
                if (tree.size() == 0){
                    throw std::invalid_argument( "bad expresion received" );
                }
                Node* temp = new Node("*", tree.top(), NULL);
                tree.top() = temp;
            } else if (result[i][0] == '.' || result[i][0] == '|'){
                if (tree.size() == 0){
                    throw std::invalid_argument( "bad expresion received" );
                }
                Node* firstValue = tree.top();
                tree.pop();
                if (tree.size() == 0){
                    throw std::invalid_argument( "bad expresion received" );
                }
                string value(1, result[i][0]);
                Node* temp = new Node(value, tree.top(), firstValue);
                tree.top() = temp;
            } else if (result[i][0] == '+'){
                if (tree.size() == 0){
                    throw std::invalid_argument( "bad expresion received" );
                }
                Node* firstValue = new Node("*", tree.top(), NULL);
                Node* temp = new Node(".", tree.top(), firstValue);
                tree.top() = temp;
            } else if (result[i][0] == '?'){
                if (tree.size() == 0){
                    throw std::invalid_argument( " bad expresion received" );
                }
                Node* firstValue = new Node("$", NULL, NULL);
                Node* temp = new Node("|", tree.top(), firstValue);
                tree.top() = temp;
            }
            //cout << result[i][0] << "\n";
        } else {
            tree.push(new Node(result[i], NULL, NULL));
        }
       //opStack.push(new Node(opStack.top(), NULL, NULL);); 
    }
    //cout << "Resultado: " << result << "\n";
    //cout << "stack: " << tree.size() << "\n";
    root = tree.top();
  }
};

/*---------------------------------------------------------------------
 * Function:      join
 * Purpose:       Joins 2 syntax trees
 * In arg:        tree, syntax tree to join with
 * Return val:    -------
 */

void SyntaxTree::join(Node* tree){
    root = new Node("|", root, tree);
}

/*---------------------------------------------------------------------
 * Function:      printNode
 * Purpose:       Generate a string from a set of nodes
 * In arg:        nodes, set of nodes
 * Return val:    Resulting string
 */
string printNode(set<Node*> nodes){
    string result = "";
    for (auto const &e: nodes) {
        result = result + " " + to_string(e->id);
    }
    return "{" + result + " }";
}

/*---------------------------------------------------------------------
 * Function:      printTree
 * Purpose:       Print the syntax tree
 * In arg:        root, a node from the tree, counter, the depth in the tree
 * Return val:    -------
 */
void printSyntaxTree(Node* root, int counter){
    string left = "";
    string right = "";
    if (root->left != NULL){
        Node* leftNode = root->left;
        left = leftNode->data;
    } 
    if (root->right != NULL){
        Node* rightNode = root->right;
        right = rightNode->data;
    } 
    cout << "(" << counter << ", value: " << root->data << ", id: " << root->id << ", left: " << left << ", right: " << right << ") firstpos: " << printNode(root->firstpos) << " lastpos: " << printNode(root->lastpos) << "\n";

    if (root->left != NULL){
        printSyntaxTree(root->left, counter + 1);
    }
    if (root->right != NULL){
        printSyntaxTree(root->right, counter + 1);
    }
}

/*---------------------------------------------------------------------
 * Function:      printIntSet
 * Purpose:       Generate a string from a set of ints
 * In arg:        nodes, set of ints 
 * Return val:    Resulting string
 */
string printIntSet(set<int> set){
    string result ;
    for (auto const &e: set) {
        result = result + to_string(e) + " ";
    }   
    return "{ " + result + "}";
}
/*---------------------------------------------------------------------
 * Function:      setNodeToSetInt
 * Purpose:       Generate a set of ints from a set of nodes
 * In arg:        nodes, set of nodes
 * Return val:    Resulting set
 */
set<int> setNodeToSetInt(set<Node*> nodes){
    set<int> result;
    for (auto const &e: nodes) {
        result.insert(e->id);
    }
    return result;
}

/*---------------------------------------------------------------------
 * Class:           AFDirect
 * Purpose:         Represents a direct deterministic finite automata
 * Attributes:    
 *         nodes       the number of nodes in the follow pos
 *         followposV  Which node represents each position in followpos
 *         ids         The ids of each node
 *         leafs       The char value of each node
 *         states      The states of the automata, the vector representes the states in the afd and the set contains the afn states
 *         transitions The transitions of each of the the automata nodes
 *         alphabet    The alphabet of the automata
 */ 
class AFDirect{
    public:
    //followpos table
    vector<int> nodes; //which node represents each position in followpos
    vector<set<int>> followposV;
    //Reference table
    vector<int> ids;
    vector<string> leafs;
    //AFDode* start;
    vector <set<int>> states;
    vector <vector <int>> transitions;
    set<string> alphabet;
    vector<int> finalids;
    vector<string> expressions;
    vector<string> expressionsId;
    map<string,bool> exceptTokens;
    set<int> whitespaces;
    //Methods
    int isTerminal(int currentState);
    void followpos(Node* node);
    set<int> getFollowpos(int id);
    string getLetter(int id);
    int getNumber(string letter);
    void simulate(string chain);
    int getTransition(string charcterNumber);
    /*---------------------------------------------------------------------
    * Contructor
    * In arg:        start, the root of the syntax tree    alphabet, the automata alphabet
    */
    AFDirect(Node* start, set<string> alphabet, vector<int> finalids, vector<string> expressions , vector<string> expressionsId, 
    set<int> whitespaces, map<string,bool> exceptTokens){
        followpos(start);
       // cout << "alhabet " << setToString(alphabet) << endl;
        this->finalids = finalids;
        this->alphabet = alphabet;
        this->expressions = expressions;
        this->expressionsId = expressionsId;
        this->whitespaces = whitespaces;
        this->exceptTokens = exceptTokens;
        queue<set<int>> pendingStates;
        states.push_back(setNodeToSetInt(start->firstpos));
        pendingStates.push(setNodeToSetInt(start->firstpos));
        while (!pendingStates.empty()){
            vector<int> temporalTransitions;
            for (auto const &e: alphabet) {
                //cout << e << endl;
                set<int> newState;
                set<int> result;
                for (auto const &h: pendingStates.front()) {
                    if(getLetter(h).compare(e) == 0){
                        //cout << h << getLetter(h) << " " << printIntSet(getFollowpos(h)) << "\n";
                        result = getFollowpos(h);
                        newState.insert(result.begin(), result.end());
                    }
                }
                int pos = -1;
                if (newState.size() > 0){
                    //ver si existe
                    for (int i = 0; i < states.size(); i = i + 1){
                        if (equal(states[i].begin(), states[i].end(), newState.begin(), newState.end())){
                            pos = i;
                        }
                    }
                    //sino agreagar al pool de estados
                    if (pos == -1){
                        states.push_back(newState);
                        pendingStates.push(newState);
                        pos = states.size() - 1;
                    }
                //Agregar transicipon
                }
                temporalTransitions.push_back(pos);
                //cout << printIntSet(pendingStates.front()) << " " << e << " " << printIntSet(newState) << pos << "\n";
            }
            transitions.push_back(temporalTransitions);
            pendingStates.pop();
        }
    }
/*---------------------------------------------------------------------
 * Contructor
 * In arg:        states, the states of the automata, transitions, the transitions of the automata
 *                alphabet, the automata alphabet   ids, the ids of the nodes    leafs, the char value of the nodes
 */
AFDirect( vector <set<int>> states, vector <vector <int>> transitions, 
    set<string> alphabet, vector<int> ids, vector<string> leafs){
        this->alphabet = alphabet;
        this->states = states;
        this->transitions = transitions;
        this->leafs = leafs;
        this->ids = ids;
    }
};
/*---------------------------------------------------------------------
 * Function:      followpos
 * Purpose:       Calculate the followpos from a syntax tree node
 * In arg:        node, a syntaxtree node
 * Return val:    -------
 */
void AFDirect::followpos(Node* node){
    Node* root;
    set<int> result;
    queue<Node*> pendingnodes;
    pendingnodes.push(node);
    while(!pendingnodes.empty()){
        Node* root = pendingnodes.front();
        pendingnodes.pop();
        if (root->data[0] == '.'){
            for (auto const &e: lastpos(root->left)) {
                int pos  = -1;
                for (int i = 0; i < nodes.size(); i = i + 1){
                    if (e->id == nodes[i]){
                        pos = i;
                    } 
                }                if (pos == -1){
                    nodes.push_back(e->id);
                    set<int> dummy;
                    followposV.push_back(dummy);
                    pos = nodes.size() - 1;
                }
                result = setNodeToSetInt(firstpos(root->right));
                followposV[pos].insert(result.begin(), result.end());
            } 

        } else if (root->data[0] == '*'){
            for (auto const &e: root->lastpos) {         
                int pos  = -1;
                for (int i = 0; i < nodes.size(); i = i + 1){
                    if (e->id == nodes[i]){
                        pos = i;
                    } 
                }
                if (pos == -1){
                    nodes.push_back(e->id);
                    set<int> dummy;
                    followposV.push_back(dummy);
                    pos = nodes.size() - 1;
                }
                result = setNodeToSetInt(root->firstpos);
                followposV[pos].insert(result.begin(), result.end());
            };

        } else if (root->data[0] != '|'){ //create reference table
            int pos  = -1;
            for (int i = 0; i < ids.size(); i = i + 1){
                if (root->id == ids[i]){
                        pos = i;
                } 
            }
            if (pos = -1){
                ids.push_back(root->id);
                leafs.push_back(root->data);
            }
        }
        // follow pos
        
        if (root->left != NULL) {
            pendingnodes.push(root->left);
        }
        if (root->right != NULL) {
            pendingnodes.push(root->right);
        }
    }
}

/*---------------------------------------------------------------------
 * Function:      getFollowpos
 * Purpose:       Optain the followpos from a node
 * In arg:        id, id of the node
 * Return val:    set of ints that contain the ids of the followpos result
 */
set<int> AFDirect::getFollowpos(int id){
    set<int> result;
    for (int i = 0; i < nodes.size(); i = i + 1){
        if (nodes[i] == id){
            result = followposV[i];
        }
    }
    return result;
}

/*---------------------------------------------------------------------
 * Function:      getLetter
 * Purpose:       Optain the the letter of a node using its id
 * In arg:        id, id of the node
 * Return val:    nodes letter
 */
string AFDirect::getLetter(int id){
    for (int i = 0; i < ids.size(); i= i +1){
        if (ids[i] == id){
            return leafs[i];
        }
    }
}
/*---------------------------------------------------------------------
 * Function:      getNumber
 * Purpose:       Optain the the id of a node using its letter
 * In arg:        letter, letter of the node
 * Return val:    nodes id
 */
int AFDirect::getNumber(string letter){
    for (int i = 0; i < leafs.size(); i= i +1){
        if (leafs[i].compare(letter) == 0){
            return ids[i];
        }
    }
    return -1;
}

/*---------------------------------------------------------------------
 * Function:      isTerminal
 * Purpose:       Check is a state is a terminal state
 * In arg:        currentState, the state to be checked
 * Return val:    -------
 */
int AFDirect::isTerminal(int currentState){
    for (int j = 0; j < finalids.size(); j = j + 1){
        int finalNum = finalids[j];
        if (states[currentState].find(finalNum) != states[currentState].end()){
            return j;
        }
    }
    return -1;
}


/*---------------------------------------------------------------------
 * Function:      getTransition
 * Purpose:       Get the position of the character in the alphabets set
 * In arg:        charcterNumber, the character that is going to be search
 * Return val:    The position of the character
 */
int AFDirect:: getTransition(string charcterNumber){
    int cont = 0 ;
    int transition = -1 ;//check if it does not change
    //Make transition
    for (auto const &e: alphabet) {
        //cout << e << " " << charcterNumber << endl;
        if (  charcterNumber == e ){
            //cout << "si" << endl;
            transition = cont;
        //subStringlength = subchain.size();
         }
        cont = cont + 1;
    }
    return transition;
}

/*---------------------------------------------------------------------
 * Function:      simulate
 * Purpose:       Simulate a direct deterministic finite automata for a input chain
 * In arg:        afd, the direct deterministic finite automata     chain, the input chain
 * Return val:    -------
 */
void AFDirect::simulate(string chain){
    //check alphabet
    stack<int> chequedStates;
    int i = 0;
    int currentState = 0;
    chequedStates.push(0);
    string readCharacters = "";
    while(i < chain.size()){
        if (this->whitespaces.find((int)chain[i]) != whitespaces.end()){
            //check if terminal state
            i = i + 1;
        } else {
            int transition = getTransition(to_string((int)chain[i]));
            if (transitions[currentState][transition] == -1){
                if (chequedStates.size() == 1){
                    readCharacters = readCharacters + chain[i];
                    i++;
                    cout << "<" << readCharacters << ", error>\n";
                    readCharacters = "";
                } else {
                    int goback = 0;
                    int terminalId = isTerminal(currentState);
                    while(terminalId == -1){
                        if (chequedStates.size() == 1){
                            cout << "<" << readCharacters << ", error>\n";
                            readCharacters = "";
                            currentState = 0;
                            chequedStates.top() = 0;
                            goback = -1;
                            break;
                        }
                        chequedStates.pop();
                        currentState = chequedStates.top();
                        terminalId = isTerminal(currentState);
                        goback++;
                    }
                    if (goback > -1){
                        i = i - goback;
                        readCharacters = readCharacters.substr(0, readCharacters.size()-goback);
                        if (exceptTokens[expressionsId[terminalId]]){
                            cout << "<" << readCharacters << ", keyword>" << endl;
                        } else {
                            cout << "<" << readCharacters << "," << expressionsId[terminalId] << ">" << endl;
                        }
                        readCharacters = "";
                        while (chequedStates.size() > 1){
                            chequedStates.pop();
                        }
                        currentState = 0;
                    }
                }
            } else {
                currentState = transitions[currentState][transition];
                chequedStates.push(currentState);
                readCharacters = readCharacters + chain[i];
                i++;
            }
        }
    }
}
/*---------------------------------------------------------------------
 * Function:      writeAFDirect
 * Purpose:       Write the direct deterministic finite automata
 * In arg:        afdirect, the direct deterministic finite automata       name, the name of the file
 * Return val:    -------
 */
void writeAFDirect(AFDirect* afdirect, string name){ //, vector<int> finalNumers){
    fstream my_file;
	my_file.open(name, ios::out);
    //cout << nodes.size() << "\n";
	if (!my_file) {
		cout << "File not created!";
	}
	else {
		cout << "File created successfully!" << "\n";
        for (int j = 0; j < afdirect->finalids.size(); j = j + 1){
            int finalNum = afdirect->finalids[j];
            for (int i = 0; i < afdirect->states.size(); i = i + 1){
                if (afdirect->states[i].find(finalNum) != afdirect->states[i].end()){
                    my_file << i << "\n";
                }
            }
        }
        vector<string> s = setToStringVector(afdirect->alphabet);
        for (int i = 0; i < afdirect->transitions.size(); i = i + 1){
            for (int j = 0; j < afdirect->transitions[i].size(); j = j + 1){
                if (afdirect->transitions[i][j] != -1){
                    my_file << i << " "<< afdirect->transitions[i][j] << " " << s[j] << "\n";
                }
            }
        }
		my_file.close();
	}
}
/*---------------------------------------------------------------------
 * Function:      printAFDirect
 * Purpose:       Print in console the direct deterministic finite automata
 * In arg:        afd, the direct deterministic finite automata 
 * Return val:    -------
 */
void printAFDirect(AFDirect* afd){
    cout << "///////////////////////deterministic finite direct automata/////////////////////// \n";
    cout << "=========== Reference table =========== \n";
    for (int i = 0; i <  afd->ids.size(); i = i + 1){
        cout << afd->ids[i] << " " << afd->leafs[i] << "\n";
    }
    cout << "=============================== \n";
        cout << "=========== Follow pos =========== \n";
    for (int i = 0; i <  afd->nodes.size(); i = i + 1){
        cout << afd->nodes[i] << " " << printIntSet(afd->followposV[i]) << "\n";
    }
    cout << "=============================== \n";
    cout << "=========== States =========== \n";
    for (int i = 0; i <  afd->states.size(); i = i + 1){
        cout << i << " " << printIntSet(afd->states[i]) << "\n";
    }
    cout << "=============================== \n";
    cout << "=========== Transitions =========== \n";
    string trans = "    ";
    for (auto const &e: afd->alphabet) {
        trans = trans + e + "   ";
    }
    cout << trans << "\n";
    for (int i = 0; i <  afd->states.size(); i = i + 1){\
        trans = to_string(i) + "   ";
        for (int j = 0; j <  afd->alphabet.size(); j = j + 1){
            if (afd->transitions[i][j] == -1){
                trans = trans + "-" + "   ";
            } else {
                trans = trans + to_string(afd->transitions[i][j]) + "   ";
            }
        }
        cout << trans << "\n";
    }
    cout << "=============================== \n";
    cout << "//////////////////////////////////////////////////////// \n";
}

/*---------------------------------------------------------------------
 * Function:      setIntToVectorInt
 * Purpose:       Generate a vector of ints from a set of ints
 * In arg:        set, set of ints
 * Return val:    Resulting vector
 */
vector<int> setIntToVectorInt(set<int> set){
    vector<int> result;
    for (auto const &e: set) {
        result.push_back(e);
    }
    return result;
}

/*---------------------------------------------------------------------
 * Function:      getPositionInSet
 * Purpose:       Get the set number, of the set that contains a number in a set of sets of ints
 * In arg:        partition, set of sets of ints   num, the number that is going to be search
 * Return val:    the set position
 */
int getPositionInSet(set<set<int>> partition, int num) {
    int cont = 0;
    for (auto const &e: partition) {
        if (e.find(num) != e.end() ){
            return cont;
        }
        cont ++;
    }
    return -1;
}

/*---------------------------------------------------------------------
 * Function:      separateSets
 * Purpose:       Separate Sets of states that go to diferent sets in a automata
 * In arg:        partition,set that contains the initial partition of final and non final states alphabet,alphabet of the automata  transitions, transitions of the automata
 * Return val:    Resulting partitions
 */
set<set<int>> separateSets(set<set<int>> partition, set<string> alphabet, vector <vector <int>> transitions){
    bool changed = true;
    while (changed){
        changed = false;
        set<set<int>> resul;
        set<int> newSet;
        for (auto const &e: partition) {
            set<int> tempSet = e;
            vector<int> comparisonVector (alphabet.size(),-1);  
            if (tempSet.size() > 1){
                for (auto const &f: e) {
                    for(int i = 0; i < alphabet.size(); i = i + 1){
                        int result = transitions[f][i];
                        if (result != -1) {
                            // add -1 condition
                            if (comparisonVector[i] == -1){
                                comparisonVector[i] = getPositionInSet(partition, result);
                            } else if (comparisonVector[i] != getPositionInSet(partition, result) ){
                                newSet.insert(f);
                                tempSet.erase(f);
                            }
                        }
                    }
                }
                resul.insert(tempSet); 
            } else {
                resul.insert(tempSet);
            }
        }
        if (newSet.size() > 0){
            resul.insert(newSet);
            partition = resul;
            changed = true;
        }
    }
    return partition;
}

/*---------------------------------------------------------------------
 * Function:      minimization
 * Purpose:       Minimization of finite direct automatas
 * In arg:        states,states of the automata  alphabet,alphabet of the automata  transitions, transitions of the automata finalId, the id of the final states in the automata
 * Return val:    Resulting partitions
 */
AFDirect* minimization(vector <set<int>> states, set<string> alphabet,vector <vector <int>> transitions, int finalId){
    set<int> temp1;
    set<int> temp2;
    //Separate sets
    set<int> terminals;
    for (int i = 0; i < states.size(); i = i + 1){
        if (states[i].find(finalId) == states[i].end() ){
            temp1.insert(i);
        } else {
            temp2.insert(i);
            terminals.insert(i);
        }
    }
    set<set<int>> partition;
    if (temp1.size() > 0){
        partition.insert(temp1);
    }
    if (temp2.size() > 0){
        partition.insert(temp2);
    }
    partition = separateSets(partition, alphabet, transitions);

    vector <set<int>> finalStates;
    vector <vector <int>> finalTransitions;
    for (auto const &e: partition) {
        bool terminalFlag = false;
        set<int> tempSet = e;
        vector<int> tempTransition (alphabet.size(),-1);
        for (auto const &f: e) {
            if (terminals.find(f) != terminals.end()){
                terminalFlag = true;
            }
            for(int i = 0; i < alphabet.size(); i = i + 1){
                int result = transitions[f][i];
                if (result != -1) {
                    tempTransition[i] = getPositionInSet(partition, result);
                }
            }
        }
        if (terminalFlag){
            tempSet.insert(9999);
        }
        finalStates.push_back(tempSet);
        finalTransitions.push_back(tempTransition);
    }

    vector<int> ids;
    ids.push_back(9999);
    vector<string> leafs;
    leafs.push_back("#");
    return new AFDirect(finalStates, finalTransitions, alphabet, ids, leafs);
    //return NULL;
}

/*---------------------------------            if (tempValue.size()>0) {
                result.insert(tempValue);
            }
            tempValue = "";------------------------------------
 * Function:      checkAlphabet
 * Purpose:       Compare if the alphabet os the input string is the same of the regular expresion
 * In arg:        alphaber,alphabet of the regular expresion  chainAlphabet, alphabet of the input string
 * Return val:    Resulting flag
 */
bool checkAlphabet(set<string> alphabet, set<string> chainAlphabet){
    bool result = true;
    for (auto const &e: chainAlphabet) {
        if (alphabet.find(e) == alphabet.end()){
            result = false;
        }
    }
    return result;
}

int main(int argc, char **argv) { 
    vector<string> expressions;
    vector<string> expressionsId;
    map<string,bool> exceptTokens;
    vector<int> finalids;
    map<string,string> savedCharacters;
    set<int> whitespaces;
    //INSERT EXPRESSIONS
    string expr = expand(expressions[0]);
    cout << expr << endl;
    set<string> alphabet = getAlphabet(expr);
    expr = '(' + expr + ").#";
    SyntaxTree* syntaxtree = new SyntaxTree(expr);

    finalids.push_back(syntaxtree->root->right->id);
    for (int i = 1; i < expressions.size(); i = i + 1){
        string expr = expand(expressions[i]);
        set<string> tempAlphabet = getAlphabet(expr);
        alphabet.insert(tempAlphabet.begin(), tempAlphabet.end());
        expr = '(' + expr + ").#";
        SyntaxTree* tempsyntaxtree = new SyntaxTree(expr);
        finalids.push_back(tempsyntaxtree->root->right->id);
        syntaxtree->join(tempsyntaxtree->root);
    }
    fillFunctions(syntaxtree->root);
    printSyntaxTree(syntaxtree->root, 0);
    AFDirect* afdirect = new AFDirect(syntaxtree->root, alphabet, finalids, expressions, expressionsId, whitespaces, exceptTokens);
    writeAFDirect(afdirect, "afdirect.txt");
    printAFDirect(afdirect); 
    /*
    AFDirect* afdirectmini = minimization(afdirect->states, afdirect->alphabet,afdirect->transitions, afdirect->getNumber("#"));
    printAFDirect(afdirectmini);
    writeAFDirect(afdirectmini, "afdirectmini.txt");*/
    afdirect->simulate("if hola 12345");
    /*
    string expr = expand(argv[1]); //asign the regex expresion
    string chain = argv[2];
    set<string> alphabet = getAlphabet(expr);
    //set<string> chainAlphabet = getAlphabet(chain);
    
    if (!correctParentesis(expr)){
        cout << "Error: Missing parentesis\n";
        cout << "Check your expression and try again\n";
        return 0;
    }
    /*if (!checkAlphabet(alphabet, chainAlphabet)){
        cout << "Error: The alphabet of the regular expression and the input string are not the same\n";
        cout << "Check your expression and try again\n";
        return 0;
    }*/
    /*
    try {
        cout << expr << "\n";
        expr = '(' + expr + ").#";
        cout << expr << "\n";
        SyntaxTree* syntaxtree = new SyntaxTree(expr);
        fillFunctions(syntaxtree->root);
        cout << "///////////////////////Binary tree/////////////////////// \n";
        printSyntaxTree(syntaxtree->root, 0);
        cout << "//////////////////////////////////////////////////////// \n";
        
        AFDirect* afdirect = new AFDirect(syntaxtree->root, alphabet);
        writeAFDirect(afdirect, "afdirect.txt");
        printAFDirect(afdirect);
        
        cout << "///////////////////////Minimization/////////////////////// \n";
        AFDirect* afdirectmini = minimization(afdirect->states, afdirect->alphabet,afdirect->transitions, afdirect->getNumber("#"));
        printAFDirect(afdirectmini);
        writeAFDirect(afdirectmini, "afdirectmini.txt");
        /*
        auto start = high_resolution_clock::now();
        cout << "Direct deterministic finite automata: " << (simulateAFDirect(afdirect, chain)? "approved\n" : "rejected\n");
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start); 
        cout << "Execution time(microseconds): " << duration.count() << endl;  
        */
       /*
    } catch (std::exception& e) {
        cout << "Error: An error ocurred\n";
        cout << "Check your expression and try again\n";
    }*/
    return 0;
}