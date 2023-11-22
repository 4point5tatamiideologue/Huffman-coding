#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <map>

struct Node {
	float weight;
	Node* left_child = NULL;
	Node* right_child = NULL;
};

struct LeafNode : Node {
	char symbol;
	LeafNode(float weight, char symbol) {
		this->weight = weight;
		this->symbol = symbol;
	}
};

struct InternalNode : Node {
	InternalNode(Node* left_child, Node* right_child) {
		this->weight = left_child->weight + right_child->weight;
		this->left_child = left_child;
		this->right_child = right_child;
	}
};

// Given a vector consisting of (weight, symbol) pairs, sorted by weight, builds a Huffman tree and returns a pointer to the root.
Node* build_huffman_tree(std::vector<std::pair<float, char>> sorted_input) {
	// For each symbol, create a leaf node.
	std::vector<LeafNode*> leaf_nodes(sorted_input.size());
	for (int i = 0; i < leaf_nodes.size(); i++) {
		float weight = sorted_input[i].first;
		char symbol = sorted_input[i].second;
		leaf_nodes[i] = new LeafNode(weight, symbol);
	}

	// Create queues and enqueue leaf nodes in queue1.
	std::queue<LeafNode*> queue1;
	std::queue<InternalNode*> queue2;
	for (int i = 0; i < leaf_nodes.size(); i++)
		queue1.push(leaf_nodes[i]);


	Node* child1;
	Node* child2;

	// In each iteration of this loop, an internal node is created by combining the two nodes with smallest weights, from either queues.
	do {
		if (queue1.empty()) {
			child1 = queue2.front();
			queue2.pop();
			child2 = queue2.front();
			queue2.pop();
			queue2.push(new InternalNode(child1, child2));
		}
		else if (queue2.empty()) {
			child1 = queue1.front();
			queue1.pop();
			child2 = queue1.front();
			queue1.pop();
			queue2.push(new InternalNode(child1, child2));
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
			queue2.push(new InternalNode(child1, child2));
		}
	} while (!queue1.empty() || queue2.size() > 1);

	return queue2.front(); // This is the root.
}

void build_code_map_helper(Node* node, std::string codeword, std::map<char, std::string> &code_map) {
	if (node == NULL) {
		return;
	} else if (node->left_child == NULL && node->right_child == NULL) {
		LeafNode* leaf_node = static_cast<LeafNode*>(node);
		code_map[leaf_node->symbol] = codeword;
		return;
	}
	build_code_map_helper(node->left_child, codeword + "0", code_map);
	build_code_map_helper(node->right_child, codeword + "1", code_map); 
}

// Builds a map for symbol to codeword, given a pointer to the root of a Huffman tree.
std::map<char, std::string> build_code_map(Node* root) {
	std::map<char, std::string> code_map;
	build_code_map_helper(root, "", code_map);
	return code_map;	
}

bool compare_symbol(std::pair<float, char> pair1, std::pair<float, char> pair2) {
	return pair1.second < pair2.second; 
}



int main() {
	// Read the input file and store relative frequencies and symbols in a vector of pairs.
	std::ifstream input_file("../test/input.txt");
	std::string symbol_string;
	std::string weight_string;
	std::vector<std::pair<float, char>> input;
	while (!input_file.eof()) {
		getline(input_file, symbol_string, ';');
		getline(input_file, weight_string, '\n');
		char symbol = symbol_string[0];
		float weight = stof(weight_string);
		input.push_back({ weight, symbol });
	}
	input_file.close();

	// Sort pairs by weight.
	std::sort(input.begin(), input.end());

	Node* root = build_huffman_tree(input);
	std::map<char, std::string> code_map = build_code_map(root);

	// Sort the input elements by lexicographic order for cleaner output.
	sort(input.begin(), input.end(), compare_symbol);

	// Encode each symbol.
	std::vector<std::pair<char, std::string>> output(input.size());
	for (int i = 0; i < input.size(); i++) {
		char symbol = input[i].second;
		output[i] = make_pair(symbol, code_map[symbol]);
	}

	// Save the output in a file.
	std::ofstream output_file("output.txt");
	for (int i = 0; i < output.size(); i++) {
		char symbol = output[i].first;
		std::string codeword = output[i].second;
		output_file << symbol << ";" << codeword << "\n";
	}
	output_file.close();
	std::cout << "The code has been saved in file \"output.txt\".\n";

	// Evaluate the performance.
	float expected_codeword_length = 0;
	float entropy = 0;
	for (int i = 0; i < input.size(); i++) {
		float weight = input[i].first;
		int codeword_length = output[i].second.length();
		expected_codeword_length += weight * codeword_length;
	}
	for (int i = 0; i < input.size(); i++) {
		float weight = input[i].first;
		entropy += weight * log2(1 / weight);
	}
	int fixed_codeword_length = ceil(log2(input.size()));
	std::cout << "The expected codeword length is "
		<< expected_codeword_length << ".\n";
	std::cout << "The entropy of the source is "
		<< entropy << ".\n";
	std::cout << "A fixed-length code would have codeword lengths of "
		<< fixed_codeword_length << ".\n";
	return 0;

}