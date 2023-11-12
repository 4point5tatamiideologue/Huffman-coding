#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <stdio.h>
#include <math.h>
using namespace std;



// node structures

struct Node {
	float weight;
	Node* parent;
};

struct leafNode : Node {
	char symbol;
	leafNode(float weight, char symbol) {
		this->weight = weight;
		this->symbol = symbol;
	}
};

struct internalNode : Node {
	Node* leftChild;
	Node* rightChild;
	internalNode(float weight, Node* leftChild, Node* rightChild) {
		this->weight = weight;
		this->leftChild = leftChild;
		this->rightChild = rightChild;
	}
};



// functions

internalNode* newInternalNode(Node* child1, Node* child2) {
	float newWeight = child1->weight + child2->weight;
	internalNode* newNode = new internalNode(newWeight, child1, child2);
	child1->parent = newNode;
	child2->parent = newNode;
	return newNode;
}

// recursive function that returns the code (i.e. the path from
// the root to the node) of any node
string Code(Node* node, Node* root, string codeString = "") {
	// if the node is the root, do nothing
	if (node == root)
		return "";
	else {
		codeString += Code(node->parent, root, codeString);
		// if the node is a left child, append 0, else 1.
		if (node == static_cast<internalNode*>(node->parent)->leftChild)
			codeString += "0";
		else
			codeString += "1";
		return codeString;
	}
}



int main() {

	// read the input file and store relative frequencies and
	// symbols in a vector of pairs, with probability in the first place
	ifstream inputFile("input.txt");
	string symbolString;
	string frequencyString;
	vector<pair<float, char>> input;
	int count = 0;
	while (!inputFile.eof()) {
		getline(inputFile, symbolString, ';');
		getline(inputFile, frequencyString, '\n');
		char symbol = symbolString[0];
		float frequency = stof(frequencyString);
		input.push_back({ frequency, symbol });
		count++;
	}
	inputFile.close();

	// create a copy of the input vector to be sorted
	vector<pair<float, char>> inputSorted(input.size());
	for (int i = 0; i < input.size(); i++) {
		inputSorted[i] = input[i];
	}

	// sort pairs by weight
	sort(inputSorted.begin(), inputSorted.end());

	// for each symbol, create leaf
	vector<leafNode*> leafNodes(input.size());
	for (int i = 0; i < leafNodes.size(); i++) {
		float weight = inputSorted[i].first;
		char symbol = inputSorted[i].second;
		leafNodes[i] = new leafNode(weight, symbol);
	}

	// create queues and enqueue leaf nodes in queue1
	queue<leafNode*> queue1;
	queue<internalNode*> queue2;
	for (int i = 0; i < leafNodes.size(); i++)
		queue1.push(leafNodes[i]);


	// Huffman's algorithm
	Node* child1;
	Node* child2;

	do {
		if (queue1.empty()) {
			child1 = queue2.front();
			queue2.pop();
			child2 = queue2.front();
			queue2.pop();
			queue2.push(newInternalNode(child1, child2));
		}
		else if (queue2.empty()) {
			child1 = queue1.front();
			queue1.pop();
			child2 = queue1.front();
			queue1.pop();
			queue2.push(newInternalNode(child1, child2));
		}
		else {
			if (queue1.front()->weight <= queue2.front()->weight) {
				child1 = queue1.front();
				queue1.pop();
			}
			else {
				child1 = queue2.front();
				queue2.pop();
			}
			if (queue1.empty()) {
				child2 = queue2.front();
				queue2.pop();
			}
			else if (queue2.empty()) {
				child2 = queue1.front();
				queue1.pop();
			}
			else {
				if (queue1.front()->weight <= queue2.front()->weight) {
					child2 = queue1.front();
					queue1.pop();
				}
				else {
					child2 = queue2.front();
					queue2.pop();
				}
			}
			queue2.push(newInternalNode(child1, child2));
		}
	} while (!queue1.empty() || queue2.size() > 1);

	Node* root = queue2.front();


	// encoding
	vector<pair<char, string>> output(input.size());
	for (int i = 0; i < output.size(); i++) {
		output[i] = make_pair(leafNodes[i]->symbol, Code(leafNodes[i], root));
	}

	// put the symbols back into the order they were in before they were sorted
	for (int i = 0; i < input.size(); i++) {
		for (int j = i; j < input.size(); j++) {
			// input[i].second is a symbol and so is output[j].first
			if (input[i].second == output[j].first) {
				swap(output[i], output[j]);
				break;
			}
		}
	}

	// print output to a file
	ofstream outputFile("output.txt");
	for (int i = 0; i < output.size(); i++) {
		char symbol = output[i].first;
		string codeword = output[i].second;
		outputFile << symbol << ";" << codeword << "\n";
	}
	outputFile.close();
	cout << "The code has been saved to file \"output.txt\".\n";

	// evaluate performance
	float expectedCodewordLength = 0;
	float entropy = 0;
	for (int i = 0; i < input.size(); i++) {
		float probability = input[i].first;
		int codewordLength = output[i].second.length();
		// formula (2) in the paper
		expectedCodewordLength += probability * codewordLength;
	}
	for (int i = 0; i < input.size(); i++) {
		float probability = input[i].first;
		// formula (3) in the paper
		entropy += probability * log2(1 / probability);
	}
	int fixedCodewordLength = ceil(log2(input.size()));
	cout << "The expected codeword length is "
		<< expectedCodewordLength << ".\n";
	cout << "The entropy of the source is "
		<< entropy << ".\n";
	cout << "A fixed-length code would have codeword lengths of "
		<< fixedCodewordLength << ".\n";

	return 0;

}