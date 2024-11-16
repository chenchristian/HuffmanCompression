#include <string>
#include <queue>
#include <fstream>
#include <map>
#include "Node.h"
#include "Storage/Storage.h"

using namespace std;

#ifndef HUFFMAN_H
#define HUFFMAN_H

#endif //HUFFMAN_H


class Huffman{

private:

    //Compress Helper Functions

    /**
     * @brief Reads the file and puts file context into a string
     * @param in_file where to read
     * @param text what to write to (passed by reference)
     * @return returns true if successful, and false if file not found
     */
    bool readFile(string in_file, string& text);

    /**
     * @brief Makes a map with char as the key and int for the values
     * @param text a string of text we want to count each character of
     * @return returns the map that we created
     */
    unordered_map<char, int> makeCharacterCountMap(string text);

    /**
     * @brief makes a binary tree whose leaves are the nodes that hold the
     * characters from the text. The most common characters are higher up and the
     * most unique characters are at the bottom
     * @param minPriorityQueue Uses this queue of nodes to construct the tree
     * @param characterCountMap uses the pairings in this map to make nodes which will
     * be pushed into the queue
     * @return
     */
    Node* makeTree(priority_queue<Node*, vector<Node*>, compareWeights> &minPriorityQueue,
                   unordered_map<char, int> characterCountMap);

    /**
     *@brief creates a map with the unique characters as the key, and the binary equivalent
     * as the value in the form of a string
     * @param rootNode pointer to the root node
     * @param code builds out the binary string, 0 for left, 1 for right
     * @param binaryCodeMap passing the map by reference
     */
    void makeBinaryMap(Node* rootNode, string code, unordered_map<char, string> &binaryCodeMap);

    /**
     * @brief function will write out the compressed file
     * @param binaryCodeMap the map with the keys characters and binary value
     * @param header keeping the header in the file so we can decompress
     * @param writeFile where we want to write this file to
     * @param readFileContents the contents of the file we are reading from
     * @return
     */
    void writeWithStorage(unordered_map<char, string> &binaryCodeMap, string header,
                         string writeFile, string readFileContents);


    //Decompress helper functions Below

    /**
     * @brief We are recreating the tree using the header string. So we can decode the binary
     * @param header this is a string of our character and binary code legend
     * @return returns the root of the tree
     */
    Node* makeTreeFromHeader(string header);

    /**
     * @brief a recursive helper function to create the tree
     * @param characterAndBinary the string used to direct us on how to create the tree
     * which follows this format, a char followed by binary, ie g00 or h0101
     */
    void makeTreeFromHeaderHelper(string characterAndBinary, Node* tree);

    /**
     * @brief this function will read in the string stream and output each character
     * using the tree as a translator
     * @param tree binary tree that you can traverse to convert binary (011010) into
     * a character
     * @param FileinBinary a string stream of 1's and 0's that represent the binary in the
     * compressed file
     * @return returns the translated character
     */
    char translateFromTree(Node* tree, stringstream* FileinBinary);


public:

    /**
     * Default Constructor
     */
    Huffman();

    void compress(string in_file, string out_file);
    void decompress(string in_file, string out_file);

};

