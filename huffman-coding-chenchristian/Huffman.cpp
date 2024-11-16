#include "Huffman.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string.h>


using namespace std;

Huffman::Huffman() {

}

bool Huffman::readFile(string in_file, string& text) {

    //file not there
    ifstream file(in_file);
    if (!file.is_open()) {
        return false;
    }

    /**
     * getting each line from the file
     * and adding it into the stringstream
     */
    stringstream fileContent;
    string line;
    while (getline(file, line)) {
        fileContent << line << '\n';
    }

    /**
     * Adding a ascii character 3 to represent the end of the file.
     * This will be useful later to tell us when to stop decompressing
     */
    char endOfFile = 3;
    fileContent << endOfFile;

    text = fileContent.str();

    return true;
}



unordered_map<char, int> Huffman::makeCharacterCountMap(string text) {

    //Creating a map, left side characters, right side int.
    unordered_map<char, int> characterCountMap;

    for(int i = 0; i < text.length(); i++)
    {
        char nextCharacter = text[i];

        /**
         * Seeing if the character is in the map.
         *
         * in the standard library, find function will either return
         * the position of the found item, or return the end (new spot)
         * if an item is not found.
         * @see https://www.geeksforgeeks.org/map-find-function-in-c-stl/?ref=lbp
         */

        //if new character
        if(characterCountMap.find(nextCharacter) == characterCountMap.end())
        {
            //if character is found for the first time, set value to 1
            int firstTime = 1;
            characterCountMap.insert(pair<char, int>(nextCharacter, firstTime));
        }
        else
        {
            //this will increase the value (the count) of the character in the map by 1
            characterCountMap[nextCharacter]++;
        }
    }
    return characterCountMap;
}



Node* Huffman::makeTree(priority_queue<Node*, vector<Node *>, compareWeights> &minPriorityQueue,
                        unordered_map<char, int> characterCountMap) {
    /**
         * Creating a priority queue
         * priority queue declaration structure:
         * Type, how it is stored, min or max
         * @see https://en.cppreference.com/w/cpp/container/priority_queue
         *
         * The compareWeights function, takes in Node* so we need to make our
         * priority_queue out of Node*
         */
    //For each pair in the map, Character and its count
    for (auto pair : characterCountMap) {


        /**
         * Node constructor (character, weight) so we are setting the count of the character by the weight
         * and the minPriorityQueue looks at the weights
         */
        Node* insertNode = new Node(pair.first, pair.second);
        minPriorityQueue.push(insertNode);
    }

    /**
     * Now that we have the queue, we can make our tree by adding the smallest
     * weighted Nodes together into a new node, with the weight of the new node
     * as the combination of the children Node's weights. Set the new nodes children
     * as those two smallest nodes. Remove those 2 smallest nodes, and repeat until one
     * item in the queue.
     */
    Node* combinedNode = nullptr;
    Node* zeroSmallest = nullptr; //left child node
    Node* oneSmallest = nullptr; //right child node
    while(minPriorityQueue.size() != 1)
    {
        zeroSmallest = minPriorityQueue.top();
        minPriorityQueue.pop();

        oneSmallest = minPriorityQueue.top();
        minPriorityQueue.pop();

        int combinedWeight = zeroSmallest->weight + oneSmallest->weight;
        char nullChar = '\0';
        combinedNode = new Node(nullChar ,combinedWeight, zeroSmallest, oneSmallest);

        minPriorityQueue.push(combinedNode);
    }

    Node* rootNode = minPriorityQueue.top();
    return rootNode;
}

void Huffman::makeBinaryMap(Node* tree, string code, unordered_map<char, string> &binaryCodeMap) {
    //Base Case
    if(tree->letter != '\0')
    {
        //Inserts into a map, the character and the binary code that goes with it.
        binaryCodeMap.insert(pair<char, string>(tree->letter,code));
        return;
    }
    makeBinaryMap(tree->zero, code + "0", binaryCodeMap);
    makeBinaryMap(tree->one, code + "1", binaryCodeMap);
}

void Huffman::writeWithStorage(unordered_map<char, string> &binaryCodeMap, string header, string writeFile,
                         string readFileContent) {
    Storage *storage = new Storage();

    if (!storage->open(writeFile, "write")) {
        cout << "There was an error opening the file." << endl;
        exit(0);
    }

    //setting the header for the storage
    storage->setHeader(header);

    //now i need to write my file character by character.
    string binary;
    for(int i = 0; i < readFileContent.length(); i++)
    {
        char theChar = readFileContent[i];
        //gets the value of the paired to the key
        binary = binaryCodeMap.find(theChar)->second;
        storage->insert(binary);
    }
    storage->close();
}

Node* Huffman::makeTreeFromHeader(string header) {
    char letter = '\0';
    int weight = 0;
    //Creating the root of the new tree
    Node* root = new Node(letter, weight);

    int headerLength = header.length();

    /**
     *  Getting each segment of the header that is comprised of
     *  char followed by the binary. ex: g00 or p0101
     *
     *  then putting each segment into the map
     */
    string characterAndBinary;
    for(int i=0; i < headerLength; i++)
    {
        //reset the string
        characterAndBinary = "";
        while(header[i] != '\30')
        {
            characterAndBinary += header[i];
            //go next character
            i++;
        }

        /**
         * now we need a function to insert things into the tree
         */
        makeTreeFromHeaderHelper(characterAndBinary, root);
    }

    return root;
}

