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
////////Tree external methods
/*
string sustitutions(string expr){
    string r = "";
    for (int i = 0; i < expr.length(); i = i + 1){
        if (expr[i] == '?'){
            r = r + expr[i];
        } else if (expr[i] == '+'){
            r = r + "";
        } else {
            r = r + expr[i];
        }
    }
}*/
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
        } else if (expr[i] == ')' || expr[i] == '*'){
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

bool isOperand(char v){
    string ops = "()|.*";
    for (int i = 0; i < ops.length(); i = i + 1){
        if (ops[i] == v)
            return 1;
    }
    return 0;
}

int precedence(char v){
    string ops = "()|.*";
    for (int i = 0; i < ops.length(); i = i + 1){
        if (ops[i] == v)
            return i;
    }
    return -1;
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

class Tree{
  public:
  Node* root;
  Tree(string expr){
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
                    while (precedence(expr[i]) < precedence(opStack.top())){
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

    stack<Node*> tree;
    for (int i=0; i < result.size(); i=i+1){
        if (isOperand(result[i])){
            if (result[i] == '*'){
                Node* temp = new Node('*', tree.top(), NULL);
                tree.top() = temp;
            } else if (result[i] == '.' || result[i] == '|'){
                Node* firstValue = tree.top();
                tree.pop();
                Node* temp = new Node(result[i], tree.top(), firstValue);
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

set<int> toIntSet(set<Node*> nodes){
    set<int> result;
    for (auto const &e: nodes) {
        result.insert(e->id);
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
        if (s[i] != '.' && s[i] != '*' && s[i] != '|' && s[i] != '(' && s[i] != ')'){
            string t(1,s[i]);
            result.insert(t);
        }
    }
    return result;
}

set<AFNode*> lock(set<AFNode*> initialNodes){
    set<AFNode*> result;
    stack<AFNode*> nodes;
    for (auto const &e: initialNodes) {
        nodes.push(e);
        result.insert(e);
    }
    while (!nodes.empty()){
        AFNode* temporalNode =  nodes.top();
        nodes.pop();
        for (int i = 0; i < temporalNode->edges.size(); i = i + 1){
            if (temporalNode->edges[i].first == 'e'){
                result.insert(temporalNode->edges[i].second);
                nodes.push(temporalNode->edges[i].second);
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
        while (!pendingStates.empty()){
            vector<int> temporalTransitions;
            for (auto const &e: alphabet) {
                set<AFNode*> temporalSet;
                temporalSet = lock(move(pendingStates.front(), e[0]));
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
                    while (precedence(expr[i]) < precedence(opStack.top())){
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
                cout << printIntSet(pendingStates.front()) << " " << e << " " << printIntSet(newState) << pos << "\n";
            }
            transitions.push_back(temporalTransitions);
            pendingStates.pop();
        }
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
                result = toIntSet(firstpos(root->right));
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
                result = toIntSet(root->firstpos);
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
    for (int i = 0; i <  nodes.size(); i = i + 1){
        cout << nodes[i] << " " << printIntSet(followposV[i]) << "\n";
    }
    for (int i = 0; i < leafs.size(); i = i + 1){
        cout << ids[i] << " " << leafs[i] << "\n";
    }
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

void writeAFDirect(AFDirect* afdirect){
    fstream my_file;
	my_file.open("afdirect.txt", ios::out);
    //cout << nodes.size() << "\n";
	if (!my_file) {
		cout << "File not created!";
	}
	else {
		cout << "File created successfully!" << "\n";
        //optener id del final
        //revisar sets 
        //write transitions
        string s = setToString(afdirect->alphabet);
        cout << s << "/n";
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

int main(int argc, char **argv) {
    //stack<Node*> tree; 
    string expr = expand(argv[1]); //asign the regex expresion
    cout << expr << "\n";
    Tree* tree = new Tree(expr);
    AFN* afn = new AFN(tree->root);
    afn->end->id = 9999;
    writeAFN(afn->start);
    set<string> alphabet = getAlphabet(expr);
    set<AFNode*> initialState;
    initialState.insert(afn->start);
    initialState = lock(initialState);
    AFD* afd = new AFD(initialState, alphabet);
    writeAFD(afd);
    expr = expr + ".#";
    cout << expr << "\n";
    SyntaxTree* syntaxtree = new SyntaxTree(expr);
    fillFunctions(syntaxtree->root);
    printSyntaxTree(syntaxtree->root, 0);
    AFDirect* afdirect = new AFDirect(syntaxtree->root, alphabet);
    writeAFDirect(afdirect);
    return 0;

}