#include <iostream>
#include <bits/stdc++.h> 
#include <utility> 
#include <fstream>
#include <string>
#include <stack>
#include <queue>
#include <algorithm>
#include<set>

using namespace std;

bool isOperand(char v){
    string ops = "()|.*?+";
    for (int i = 0; i < ops.length(); i = i + 1){
        if (ops[i] == v)
            return 1;
    }
    return 0;
}

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
  Node(char d, Node* left, Node* right){
    id = rand() % 5000 + 1;
    data=d;
    this->left=left;
    this->right=right;
  }
};
//expand string to include concat as a symbol
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

set<string> generateTree(Node* root, set<string> nodes){
    string s(1, root->data);
    if (root->left != NULL){
        Node* leftNode = root->left;
        string t(1, leftNode->data);
        nodes.insert(s+" "+t);
        nodes = generateTree(leftNode, nodes);
    } 
    if (root->right != NULL){
        Node* rightNode = root->right;
        string t(1, rightNode->data);
        nodes.insert(s+" "+t);
        nodes = generateTree(rightNode, nodes);
    } 
    return nodes;
}

void writeTree(Node* root){
    set<string> nodes;
    nodes = generateTree(root, nodes);
    fstream my_file;
	my_file.open("tree.txt", ios::out);
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

class AFNode{
    public:
    int id;
    vector <pair<char, AFNode*>> edges;
    void addEdge(char values, AFNode* node);
    AFNode(){
        id = rand() % 5000 + 1;
    }
};

void AFNode::addEdge(char value, AFNode* node){
    edges.push_back(make_pair(value, node)); 
}

class AFN{
    public:
    AFNode* start;
    AFNode* end;
    AFN(Node* root){
        string empty = "e";
        AFN* operand1;
        AFN* operand2;
        switch(root->data){
            case '*':
                //cout << "Estoy en * \n";
                //crear nodo inicial
                start = new AFNode();
                //crear nodo final
                end = new AFNode();
                //crear transicion de inicial a final
                start->addEdge(empty[0], end);
                //llamar a automata de hijos
                operand1 = new AFN(root->left);

                //crear transicion de final de hijo a inicio de hijo
                //crear transicion de inicial a hijo
                //crear transicion de hijo a final
                start->addEdge(empty[0], operand1->start);
                operand1->end->addEdge(empty[0], end);
                operand1->end->addEdge(empty[0], operand1->start);
                //cout << "sali de * \n";
            break;
            case '|':
                //cout << "Estoy en | \n";
                //crear nodo inicial
                start = new AFNode();
                //crear nodo final
                end = new AFNode();

                //llamar a automata de hijos
                operand1 = new AFN(root->left);
                //llamar a automata de hijos
                operand2 = new AFN(root->right);

                //crear transici[on] de inicial a hijo con e
                //crear transici[on] de hijo a final
                start->addEdge(empty[0], operand1->start);
                start->addEdge(empty[0], operand2->start);
                //crear transici[on] de inicial a hijo con e
                //crear transici[on] de hijo a final
                operand1->end->addEdge(empty[0], end);
                operand2->end->addEdge(empty[0], end);
                //cout << "Sali de |\n";
            break;
            case '.':
                //cout << "Estoy en . \n";
                //llamar a automata de hijos
                operand1 = new AFN(root->left);
                //llamar a automata de hijos
                operand2 = new AFN(root->right);
                //crear transici[on] de hijo a hijo
                //primer hijo se vuelve el inicio
                //vector <pair<char, AFNode*>> Temporaledges;
                //cout << "Creo que es aqui ... valor: " << operand2->start->edges.size() <<"\n";
                //cout << "Creo que es aqui ... valor: " << operand1->end->id << "\n";
                operand1->end->edges = operand2->start->edges;
                //cout << "No no lo es \n";
                operand2->start = operand1->end;
                start = operand1->start;
                end = operand2->end;
                //cout << "Sali de . \n";
            break; //add terminal character
            case '#':
                start = new AFNode();
                end = new AFNode();
                end->id = 9999;
                start->addEdge('#', end);
                //cout << "Sali de " << root->data << "\n";
            break; //add terminal character
            default:
                //cout << "Estoy en " << root->data << "\n";
                //crear nodo inicial
                start = new AFNode();
                //crear nodo final
                end = new AFNode();
                //crear transici[on]
                start->addEdge(root->data, end);
                //cout << "Sali de " << root->data << "\n";
        }
    }
        //Simular cadena, moverse
};

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

string printSet(set<AFNode*> nodes){
    string result = "";
    for (auto const &e: nodes) {
        result = result + to_string(e->id) + " ";
    }
    return "{" + result + "} ";
}

class AFD{
    public:
    //AFDode* start;
    vector <set<AFNode*>> states;
    vector <vector <int>> transitions;
    set<string> alphabet;
    AFD(set<AFNode*> start, set<string> alphabet){
        this->alphabet = alphabet;
        states.push_back(start);
        queue<set<AFNode*>> pendingStates;
        pendingStates.push(start);
        cout << "Estado inicial " << printSet(pendingStates.front()) << "\n";
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

string setToString(set<string> s){
    string result = "";
    for (auto const &e: s) {
        result = result + e;
    }
    return result;
}

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

class SyntaxTree{
  public:
  Node* root;
  SyntaxTree(string expr){
    stack<char> opStack; 
    //Read the expression
    string result;
    for (int i = 0; i < expr.length(); i = i + 1){
        if (isOperand(expr[i])){
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
    cout << "resultado:" << result << "\n";
    stack<Node*> tree;
    for (int i=0; i < result.size(); i=i+1){
        if (isOperand(result[i])){
            if (result[i] == '*'){
                //cout << tree.front()->data << "\n";
                Node* temp = new Node('*', tree.top(), NULL);
                tree.top() = temp;
            } else if (result[i] == '.' || result[i] == '|'){
                Node* firstValue = tree.top();
                tree.pop();
                Node* temp = new Node(result[i], tree.top(), firstValue);
                tree.top() = temp;
            } else if (result[i] == '+'){
                Node* firstValue = new Node('*', tree.top(), NULL);
                Node* temp = new Node('.', tree.top(), firstValue);
                tree.top() = temp;
            } else if (result[i] == '?'){
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

string printNode(set<Node*> nodes){
    string result = "";
    for (auto const &e: nodes) {
        result = result + " " + to_string(e->id);
    }
    return "{" + result + " }";
}

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

string printIntSet(set<int> set){
    string result ;
    for (auto const &e: set) {
        result = result + to_string(e) + " ";
    }   
    return "{ " + result + "}";
}

set<int> setNodeToSetInt(set<Node*> nodes){
    set<int> result;
    for (auto const &e: nodes) {
        result.insert(e->id);
    }
    return result;
}

set<int> setAFNodeToSetInt(set<AFNode*> nodes){
    set<int> result;
    for (auto const &e: nodes) {
        result.insert(e->id);
    }
    return result;
}

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

    AFDirect( vector <set<int>> states, vector <vector <int>> transitions, 
    set<string> alphabet, vector<int> ids, vector<char> leafs){
        this->alphabet = alphabet;
        this->states = states;
        this->transitions = transitions;
        this->leafs = leafs;
        this->ids = ids;
    }
};

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
    /*
    for (int i = 0; i <  nodes.size(); i = i + 1){
        cout << nodes[i] << " " << printIntSet(followposV[i]) << "\n";
    }
    for (int i = 0; i < leafs.size(); i = i + 1){
        cout << ids[i] << " " << leafs[i] << "\n";
    }*/
}

set<int> AFDirect::getFollowpos(int id){
    set<int> result;
    for (int i = 0; i < nodes.size(); i = i + 1){
        if (nodes[i] == id){
            result = followposV[i];
        }
    }
    return result;
}

char AFDirect::getLetter(int id){
    for (int i = 0; i < ids.size(); i= i +1){
        if (ids[i] == id){
            return leafs[i];
        }
    }
}

int AFDirect::getNumber(char letter){
    for (int i = 0; i < leafs.size(); i= i +1){
        if (leafs[i] == letter){
            return ids[i];
        }
    }
    return -1;
}

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

void printAFD(AFD* afd){
    cout << "///////////////////////Deterministic automata/////////////////////// \n";
    cout << "=========== Estados =========== \n";
    for (int i = 0; i <  afd->states.size(); i = i + 1){
        cout << i << " " << printSet(afd->states[i]) << "\n";
    }
    cout << "=============================== \n";
    cout << "=========== Transiciones =========== \n";
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

void printAFDirect(AFDirect* afd){
    cout << "///////////////////////Deterministic direct automata/////////////////////// \n";
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
    cout << "=========== Estados =========== \n";
    for (int i = 0; i <  afd->states.size(); i = i + 1){
        cout << i << " " << printIntSet(afd->states[i]) << "\n";
    }
    cout << "=============================== \n";
    cout << "=========== Transiciones =========== \n";
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

vector<int> setIntToVectorInt(set<int> set){
    vector<int> result;
    for (auto const &e: set) {
        result.push_back(e);
    }
    return result;
}

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

AFDirect* minimization(vector <set<int>> states, set<string> alphabet,vector <vector <int>> transitions){
    set<int> temp1;
    set<int> temp2;
    //Separate sets
    set<int> terminals;
    for (int i = 0; i < states.size(); i = i + 1){
        //set<int> intSet =setAFNodeToSetInt(states[i]);
        if (states[i].find(9999) == states[i].end() ){
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

int main(int argc, char **argv) { 
    string expr = expand(argv[1]); //asign the regex expresion
    string chain = argv[2];

    ///check that chain contains the correct alphabet
    ///check parentesis and operators  
    cout << expr << "\n";
    SyntaxTree* tree = new SyntaxTree(expr);
    cout << "///////////////////////Binary tree/////////////////////// \n";
    printTree(tree->root, 0);
    cout << "//////////////////////////////////////////////////////// \n";
    AFN* afn = new AFN(tree->root);
    afn->end->id = 9999;
    writeAFN(afn->start);
    set<string> alphabet = getAlphabet(expr);
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
    AFDirect* afdmini = minimization(statesVector, afd->alphabet,afd->transitions);
    writeAFDirect(afdmini, "afdmini.txt");
    printAFDirect(afdmini);
    expr = expr + ".#";
    SyntaxTree* syntaxtree = new SyntaxTree(expr);
    fillFunctions(syntaxtree->root);
    cout << "///////////////////////Binary tree/////////////////////// \n";
    printSyntaxTree(syntaxtree->root, 0);
    cout << "//////////////////////////////////////////////////////// \n";
    AFDirect* afdirect = new AFDirect(syntaxtree->root, alphabet);
    writeAFDirect(afdirect, "afdirect.txt");
    printAFDirect(afdirect);
    cout << "Non deterministic automata: " << (simulateAFN(initialState, chain) ? "approved\n" : "rejected\n");
    cout << "Deterministic automata: " << (simulateAFD(afd, chain)? "approved\n" : "rejected\n");
    cout << "Direct deterministic automata: " << (simulateAFDirect(afdirect, chain)? "approved\n" : "rejected\n");
    return 0;
}