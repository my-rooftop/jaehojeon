#include <torch/script.h> //One-stop header.
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

int web_data_size = 234055;

class information_Node* information_table[240000] = {nullptr};

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
        int file_in_node_int[4096];
    public:
        void file_read(FILE* target);
        void Get_hash_in_node(string hash);
        void Get_next_node(class Node* next_node);
        void Get_next_node();
        void Get_refer(unsigned int reference);
        void Return_file_in_node(char* file_char, int size);
        void Return_file_in_node(int* file_int, int size);
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

void Node::Return_file_in_node(int* file_int, int size){
    for (int i = 0; i < 4096; i++){
        file_int[i] = file_in_node_int[i];
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



void Node::file_read(FILE* target){
    int num[1] = {};
    for(int y = 0; y < 4096; y++){
        fread(num, 1, 1, target); 
        file_in_node_int[y] = num[0];
        file_in_node[y] = (char)num[0];
    }
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
	if (argc != 2) {
		cerr << "usage: example-app <path-to-exported-script-module>\n";
		return -1;
	}    

    FILE* web_file = fopen("web", "rb");
	if (web_file == NULL) {
		printf("cannot open\n");
		return 0;
	}

    FILE* information_file = fopen("information", "wb");
    	if (information_file == NULL) {
		printf("cannot open1\n");
		return 0;
	}

    FILE* output_file = fopen("output", "wb");
    	if (output_file == NULL) {
		printf("cannot open2\n");
		return 0;
	}


    torch::jit::script::Module module;
    try {
        module = torch::jit::load(argv[1]);
    }
    catch (const c10::Error& e){
        cerr << "error loading the model\n";
        return -1; 
    }

    auto opts = torch::TensorOptions().dtype(torch::kInt32);

    for (int count = 0; count < web_data_size; count++){
        if(count % 10000 == 0){
            cout << count << endl;
        }
        int* file_int = (int*)malloc(4096*4);
        char hash_256[256];
        class Node* node = new Node; 
        node->file_read(web_file);
        size_t dedup_hash = node->Get_hash_string();



        if (dedup.count(dedup_hash) == 1){
            // cout << "pass" << count << endl;
            class information_Node* inode = new information_Node;
            inode->initialize_information(0, 0, dedup[dedup_hash], 1, 0);
            information_table[count] = inode;
            fwrite(information_table[count], sizeof(*information_table[count]), 1, information_file);
            // inode->show();
            delete inode;
        }
        else if(dedup.count(dedup_hash) == 0){
            dedup[dedup_hash] = count;

            //using torch
            node->Return_file_in_node(file_int, 4096);
            torch::Tensor t = torch::from_blob(file_int, {1,4096}, opts);
            t = (t - 128) / 128.0;
            vector<torch::jit::IValue> inputs;
            inputs.push_back(t);

            // for (int i = 0; i < 10; i++){
            //     cout << t[0][i] << endl;
            // }


            auto t1 = module.forward(inputs).toTensor();
            //bitset<256> h;
            // cout << "<<<<<<>>>>>>" << endl;
            // for (int i = 0; i < 10; i++){
            //     cout << t1[0][i] << endl;
            // }



            for (int i = 0; i < 256; i++){
                if(t1[0][i].item<float>() <= 0){
                    hash_256[i] = '0'; //h.set(i);
                }
                else{
                    hash_256[i] = '1';
                }
            }
            inputs.clear();
            string hash_string;
            for (int i = 0; i < 256; i++){
                hash_string += hash_256[i];
            }
            // cout << hash_string << endl;
            string split = hash_string.substr(236, 15);
            unsigned int hash_index = (unsigned int)bitset<15>(split).to_ulong();



            if(hash_table[hash_index] == NULL){
                hash_table[hash_index] = node;
                node->Get_hash_in_node(hash_string);
                node->Get_next_node();
                node->Get_refer(count);

                char* output = (char*)malloc(LZ4_compressBound(4096));
                char* file_char = (char*)malloc(4096);
                node->Return_file_in_node(file_char, 4096); 
                int compressedSize = LZ4_compress_default(file_char, output, 4096, LZ4_compressBound(4096));
                //cout << "Compressed_Size : " << compressedSize << endl;
                unsigned int locate = ftell(output_file);
                fwrite(output, 1, compressedSize, output_file);

                class information_Node* inode = new information_Node;
                inode->initialize_information((unsigned int)compressedSize, locate, 0, 0, 1);

                information_table[count] = inode;
                fwrite(information_table[count], sizeof(*information_table[count]), 1, information_file);
                // inode->show();
                delete inode;
                free(output);
                free(file_char);
            
            }
            else if(hash_table[hash_index] != NULL){
                class Node* start_point;
                start_point = hash_table[hash_index];
                int trigger = 0;
                while(trigger == 0){
                    string search_hash = start_point->Return_hash_in_node();
                    if(search_hash == hash_string){
                        trigger = 1;

                        char* XOR = (char*)malloc(4096);
                        char* reference = (char*)malloc(4096);
                        char* Self = (char*)malloc(4096);
                        start_point->Return_file_in_node(reference, 4096); 
                        node->Return_file_in_node(Self, 4096);

                        for(int j = 0; j < 4096; j++){
                            XOR[j] = reference[j] ^ Self[j]; 
                            // if(j==1026){
                            //     cout << "here" << "XOR" << bitset<8>(XOR[j]) << "refer" << bitset<8>(reference[j]) << "self" << bitset<8>(Self[j]) << endl;
                            // }
                        }

                        char* output = (char*)malloc(LZ4_compressBound(4096));

                        int compressedSize = LZ4_compress_default(XOR, output, 4096, LZ4_compressBound(4096));

                        unsigned int locate = ftell(output_file);

                        fwrite(output, 1, compressedSize, output_file);
                        class information_Node* inode = new information_Node;
                        // unsigned int compressed_length_, 
                        // unsigned int saved_location_, unsigned int reference_, 
                        // unsigned int deduped_, unsigned int self_compressed_
                        unsigned int referr = start_point->Return_refer_in_node();
                        inode->initialize_information((unsigned int)compressedSize, locate, referr, 0, 0);

                        information_table[count] = inode;

                        fwrite(information_table[count], sizeof(*information_table[count]), 1, information_file);
                        // inode->show();
                        delete inode;
                        free(XOR);
                        free(reference);
                        free(Self);

                    }
                    else if(search_hash != hash_string){
                        //cout << "@" << endl;
                        class Node* Next;
                        Next = start_point->Return_next();
                        if(Next == NULL){
                            node->Get_hash_in_node(hash_string);
                            node->Get_next_node();
                            node->Get_refer(count);
                            start_point->Get_next_node(node);
                            trigger = 1;

                            char* output = (char*)malloc(LZ4_compressBound(4096));
                            char* file_char = (char*)malloc(4096);
                            node->Return_file_in_node(file_char, 4096); 
                            
                            int compressedSize = LZ4_compress_default(file_char, output, 4096, LZ4_compressBound(4096));

                            unsigned int locate = ftell(output_file);
                            fwrite(output, 1, compressedSize, output_file);

                            class information_Node* inode = new information_Node;
                            inode->initialize_information((unsigned int)compressedSize, locate, 0, 0, 1);

                            information_table[count] = inode;
                            fwrite(information_table[count], sizeof(*information_table[count]), 1, information_file);
                            // inode->show();
                            delete inode;
                            free(output);
                            free(file_char);
                        }
                        else if(Next != NULL){
                
                            start_point = Next;
                      
                        }
                        else{
                            cerr << "Something wrong!!!!\n";
                            return -1; 
                        }
                    }
                    else{
                        cerr << "Something wrong!!!\n";
                        return -1;    
                    }
                }
            }
        }
        else{
            cerr << "Something wrong!!\n";
            return -1; 
        }
        free(file_int);
    }

    fclose(web_file);
    fclose(information_file);
    fclose(output_file);

    return 0;

}