void Huffman::makeTreeFromHeaderHelper(string characterAndBinary, Node* tree) {

    //BASE CASE 1
    if(characterAndBinary.substr(1) == "1")
    {
        char theCharacter = characterAndBinary[0];
        int theWeight = 0; //we dont need anything for thr weight, but we need to initialize it
        Node* insertNode = new Node(theCharacter, theWeight);
        tree->one = insertNode;
        return;
    }
    //BASE CASE 2
    if(characterAndBinary.substr(1) == "0")
    {
        char theCharacter = characterAndBinary[0];
        int theWeight = 0; //we dont need anything for thr weight, but we need to initialize it
        Node* insertNode = new Node(theCharacter, theWeight);
        tree->zero = insertNode;
        return;
    }

    //RECURSIVE CASE
    else
    {

        if(characterAndBinary[1] == '1')
        {
            string cut = characterAndBinary.substr(0,1) + characterAndBinary.substr(2);
            //creating a non leaf node for the tree
            if(tree->one == nullptr)
            {
                Node* temp = new Node('\0', 0);
                tree->one = temp;
               return makeTreeFromHeaderHelper(cut, temp);
            }
            return makeTreeFromHeaderHelper(cut, tree->one);
        }
        else if(characterAndBinary[1] == '0')
        {
            string cut = characterAndBinary.substr(0,1) + characterAndBinary.substr(2);
            //creating a non leaf node for the tree
            if(tree->zero == nullptr)
            {
                Node* temp = new Node('\0', 0);
                tree->zero = temp;
                return makeTreeFromHeaderHelper(cut, temp);
            }
            return makeTreeFromHeaderHelper(cut, tree->zero);
        }
    }
}

char Huffman::translateFromTree(Node *tree, stringstream* fileInBinary) {
    char firstChar = fileInBinary->peek();
    if(tree->letter != '\0')
    {
        return tree->letter;
    }
    if(firstChar == '1')
    {
        //cutting the first character away
        fileInBinary->ignore(1);
        return translateFromTree(tree->one, fileInBinary);
    }
    if(firstChar == '0')
    {
        fileInBinary->ignore(1);
        return translateFromTree(tree->zero, fileInBinary);
    }
}

void Huffman::compress(string in_file, string out_file) {

    //Text will be the contents of the file in a string
    string text;
    if(!readFile(in_file, text))
    {
        cout << "Unable to open file\n";
    }
    else
    {
        cout << "Read Successful\n";

        /**
         * This will make a map<char, int>
         * Counts how many characters are in the text
         */
        unordered_map<char, int> characterCountMap = makeCharacterCountMap(text);

        //Creating a minimum priority queue
        priority_queue<Node*, vector<Node*>, compareWeights> minPriorityQueue;


        /**
         * Passing the map that has all the character and their counts as a pair
         * with the empty queue into the makeTree function
         *
         * binaryCodeMap will be a map with the unique characters as a key
         * and the binary equivalent as the value
         */
        Node* treeRoot = makeTree(minPriorityQueue, characterCountMap);

        unordered_map<char, string> binaryCodeMap;
        string binaryCode = "";
        makeBinaryMap(treeRoot, binaryCode, binaryCodeMap);


        /**
         * Creating the header for our new storage object.
         * It will be the character, then its binary equivalent in the form of a string,
         * followed by a delimiter.
         */

        string header;
        for(const auto& pair : binaryCodeMap)
        {
            //Character,Binary Code, Delimiter
            header +=  pair.first + pair.second + "\30";
        }


        /**
         * This will create the new compressed file.
         */
        writeWithStorage(binaryCodeMap, header, out_file, text);

    }
}

void Huffman::decompress(string in_file, string out_file) {

    Storage *storage = new Storage();

    if (!storage->open(in_file, "read")) {
        cout << "There was an error opening the file." << endl;
        exit(0);
    }

    string header;
    //setting the first line of the file, as the header because that's where we store it
    header = storage->getHeader();

    //verifying that the header was stored correctly and read correctly


    //Lets remake our tree using the info from the header
    Node* remadeTree = makeTreeFromHeader(header);


    stringstream fileInBinaryStream;
    string binary; //the segments of binary to use in our extract function

    //creating the outfile
    ofstream decompressedFile(out_file);

    while(storage->extract(binary))
    {
        fileInBinaryStream << binary;
    }


    stringstream* fileInBinary_ptr = &fileInBinaryStream;
    //while not end of the string stream
    while(fileInBinary_ptr->peek() != EOF)
    {
        char c = translateFromTree(remadeTree, fileInBinary_ptr);
        if(c == 3)
        {
            break;
        }
        decompressedFile << c;
    }

    decompressedFile.close();

}










