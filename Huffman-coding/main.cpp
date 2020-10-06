#include "huffman.h"

using namespace std;

int main(void) {

	const string filename = "text.txt";

	compress(filename);

	string text = read_encoded_file(filename + ".hff");

	cout << text << endl;

	return 0;
}