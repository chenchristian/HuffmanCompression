# HuffmanCompression
A text file compression program using Huffman coding. The goal is to compress text files efficiently, potentially reducing their size by nearly half.


Moby Dick compressed from 1.3mb to 708k. This is around a 65% of its original size. My program is implemented in two phases, a compress and a decompress.

The compress function first reads in the contents of the file and create a count of each character. The count of the characters are stored in a map with the key being the character and the value being the frequency of this character. Then we take the map and store it into a priority queue which will tell us how to create our tree. The tree is created by adding Nodes, which carry the characters and its frequency, with the lowest frequency first. Once our map is created, we can traverse the tree to generate the new binary code for each character. Next we will create a string called header which will contain each character followed by its binary paring (example: g0110). We will then store the header into a Storage object. Using the map we will translate each character into its binary counterpart and insert that into a new file using Storage's insert function.

To decompress a compressed file we will create a Storage object and grab the first line of the compressed file, this will be the header. We will use the header to generate a new tree reverseing the method we used to create it in our compress function. Once we have our reconstructed tree, we will call extract from Storage to read in the binary from the compressed file and use the reconstructed tree to get our corresponding character. We take these characters and recreate our file.
