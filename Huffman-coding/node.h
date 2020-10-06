#pragma once

#include <iostream>
#include <memory>
#include <string>

using uchar = unsigned char;

class Node {

private:
	uchar ch{ 0 };
	int frequency{ 0 };
	std::string code_str{ "" };
	std::string name{ "" };

public:

	using ptr = std::shared_ptr<Node>;

	friend std::ostream& operator<<(std::ostream& os, const Node& node);

	ptr left{ nullptr };
	ptr right{ nullptr };
	ptr parent{ nullptr };


	Node();
	Node(uchar uch, int f);
	Node(const std::string& _name, int f);

	uchar get_char()    const;
	int get_frequency() const;

	std::string get_name() const;


	std::string code() const;

	void code(const std::string& c);

	bool is_leaf() const;

};

std::ostream& operator<<(std::ostream& os, const Node& node);

bool operator>(const Node::ptr& lhs, const Node::ptr& rhs);



