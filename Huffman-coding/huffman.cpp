#define _CRT_SECURE_NO_DEPRECATE

#include "huffman.h"

#define DEBUG

using namespace std;

vector<int> get_frequencies(ifstream& ifs) {

	vector<int> f(0x100, 0);
	char c;

	while (true) {

		ifs.read(&c, 1);

		if (ifs.eof()) break;

		++f[static_cast<unsigned char>(c)];
	}


#ifdef DEBUG

	cout << "Frequencies:\n";
	for (size_t i = 0; i < f.size(); ++i) {
		if (f[i] != 0) {
			cout << "[" << (uchar)i << " - " << f[i] << "]  ";
		}
	}
	cout << "\n";

#endif // DEBUG

	return f;
}

void fill_queue(queue_t& q, const vector<int>& f) {

	for (int i = 0; i < f.size(); ++i)
		if (f[i] != 0) {
			q.push(make_shared<Node>((uchar)i, f[i]));
		}
}

void build_tree(queue_t& queue) {

	while (queue.size() > 1) {

		Node::ptr x = queue.top();
		queue.pop();

		Node::ptr y = queue.top();
		queue.pop();

		string name = x->get_name() + y->get_name();

		Node::ptr z = make_shared<Node>(name, x->get_frequency() + y->get_frequency());

		z->left = x;
		z->right = y;

		x->parent = z;
		y->parent = z;

		queue.push(z);
	}
}

void make_code(Node::ptr& node, string str, vector<string>& codes) {

	if (node->left)
		make_code(node->left, str + "0", codes);

	if (node->right)
		make_code(node->right, str + "1", codes);

	if (!node->right and !node->left) {
		node->code(str);
		codes[node->get_char()] = str;

#ifdef DEBUG
		cout << "Leaf: " << "[" << *node << "] " << " code: " << node->code() << "\n";
#endif // DEBUG

	}
}

string msg_encoded(const string& filename, const vector<string>& codes) {

	string msg{ "" };

	ifstream ifs(filename, ifstream::binary);

	CHECK_STREAM(ifs, return msg);

	char c;
	while (true) {

		ifs.read(&c, 1);

		if (ifs.eof()) break;

		msg += codes[static_cast<uchar>(c)];
	}

	return msg;
}

void write_file_compressed(
	const string& filename, const vector<int>& frequencies, const queue_t& queue,
	const string& msg
) {
	string new_filename = filename + ".hff";
	ofstream ofs(new_filename, ofstream::binary);

	CHECK_STREAM(ofs, exit(-1));

	uchar count = count_if(
		frequencies.begin(), frequencies.end(),
		[](const int value) { return value != 0; }
	);

#ifdef DEBUG
	cout << "Char count: " << (int)count << endl;
#endif // DEBUG

	ofs.write(reinterpret_cast<char*>(&count), sizeof count);

	for_each(
		frequencies.begin(), frequencies.end(),
		[index = uchar{ 0 }, &ofs](int value) mutable {
		if (value != 0) {
			ofs.write(reinterpret_cast<char*>(&index), sizeof index);
			ofs.write(reinterpret_cast<char*>(&value), sizeof value);
		}
		++index;
	});

	int byte_count = msg.size() / CHAR_BIT;
	uchar modulo = msg.size() % CHAR_BIT;

#ifdef DEBUG
	cout << "byte count: " << byte_count << " of size " << msg.size() << endl;
	cout << "modulo: " << (int)modulo << " of size " << msg.size() << endl;
#endif // DEBUG

	ofs.write(reinterpret_cast<char*>(&byte_count), sizeof byte_count);
	ofs.write(reinterpret_cast<char*>(&modulo), sizeof modulo);

	int i = 0;
	for (; i < byte_count; ++i) {
		bitset<CHAR_BIT> b(msg.substr(i * CHAR_BIT, CHAR_BIT));
		uchar value = static_cast<uchar>(b.to_ulong());
		ofs.write(reinterpret_cast<char*>(&value), sizeof value);
	}
	if (modulo > 0) {
		bitset<CHAR_BIT> b(msg.substr(i * CHAR_BIT, CHAR_BIT));
		uchar value = static_cast<uchar>(b.to_ulong());
		ofs.write(reinterpret_cast<char*>(&value), sizeof value);
	}

}

void decode(const Node::ptr& root, string& text, const string& msg_encoded) {

	Node::ptr node = root;

	const auto letter_found = [&text, &root](Node::ptr& node) {
		text += node->get_char();
		node = root;
	};

	for (size_t i = 0; i < msg_encoded.size(); ++i) {

		char c = msg_encoded[i];

		if (c == '0') {
			if (node->left) {
				node = node->left;
				if (node->is_leaf()) {
					letter_found(node);
				}
			}
		}
		else if (c == '1') {
			if (node->right) {
				node = node->right;
				if (node->is_leaf()) {
					letter_found(node);
				}
			}
		}
	}

}

string read_encoded_file(const string& filename_hff) {

	ifstream ifs(filename_hff, ifstream::binary);
	vector<int> f(0x100, 0);

	CHECK_STREAM(ifs, exit(-1));

	uchar count = 0;
	ifs.read(reinterpret_cast<char*>(&count), sizeof count);


	int i = 0;
	while (i < count) {

		uchar uc;
		ifs.read(reinterpret_cast<char*>(&uc), sizeof uc);

		int _f = 0;
		ifs.read(reinterpret_cast<char*>(&_f), sizeof _f);
		f[uc] = _f;

		++i;
	}

#ifdef DEBUG

	cout << "\nData from encoded file:\n";
	for_each(f.begin(), f.end(), [index = uchar{ 0 }](const int value) mutable {
		if (value != 0) {
			cout << "[" << static_cast<char>(index) << "] = " << value << "\n";
		}
		++index;
	});

#endif // DEBUG

	int byte_count = 0;
	uchar modulo = 0;

	ifs.read(reinterpret_cast<char*>(&byte_count), sizeof byte_count);
	ifs.read(reinterpret_cast<char*>(&modulo), sizeof modulo);

	string msg_encoded = "";

	i = 0;
	for (; i < byte_count; ++i) {
		uchar byte;
		ifs.read(reinterpret_cast<char*>(&byte), sizeof byte);

		bitset<CHAR_BIT> b(byte);
		msg_encoded += b.to_string();
	}
	if (modulo > 0) {
		uchar byte;
		ifs.read(reinterpret_cast<char*>(&byte), sizeof byte);

		bitset<CHAR_BIT> b(byte);
		msg_encoded += b.to_string().substr(CHAR_BIT - modulo, CHAR_BIT);
	}

	queue_t q;

	fill_queue(q, f);
	build_tree(q);

	Node::ptr root = q.top();

	string text_decoded;

	decode(root, text_decoded, msg_encoded);

	return text_decoded;

}

// compress a file [filename] and creates a compressed
// file [filename].hff
void compress(const string& filename) {

	ifstream ifs(filename, ifstream::binary);

	CHECK_STREAM(ifs, exit(-1));

	// create a priority queue
	queue_t queue;
	vector<int> f = get_frequencies(ifs);

	fill_queue(queue, f);
	build_tree(queue);

#ifdef DEBUG
	cout << (queue.top())->get_frequency() << " : root\n";
#endif // DEBUG

	vector<string> codes(0x100, "");
	Node::ptr root = queue.top();
	make_code(root, "", codes);

	string message = msg_encoded(filename, codes);

#ifdef DEBUG
	cout << message << " : Encoded message" << endl;
#endif // DEBUG

	write_file_compressed(filename, f, queue, message);
}
