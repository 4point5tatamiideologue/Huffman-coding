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
// destroy_huffman_tree() should be called if the tree will no longer be used.
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

void destroy_huffman_tree(Node* root) {
	if (root == NULL) {
		return;
	}
	destroy_huffman_tree(root->left_child);
	destroy_huffman_tree(root->right_child);
	delete(root);
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
	std::ifstream input_file("../test/input_text.txt");
	char symbol;
	std::map<char, int> frequencies;
	int character_count;
	while (!input_file.eof()) {
		input_file.get(symbol);
		frequencies[symbol]++;
		character_count++;
	}
	input_file.close();

	std::vector<std::pair<float, char>> symbols_sorted_by_weight;
	for (auto const& [symbol, frequency] : frequencies) {
		symbols_sorted_by_weight.push_back(std::make_pair((float) frequency / character_count, symbol));
	}

	// Sort pairs by frequency.
	std::sort(symbols_sorted_by_weight.begin(), symbols_sorted_by_weight.end());

	Node* root = build_huffman_tree(symbols_sorted_by_weight);
	std::map<char, std::string> code_map = build_code_map(root);
	destroy_huffman_tree(root);

	// Encode each symbol.
	std::ifstream input_file2("../test/input_text.txt");
	std::ofstream output_file("output.txt");
	std::string codeword;
	while (!input_file2.eof()) {
		input_file2.get(symbol);
		codeword = code_map[symbol];
		output_file << codeword;
	}
	input_file2.close();
	output_file.close();
	std::cout << "The compressed file has been saved in \"output.txt\".\n";

}