#include "node.h"

Node::Node() = default;
Node::Node(uchar uch, int f) : ch(uch), frequency(f) { }
Node::Node(const std::string & _name, int f) : name(_name), frequency(f) { }

uchar Node::get_char()    const { return ch; }
int Node::get_frequency() const { return frequency; }

std::string Node::get_name() const {
	return ch == 0 ? name :
		ch == 10 ? "\\n" :
		std::string(1, static_cast<char>(ch));
}


std::string Node::code() const {
	return code_str;
}

void Node::code(const std::string & c) {
	code_str = c;
}

bool Node::is_leaf() const {
	return !left && !right;
}

std::ostream& operator<<(std::ostream & os, const Node & node) {
	return os << "[" << node.get_name() << "] = " << node.frequency;
}

bool operator>(const Node::ptr & lhs, const Node::ptr & rhs) {
	return lhs->get_frequency() > rhs->get_frequency();
}