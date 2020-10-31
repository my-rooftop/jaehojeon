
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <typeinfo>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <string>
#include <bitset>
#include <map>
#include <stdlib.h>
#include <sys/time.h>
extern "C" {
	#include "lz4.h"
}

using namespace std;

int web_data_size = 230455;

class information_Node* information_table[230455] = {nullptr};

class information_Node
{	
    private:
        unsigned int compressed_length;
        unsigned int saved_location;
        unsigned int reference;
        unsigned int deduped;
        unsigned int self_compressed;
    public:
        void initialize_information(unsigned int compressed_length_, unsigned int saved_location_, unsigned int reference_, unsigned int deduped_, unsigned int self_compressed_);
        void show();
};

void information_Node::show()
{

    cout << "inode->deduped : "<< deduped << endl;
    cout << "inode->compressed_length : "<< compressed_length<< endl;
    cout << "inode->saved_location : "<< saved_location << endl;
    cout << "inode->reference : "<< reference << endl;
    cout << "inode->self_compressed : "<< self_compressed << endl;
}

void information_Node::initialize_information(unsigned int compressed_length_, unsigned int saved_location_, unsigned int reference_, unsigned int deduped_, unsigned int self_compressed_){
    compressed_length = compressed_length_;
    saved_location = saved_location_;
    reference = reference_;
    deduped = deduped_;
    self_compressed = self_compressed_;
}

class Node* hash_table[140000] = {};

class Node
{
    private:
        string hash_in_node;
        class Node* next = NULL;
        unsigned int refer;
        char file_in_node[4096];
    public:
        void file_read(FILE* target);
        void file_in_node_int(int* file_int, int size);
        void Get_hash_in_node(string hash);
        void Get_next_node(class Node* next_node);
        void Get_next_node();
        void Get_refer(unsigned int reference);
        void Return_file_in_node(char* file_char, int size);
        string Return_hash_in_node();
        class Node* Return_next();
        unsigned int Return_refer_in_node();
        size_t Get_hash_string();
};

unsigned int Node::Return_refer_in_node(){
    return refer;
}

class Node* Node::Return_next(){
    return next;
}

string Node::Return_hash_in_node(){
    return hash_in_node;
}

void Node::Return_file_in_node(char* file_char, int size){
    for (int i = 0; i < 4096; i++){
        file_char[i] = file_in_node[i];
    }

}

void Node::Get_refer(unsigned int reference){
    refer = reference;
}



void Node::Get_next_node(class Node* next_node){
    next = next_node;
}

void Node::Get_next_node(){
    next = NULL;
}

void Node::Get_hash_in_node(string hash){
    hash_in_node = hash;
}

void Node::file_in_node_int(int* file_int, int size)
{
    char file_char[4096];
    for(int i = 0; i < 4096; i++){
        file_char[i] = file_in_node[i];
    }
    for(int i = 0; i < 4096; i++){
        file_int[i] = (int)file_char[i];
    }

}

void Node::file_read(FILE* target){
    fread(file_in_node, 1, 4096, target); 
}

size_t Node::Get_hash_string()
{
    string str;
    for(int i = 0; i<4096; i++){
        str += file_in_node[i];
    }
    size_t d = hash<string>{}(str);

    return d;
}


map<size_t, int> dedup;

int main(int argc, const char* argv[]){

    FILE* web_file = fopen("web", "rb");
	if (web_file == NULL) {
		printf("cannot open\n");
		return 0;
	}

    FILE* information_file = fopen("information1", "wb");
    	if (information_file == NULL) {
		printf("cannot open1\n");
		return 0;
	}

    FILE* output_file = fopen("output1", "wb");
    	if (output_file == NULL) {
		printf("cannot open2\n");
		return 0;
	}


    for (int count = 0; count < web_data_size; count++){

        class Node* node = new Node; 
        node->file_read(web_file);
        size_t dedup_hash = node->Get_hash_string();



        if (dedup.count(dedup_hash) == 1){

        }
        else if(dedup.count(dedup_hash) == 0){
            dedup[dedup_hash] = count;
            char* output = (char*)malloc(LZ4_compressBound(4096));
            char* file_char = (char*)malloc(4096);
            node->Return_file_in_node(file_char, 4096); 
            int compressedSize = LZ4_compress_default(file_char, output, 4096, LZ4_compressBound(4096));
            unsigned int locate = ftell(output_file);
            fwrite(output, 1, compressedSize, output_file);

            // inode->show();
            free(output);
            free(file_char);
        }
        else{
            cerr << "Something wrong!!\n";
            return -1; 
        }

    }

    fclose(web_file);
    fclose(information_file);
    fclose(output_file);

    return 0;

}