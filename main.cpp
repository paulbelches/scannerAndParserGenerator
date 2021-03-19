#include <iostream>
#include <bits/stdc++.h> 
#include <utility> 
#include <fstream>
#include <string>
#include <stack>
#include <queue>
#include <algorithm>
#include<set>
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
 *    omp_mat_vect <'regular expresion'> <string>
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
  char data;
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
  Node(char d, Node* left, Node* right){
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
    for (int i = 0; i < expr.length(); i = i + 1){
        if (expr[i] == '|'){
            cont = 0;
        } 
        else if (expr[i] == '('){
            if (cont == 1){
                r = r + '.';
                cont = 0;
            }
        } else if (expr[i] == ')' || expr[i] == '*' || expr[i] == '+' || expr[i] == '?'){
            ;
        }
        else {
            cont = cont + 1;
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
 * Function:      printTree
 * Purpose:       Print the syntax tree
 * In arg:        root, a node from the tree, counter, the depth in the tree
 * Return val:    -------
 */
void printTree(Node* root, int counter){
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
    cout << "(" << counter << ", value: " << root->data << ", left: " << left << ", right: " << right << ")" << "\n";
    if (root->left != NULL){
        printTree(root->left, counter + 1);
    }
    if (root->right != NULL){
        printTree(root->right, counter + 1);
    }
}

/*---------------------------------------------------------------------
 * Function:      nullable
 * Purpose:       Calculate nullable of a node of the syntax tree
 * In arg:        root, a node from the tree
 * Return val:    Whether is nullable or not
 */
bool nullable(Node* root){
    if (root->data == 'e'){
        return true;
    } else if (root->data == '|'){
        return nullable(root->left) || nullable(root->right);
    } else if (root->data == '.'){
        return nullable(root->left) && nullable(root->right);
    } else if (root->data == '*'){
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
    if (root->data == '|'){
        tempSet = lastpos(root->right);
        result = lastpos(root->left);
        result.insert(tempSet.begin(), tempSet.end());
    } else if (root->data == '.'){
        if (nullable(root->right)){
            tempSet = lastpos(root->right);
            result = lastpos(root->left);
            result.insert(tempSet.begin(), tempSet.end());
        } else {
            result = lastpos(root->right);
        }
    } else if (root->data == '*'){
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
    if (root->data == '|'){
        tempSet = firstpos(root->right);
        result = firstpos(root->left);
        result.insert(tempSet.begin(), tempSet.end());
    } else if (root->data == '.'){
        if (nullable(root->left)){
            tempSet = firstpos(root->right);
            result = firstpos(root->left);
            result.insert(tempSet.begin(), tempSet.end());
        } else {
            result = firstpos(root->left);
        }
    } else if (root->data == '*'){
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
 * Class:         AFNode
 * Purpose:       Represents a non-deterministic finite automata node
 * Attributes:    
 *      id        The id of the node
 *      edges     The edges comming out of the node
 */
class AFNode{
    public:
    int id;
    vector <pair<char, AFNode*>> edges;
    void addEdge(char values, AFNode* node);
/*---------------------------------------------------------------------
 * Contructor
 * In arg:        -------
 */
    AFNode(){
        id = rand() % 5000 + 1;
    }
};

/*---------------------------------------------------------------------
 * Function:      addEdge
 * Purpose:       Add an Edge to the non-deterministic finite automata node
 * In arg:        value, the transition values   node, the non-deterministic finite automata node
 * Return val:    -------
 */
void AFNode::addEdge(char value, AFNode* node){
    edges.push_back(make_pair(value, node)); 
}
/*---------------------------------------------------------------------
 * Class:         AFN
 * Purpose:       Represents a non-deterministic finite automata
 * Attributes:    
 *      start     Starting node
 *      end       Finishing node
 */
class AFN{  
    public:
    AFNode* start;
    AFNode* end;
/*---------------------------------------------------------------------
 * Contructor
 * In arg:        root, the root of the syntax tree
 */
    AFN(Node* root){
        string empty = "e";
        AFN* operand1;
        AFN* operand2;
        switch(root->data){
            case '*':
                start = new AFNode();
                end = new AFNode();
                start->addEdge(empty[0], end);
                operand1 = new AFN(root->left);
                start->addEdge(empty[0], operand1->start);
                operand1->end->addEdge(empty[0], end);
                operand1->end->addEdge(empty[0], operand1->start);
            break;
            case '|':
                start = new AFNode();
                end = new AFNode();
                operand1 = new AFN(root->left);
                operand2 = new AFN(root->right);
                start->addEdge(empty[0], operand1->start);
                start->addEdge(empty[0], operand2->start);
                operand1->end->addEdge(empty[0], end);
                operand2->end->addEdge(empty[0], end);
            break;
            case '.':
                operand1 = new AFN(root->left);
                operand2 = new AFN(root->right);
                operand1->end->edges = operand2->start->edges;
                operand2->start = operand1->end;
                start = operand1->start;
                end = operand2->end;
            break; //add terminal character
            case '#':
                start = new AFNode();
                end = new AFNode();
                end->id = 9999;
                start->addEdge('#', end);
            break; //add terminal character
            default:
                start = new AFNode();
                end = new AFNode();
                start->addEdge(root->data, end);
        }
    }
};
/*---------------------------------------------------------------------
 * Function:      generateSet
 * Purpose:       Generate a set of string cointaining the edges of each node of a non-deterministic finite automata
 * In arg:        start, the non-deterministic finite automata starting node 
 * Return val:    Set of string cointaining the edges of each node 
 */
set<string> generateSet(AFNode* start){
    set<string> result;
    stack<AFNode*> nodes;
    set<int> checkedNodes;
    nodes.push(start);
    checkedNodes.insert(start->id);
    while (!nodes.empty()){
        AFNode* temporalNode =  nodes.top();
        nodes.pop();
        for (int i = 0; i < temporalNode->edges.size(); i = i + 1){
            int tnum = temporalNode->edges[i].second->id;
            if (checkedNodes.find(tnum) == checkedNodes.end()){
                nodes.push(temporalNode->edges[i].second);
                checkedNodes.insert(tnum);
            }
            string s = to_string(temporalNode->id)+" "+to_string(tnum) +" "+temporalNode->edges[i].first;
            result.insert(s);
        }
    }
    return result;
}
/*---------------------------------------------------------------------
 * Function:      writeAFN
 * Purpose:       Write the non-deterministic finite automata
 * In arg:        start, the non-deterministic finite automata starting node 
 * Return val:    -------
 */
void writeAFN(AFNode* start){
    set<string> nodes;
    nodes = generateSet(start);
    fstream my_file;
	my_file.open("thompson.txt", ios::out);
    //cout << nodes.size() << "\n";
	if (!my_file) {
		cout << "File not created!";
	}
	else {
		cout << "File created successfully!" << "\n";
        for (auto const &e: nodes) {
            my_file << e << "\n";
        }
		my_file.close();
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
    for (int i=0; i<s.length();i++){
        if (s[i] != '.' && s[i] != '*' && s[i] != '|' && s[i] != '(' && s[i] != ')' && s[i] != 'e' && s[i] != '?' && s[i] != '+'){
            string t(1,s[i]);
            result.insert(t);
        }
    }
    return result;
}

/*---------------------------------------------------------------------
 * Function:      lock
 * Purpose:       Calculate the lock of a set of non/deterministic finite automata nodes
 * In arg:        initialNodes, Set of non/deterministic finite automata nodes
 * Return val:    Resulting  set of non/deterministic finite automata nodes
 */
set<AFNode*> lock(set<AFNode*> initialNodes){
    set<AFNode*> result;
    stack<AFNode*> nodes;
    set<int> checkedNodes;
    for (auto const &e: initialNodes) {
        nodes.push(e);
        result.insert(e);
    }
    while (!nodes.empty()){
        AFNode* temporalNode =  nodes.top();
        nodes.pop();
        for (int i = 0; i < temporalNode->edges.size(); i = i + 1){
            int tnum = temporalNode->edges[i].second->id;
            if (temporalNode->edges[i].first == 'e' && checkedNodes.find(tnum) == checkedNodes.end()){
                result.insert(temporalNode->edges[i].second);
                nodes.push(temporalNode->edges[i].second);
                checkedNodes.insert(tnum);
            }
        }
    }
    return result;
}

/*---------------------------------------------------------------------
 * Function:      move
 * Purpose:       Calculate the move of a set of non-deterministic finite automata nodes
 * In arg:        initialNodes, Set of non/deterministic finite automata nodes   s, the value of the transition
 * Return val:    Resulting  set of non-deterministic finite automata nodes
 */
set<AFNode*> move(set<AFNode*> initialNodes, char character){
    set<AFNode*> result;
    stack<AFNode*> nodes;
    for (auto const &e: initialNodes) {
        nodes.push(e);
    }
    while (!nodes.empty()){
        AFNode* temporalNode =  nodes.top();
        nodes.pop();
        for (int i = 0; i < temporalNode->edges.size(); i = i + 1){
            if (temporalNode->edges[i].first == character){
                result.insert(temporalNode->edges[i].second);
                //nodes.push(temporalNode->edges[i].second);
            }
        }
    }
    return result;
}

/*---------------------------------------------------------------------
 * Function:      printSet
 * Purpose:       Generate a string from a set of non/deterministic finite automata nodes
 * In arg:        nodes, Set of non/deterministic finite automata nodes
 * Return val:    Resulting string
 */
string printSet(set<AFNode*> nodes){
    string result = "";
    for (auto const &e: nodes) {
        result = result + to_string(e->id) + " ";
    }
    return "{" + result + "} ";
}

/*---------------------------------------------------------------------
 * Class:           AFN
 * Purpose:         Represents a deterministic finite automata
 * Attributes:    
 *      states      The states of the automata, the vector representes the states in the afd and the set contains the afn states
 *      transitions The transitions of each of the the automata nodes
 *      alphabet    The alphabet of the automata
 */ 
class AFD{
    public:
    //AFDode* start;
    vector <set<AFNode*>> states;
    vector <vector <int>> transitions;
    set<string> alphabet;
/*---------------------------------------------------------------------
 * Contructor
 * In arg:        start, the afn starting node   alphabet,the alphabet of the automata
 */
    AFD(set<AFNode*> start, set<string> alphabet){
        this->alphabet = alphabet;
        states.push_back(start);
        queue<set<AFNode*>> pendingStates;
        pendingStates.push(start);
        cout << "Initial state" << printSet(pendingStates.front()) << "\n";
        while (!pendingStates.empty()){
            vector<int> temporalTransitions;
            for (auto const &e: alphabet) {
                set<AFNode*> temporalSet;
                temporalSet = lock(move(pendingStates.front(), e[0]));
                cout << printSet(pendingStates.front()) << "-" << e[0] << "->" << printSet(temporalSet) << "\n";
                int pos = -1;
                for (int i = 0; i < states.size(); i = i + 1){
                    if (equal(states[i].begin(), states[i].end(), temporalSet.begin(),temporalSet.end())){
                        pos = i;
                    }
                }
                if (pos == -1 && temporalSet.size() > 0){
                    states.push_back(temporalSet);
                    pos = states.size() - 1;
                    pendingStates.push(temporalSet);
                }
                temporalTransitions.push_back(pos);
            }
            transitions.push_back(temporalTransitions);
            pendingStates.pop();
        }
    }
};

/*---------------------------------------------------------------------
 * Function:      setToString
 * Purpose:       Generate a string from a set of strings 
 * In arg:        s, Set of strings
 * Return val:    Resulting string
 */
string setToString(set<string> s){
    string result = "";
    for (auto const &e: s) {
        result = result + e;
    }
    return result;
}
/*---------------------------------------------------------------------
 * Function:      writeAFD
 * Purpose:       Write the deterministic finite automata
 * In arg:        afd, the  deterministic finite automata
 * Return val:    -------
 */
void writeAFD(AFD* afd){
    fstream my_file;
	my_file.open("afd.txt", ios::out);
    //cout << nodes.size() << "\n";
	if (!my_file) {
		cout << "File not created!";
	}
	else {
		cout << "File created successfully!" << "\n";
        //write terminal nodes
        for (int i = 0; i < afd->states.size(); i = i + 1){
            bool flag = false;
            for (auto const &e: afd->states[i]) {
                if (e->id == 9999){
                    flag = true;
                }
            }
            if (flag){
                my_file << i << "\n";
            }
            
        }
        //write transitions
        string s = setToString(afd->alphabet);
        for (int i = 0; i < afd->transitions.size(); i = i + 1){
            for (int j = 0; j < afd->transitions[i].size(); j = j + 1){
                if (afd->transitions[i][j] != -1){
                    my_file << i << " "<< afd->transitions[i][j] << " " << s[j] << "\n";
                }
            }
        }
		my_file.close();
	}
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
/*---------------------------------------------------------------------
 * Contructor
 * In arg:        expr, the regular expresion
 */
  SyntaxTree(string expr){
    stack<char> opStack; 
    //Read the expression
    string result;
    for (int i = 0; i < expr.length(); i = i + 1){
        if (isOperator(expr[i])){
            if (opStack.size() == 0){
                opStack.push(expr[i]);
            } else {
                if (precedence(expr[i]) > precedence(opStack.top())) {
                    opStack.push(expr[i]);
                } else if (expr[i] == '(') {
                    opStack.push(expr[i]);
                } else if (expr[i] == ')') {
                    while (opStack.top() != '('){
                        result = result + opStack.top();
                        opStack.pop();
                    }
                    opStack.pop();
                } else {
                    while (precedence(expr[i]) <= precedence(opStack.top())){
                        result = result + opStack.top();
                        opStack.pop();
                        if (opStack.empty()) {
                            break;
                        } 
                    }
                    opStack.push(expr[i]);
                }
            }
        } else {
            result = result + expr[i];
        }
    }
    
    while (!opStack.empty()){
        result = result + opStack.top();
        opStack.pop();
    }
    cout << "result:" << result << "\n";
    stack<Node*> tree;
    for (int i=0; i < result.size(); i=i+1){
        if (isOperator(result[i])){
            if (result[i] == '*'){
                //cout << tree.front()->data << "\n";
                if (tree.size() == 0){
                    throw std::invalid_argument( "bad expresion received" );
                }
                Node* temp = new Node('*', tree.top(), NULL);
                tree.top() = temp;
            } else if (result[i] == '.' || result[i] == '|'){
                if (tree.size() == 0){
                    throw std::invalid_argument( "bad expresion received" );
                }
                Node* firstValue = tree.top();
                tree.pop();
                if (tree.size() == 0){
                    throw std::invalid_argument( "bad expresion received" );
                }
                Node* temp = new Node(result[i], tree.top(), firstValue);
                tree.top() = temp;
            } else if (result[i] == '+'){
                if (tree.size() == 0){
                    throw std::invalid_argument( "bad expresion received" );
                }
                Node* firstValue = new Node('*', tree.top(), NULL);
                Node* temp = new Node('.', tree.top(), firstValue);
                tree.top() = temp;
            } else if (result[i] == '?'){
                if (tree.size() == 0){
                    throw std::invalid_argument( " bad expresion received" );
                }
                Node* firstValue = new Node('e', NULL, NULL);
                Node* temp = new Node('|', tree.top(), firstValue);
                tree.top() = temp;
            }
            //cout << result[i] << "\n";
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
 * Function:      setAFNodeToSetInt
 * Purpose:       Generate a set of ints from a set of non/deterministic finite automata nodes
 * In arg:        nodes, set of nodes
 * Return val:    Resulting set
 */
set<int> setAFNodeToSetInt(set<AFNode*> nodes){
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
    vector<char> leafs;
    //AFDode* start;
    vector <set<int>> states;
    vector <vector <int>> transitions;
    set<string> alphabet;
    void followpos(Node* node);
    set<int> getFollowpos(int id);
    char getLetter(int id);
    int getNumber(char letter);
/*---------------------------------------------------------------------
 * Contructor
 * In arg:        start, the root of the syntax tree    alphabet, the automata alphabet
 */
    AFDirect(Node* start, set<string> alphabet){
        followpos(start);
        this->alphabet = alphabet;
        queue<set<int>> pendingStates;
        states.push_back(setNodeToSetInt(start->firstpos));
        pendingStates.push(setNodeToSetInt(start->firstpos));
        while (!pendingStates.empty()){
            vector<int> temporalTransitions;
            for (auto const &e: alphabet) {
                set<int> newState;
                set<int> result;
                for (auto const &h: pendingStates.front()) {
                    if(getLetter(h) == e[0]){
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
    set<string> alphabet, vector<int> ids, vector<char> leafs){
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
        if (root->data == '.'){
            for (auto const &e: lastpos(root->left)) {
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
                result = setNodeToSetInt(firstpos(root->right));
                followposV[pos].insert(result.begin(), result.end());
            } 

        } else if (root->data == '*'){
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
        } else if (root->data != '|'){ //create reference table
            int pos  = -1;
            for (int i = 0; i < ids.size(); i = i + 1){
                if (root->id== ids[i]){
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
char AFDirect::getLetter(int id){
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
int AFDirect::getNumber(char letter){
    for (int i = 0; i < leafs.size(); i= i +1){
        if (leafs[i] == letter){
            return ids[i];
        }
    }
    return -1;
}

/*---------------------------------------------------------------------
 * Function:      writeAFDirect
 * Purpose:       Write the direct deterministic finite automata
 * In arg:        afdirect, the direct deterministic finite automata       name, the name of the file
 * Return val:    -------
 */
void writeAFDirect(AFDirect* afdirect, string name){
    fstream my_file;
	my_file.open(name, ios::out);
    //cout << nodes.size() << "\n";
	if (!my_file) {
		cout << "File not created!";
	}
	else {
		cout << "File created successfully!" << "\n";
        int finalNum = afdirect->getNumber('#');
        for (int i = 0; i < afdirect->states.size(); i = i + 1){
            if (afdirect->states[i].find(finalNum) != afdirect->states[i].end()){
                my_file << i << "\n";
            }
        }
        string s = setToString(afdirect->alphabet);
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
 * Function:      printAFD
 * Purpose:       Print in console the deterministic finite automata
 * In arg:        afd, the deterministic finite automata 
 * Return val:    -------
 */
void printAFD(AFD* afd){
    cout << "///////////////////////deterministic finite automata/////////////////////// \n";
    cout << "=========== States =========== \n";
    for (int i = 0; i <  afd->states.size(); i = i + 1){
        cout << i << " " << printSet(afd->states[i]) << "\n";
    }
    cout << "=============================== \n";
    cout << "=========== Transitions =========== \n";
    string s = setToString(afd->alphabet);
    string trans = "    ";
    for (int j = 0; j <  s.size(); j = j + 1){
        trans = trans + s[j] + "   ";
    }
    cout << trans << "\n";
    for (int i = 0; i <  afd->states.size(); i = i + 1){\
        trans = to_string(i) + "   ";
        for (int j = 0; j <  s.size(); j = j + 1){
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
    string s = setToString(afd->alphabet);
    string trans = "    ";
    for (int j = 0; j <  s.size(); j = j + 1){
        trans = trans + s[j] + "   ";
    }
    cout << trans << "\n";
    for (int i = 0; i <  afd->states.size(); i = i + 1){\
        trans = to_string(i) + "   ";
        for (int j = 0; j <  s.size(); j = j + 1){
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
 * Function:      simulateAFN
 * Purpose:       Simulate a non-deterministic finite automata for a input chain
 * In arg:        initialState, the lock of the initial node of the  non-deterministic finite automata     chain, the input chain
 * Return val:    -------
 */
bool simulateAFN(set<AFNode*> initialState, string chain){
    bool result = false;
    set<AFNode*> resultSet = initialState;
    for (int i = 0; i < chain.size(); i++){
        resultSet = lock(move(resultSet, chain[i]));
    }
    for (auto const &e: resultSet) {
        if (e->id == 9999) {
            result = true;
        }
    }  
    return result;
}
/*---------------------------------------------------------------------
 * Function:      simulateAFD
 * Purpose:       Simulate a deterministic finite automata for a input chain
 * In arg:        afd, the deterministic finite automata     chain, the input chain
 * Return val:    -------
 */
bool simulateAFD(AFD* afd, string chain){
    bool result = false;
    int currentState = 0;
    for (int i = 0; i < chain.size(); i++){
        int cont = 0 ;
        int transition = -1 ;//check if it does not change
        for (auto const &e: afd->alphabet) {
            if (e[0] == chain[i]) {
                transition = cont;
            }
            cont = cont + 1;
        } 
        if (transition == -1){
            return 0;
        }
        //cout << currentState << " " << transition << " " << afd->transitions[i].size() << "\n";
        currentState = afd->transitions[currentState][transition];
        if (currentState == -1){
            return 0;
        }
    }
    for (auto const &e: afd->states[currentState]) {
        if (e->id == 9999) {
            result = true;
        }
    }  
    return result;
}
/*---------------------------------------------------------------------
 * Function:      simulateAFDirect
 * Purpose:       Simulate a direct deterministic finite automata for a input chain
 * In arg:        afd, the direct deterministic finite automata     chain, the input chain
 * Return val:    -------
 */
bool simulateAFDirect(AFDirect* afd, string chain){
    bool result = false;
    int currentState = 0;
    for (int i = 0; i < chain.size(); i++){
        int cont = 0 ;
        int transition = -1 ;//check if it does not change
        for (auto const &e: afd->alphabet) {
            if (e[0] == chain[i]) {
                transition = cont;
            }
            cont = cont + 1;
        } 
        if (transition == -1){
            return 0;
        }
        currentState = afd->transitions[currentState][transition];
        if (currentState == -1){
            return 0;
        }
    }
    int finalNum = afd->getNumber('#');
    return afd->states[currentState].find(finalNum) != afd->states[currentState].end();
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
    partition.insert(temp1);
    partition.insert(temp2);
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
    vector<char> leafs;
    leafs.push_back('#');
    return new AFDirect(finalStates, finalTransitions, alphabet, ids, leafs);
}

/*---------------------------------------------------------------------
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
    string expr = expand(argv[1]); //asign the regex expresion
    string chain = argv[2];
    set<string> alphabet = getAlphabet(expr);
    set<string> chainAlphabet = getAlphabet(chain);
    if (!correctParentesis(expr)){
        cout << "Error: Missing parentesis\n";
        cout << "Check your expression and try again\n";
        return 0;
    }
    if (!checkAlphabet(alphabet, chainAlphabet)){
        cout << "Error: The alphabet of the regular expression and the input string are not the same\n";
        cout << "Check your expression and try again\n";
        return 0;
    }
    try {
        cout << expr << "\n";
        SyntaxTree* tree = new SyntaxTree(expr);
        
        cout << "///////////////////////Binary tree/////////////////////// \n";
        printTree(tree->root, 0);
        cout << "//////////////////////////////////////////////////////// \n";
        AFN* afn = new AFN(tree->root);
        afn->end->id = 9999;
        writeAFN(afn->start);
        set<AFNode*> initialState;
        initialState.insert(afn->start);
        initialState = lock(initialState);
        AFD* afd = new AFD(initialState, alphabet);
        writeAFD(afd);
        printAFD(afd);
        vector <set<int>> statesVector;
        for (int i = 0; i < afd->states.size(); i = i + 1) {
            statesVector.push_back(setAFNodeToSetInt(afd->states[i]));
        }
        cout << "///////////////////////Minimization/////////////////////// \n";
        AFDirect* afdmini = minimization(statesVector, afd->alphabet,afd->transitions, 9999);
        printAFDirect(afdmini);
        writeAFDirect(afdmini, "afdmini.txt");
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
        AFDirect* afdirectmini = minimization(afdirect->states, afdirect->alphabet,afdirect->transitions, afdirect->getNumber('#'));
        printAFDirect(afdirectmini);
        writeAFDirect(afdirectmini, "afdirectmini.txt");
        auto start = high_resolution_clock::now();
        cout << "Non deterministic finite automata: " << (simulateAFN(initialState, chain) ? "approved\n" : "rejected\n");
        auto stop = high_resolution_clock::now(); 
        auto duration = duration_cast<microseconds>(stop - start); 
        cout << "Execution time(microseconds): " << duration.count() << endl; 
        start = high_resolution_clock::now();
        cout << "deterministic finite automata: " << (simulateAFD(afd, chain)? "approved\n" : "rejected\n");
        stop = high_resolution_clock::now(); 
        duration = duration_cast<microseconds>(stop - start);
        cout << "Execution time(microseconds): " << duration.count() << endl;  
        start = high_resolution_clock::now();
        cout << "Direct deterministic finite automata: " << (simulateAFDirect(afdirect, chain)? "approved\n" : "rejected\n");
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start); 
        cout << "Execution time(microseconds): " << duration.count() << endl;  
    } catch (std::exception& e) {
        cout << "Error: An error ocurred\n";
        cout << "Check your expression and try again\n";
    }
    return 0;
}