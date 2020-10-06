#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#include <vector>
#include <iostream>
#include <queue>
#include <algorithm>
#include <string>
#include <functional>
#include <fstream>
#include <bitset>

#include "node.h"

using queue_t = std::priority_queue<Node::ptr, std::vector<Node::ptr>, std::greater<Node::ptr>>;

#define CHECK_STREAM(stream, do_if) {  \
	if (!stream) {                     \
		cerr << "Error in [" << __FUNCSIG__ << "]: " << strerror(errno) << "Line: " << __LINE__ << endl; \
		do_if;                         \
	}                                  \
}

// get frequency of each char in file
std::vector<int> get_frequencies(std::ifstream& ifs);

// fills priority queue by frequencieses
void fill_queue(queue_t& q, const std::vector<int>& f);

// creates a binary tree from priority queue
void build_tree(queue_t& queue);

// make a fill vector with codes for each char
void make_code(Node::ptr& node, std::string str, std::vector<std::string>& codes);

// creates an encoded message
std::string msg_encoded(const std::string& filename, const std::vector<std::string>& codes);

void write_file_compressed(
	const std::string& filename, const std::vector<int>& frequencies, const queue_t& queue,
	const std::string& msg
);

// decodes encoded message and writes it in text
void decode(const Node::ptr& root, std::string& text, const std::string& msg_encoded);

//decodes a .hff file
std::string read_encoded_file(const std::string& filename_hff);

// compress a file [filename] and creates a compressed
// file [filename].hff
void compress(const std::string& filename);
