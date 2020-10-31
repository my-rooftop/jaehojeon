#include <stdio.h>
#include <vector>
#include <iostream>
#include <memory>
#include <bitset>
#include <map>
extern "C" {
	#include "lz4.h"
}
using namespace std;

class information_Node* information_table[180000];

class information_Node
{	
public:
    unsigned int compressed_length;
    unsigned int saved_location;
    unsigned int reference;
    unsigned int deduped;
    unsigned int self_compressed;
};

char refered[180005][4096];


int main()
{
    int Max_compress = LZ4_compressBound(4096);
    FILE* finfo = fopen("information", "rb");
    FILE* finput = fopen("output", "rb");
    FILE* ftest = fopen("web", "rb");
    
    for(int i = 0; i < 180000; i++){
        class information_Node* inode = new information_Node;
        fread(inode, 20, 1, finfo);
        information_table[i] = inode;
        // if(i > 1){
        //     cout << "count [" << i << "]" << endl; 
        //     cout << "inode->deduped : "<< inode->deduped << endl;
        //     cout << "inode->compressed_length : "<< inode->compressed_length<< endl;
        //     cout << "inode->saved_location : "<< inode->saved_location << endl;
        //     cout << "inode->reference : "<< inode->reference << endl;
        //     cout << "inode->self_compressed : "<< inode->self_compressed << endl;
        // }

        if(inode->deduped == 1){
            FILE* fdecomp2 = fopen("decompressed", "wb");
            fseek(fdecomp2, i*4096 , SEEK_SET);
            fwrite(refered[inode->reference], 1, 4096, fdecomp2);
            for (int j=0; j<4096; j++){
                refered[i][j] = refered[inode->reference][j];
            }
            fclose(fdecomp2);
            char* read = (char*)malloc(4096);
            fread(read, 1, 4096, ftest);
            free(read);
        }
        else if (inode-> self_compressed == 1){

            int Compressed_size = information_table[i]->compressed_length;
            char* compressed = (char*)malloc(Max_compress);
            char* decompressed = (char*)malloc(4096);
            // cout << Compressed_size << endl;
            fread(compressed, 1, Compressed_size, finput);
            int Decompressed_size = LZ4_decompress_safe(compressed, decompressed, Compressed_size, 4096);
            // cout << Decompressed_size << endl;

            char* read = (char*)malloc(4096);
            fread(read, 1, 4096, ftest);
            for(int j = 0; j < 4096; j++){
                if(read[j] != decompressed[j]){
                    cout << "1sibal" <<  i << endl;
                    break;
                }
            }
            for (int j=0; j<4096; j++){
                refered[i][j] = decompressed[j];
            }
            

            
            FILE* fdecomp = fopen("decompressed", "wb");
            fseek(fdecomp, i*4096 , SEEK_SET);
            fwrite(decompressed, 1, 4096, fdecomp);
            fclose(fdecomp);
            
            free(read);
            free(compressed);
            free(decompressed);
        }
        else if(inode->self_compressed == 0){
            
            int Compressed_size = information_table[i]->compressed_length;
            char* compressed = (char*)malloc(Max_compress);
            fread(compressed, 1, Compressed_size, finput);
            char* decompressed = (char*)malloc(4096);


            int Decompressed_size = LZ4_decompress_safe(compressed, decompressed, Compressed_size, 4096);





            
            char* XOR = (char*)malloc(4096);

            for(int j = 0; j < 4096; j++){
                XOR[j] = refered[inode->reference][j] ^ decompressed[j];
                // if(j==1026){
                //     cout << "here" << "XOR" << bitset<8>(XOR[j]) << "refer" << bitset<8>(refered[j]) << "decomp" << bitset<8>(decompressed[j]) << endl;
                // }
            }


            for (int j=0; j<4096; j++){
                refered[i][j] = XOR[j];
            }

            FILE* fdecomp = fopen("decompressed", "wb");
            fseek(fdecomp, i*4096 , SEEK_SET);
            fwrite(XOR, 1, 4096, fdecomp);
            fclose(fdecomp);

            char* read = (char*)malloc(4096);
            fread(read, 1, 4096, ftest);
            for(int j = 0; j < 4096; j++){
                if(read[j] != XOR[j]){
                    cout << "sil" << j <<endl;
                    break;
                }

            }      

            free(read);
            free(compressed);
            free(decompressed);
            free(XOR);



        }
        
        

    }


    
    // int Compressed_size1 = information_table[1]->compressed_length;
    // char* compressed1 = (char*)malloc(Max_compress);
    // char* decompressed1 = (char*)malloc(4096);
    // cout << Compressed_size1 << endl;
    // fread(compressed1, 1, Compressed_size1, finput);
    // int Decompressed_size2 = LZ4_decompress_safe(compressed, decompressed, Compressed_size1, 4096);
    // cout << Decompressed_size2 << endl;
    // free(compressed1);
    // free(decompressed1);
    fclose(finput);




    // char* read1 = (char*)malloc(4096);
    // fread(read1, 1, 4096, ftest);
    // for(int i = 0; i < 4096; i++){
    //     if(read1[i] != compressed1[i]){
    //         cout << "2sibal"<< i <<endl;
    //     }
    // }
    // fclose(ftest);

    // FILE* finput = fopen("output", "rb");
    // for(int i = 0; i < 2; i++){
    //     char* read = (char*)malloc(4096);
    //     fread(read, 1, 4096, ftest); 

    //     //fread(read, 1, 4096, ftest);

    //     if(information_table[i]->deduped == 1){
    //         continue;
    //     }
    //     else if(information_table[i]->deduped== 0){
    //         if(information_table[i]->self_compressed == 1){   
    //             FILE* foutput = fopen("web_decomp", "wb");        
    //             fseek(foutput, i*4096, SEEK_SET); 
    //             int Compressed_size = information_table[i]->compressed_length;
    //             char* compressed = (char*)malloc(Max_compress);
    //             char* decompressed = (char*)malloc(4096);
    //             int locate = information_table[i]->saved_location;
    //             fseek(finput, locate, SEEK_SET);
    //             fread(compressed, 1, Compressed_size, finput);
    //             //Segment
    //             int Decompressed_size = LZ4_decompress_safe(compressed, decompressed, Compressed_size, 4096);
    //             cout << "Decompressed_size is" << Decompressed_size << endl;
    //             fwrite(decompressed, 1, 4096, foutput);

    //             cout << "start" << " " << i << endl;
    //             for(int t = 0; t < 20; t++){
    //                 if(read[t] != decompressed[t])
    //                 {
    //                     cout << t << "worry!" << endl;
    //                 }
    //             }

    //             free(compressed);
    //             free(decompressed);
    //             fclose(foutput);

    //         }
    //         else if(information_table[i]->self_compressed == 0){       
    //             cout << i<<"pass" << endl;
    //             int Compressed_size = information_table[i]->compressed_length;
    //             char* compressed = (char*)malloc(Max_compress);
    //             char* decompressed = (char*)malloc(4096);
    //             int locate = information_table[i]->saved_location;
    //             fseek(finput, locate, SEEK_SET);
    //             fread(compressed, 1, Compressed_size, finput);
    //             //Segment
    //             int Decompressed_size = LZ4_decompress_safe(compressed, decompressed, Compressed_size, 4096);
    //             cout << "Decompressed_size is" << Decompressed_size << endl;
    //             char* reference_data = (char*)malloc(4096);
    //             FILE* foutput = fopen("web_decomp", "rb"); 
    //             fseek(foutput, information_table[i]->reference*4096, SEEK_SET);
    //             fread(reference_data, 1, 4096, foutput);
    //             fclose(foutput);

    //             cout <<"reference is :" << information_table[i]->reference << endl;
    //             char* decompress = (char*)malloc(4096);
    //             for(int t = 0; t < 4096; t++){
                    
    //                 decompress[t] = ((reference_data[t]) ^  (decompressed[t]));
                
    //             }


    //             cout << "start" << " " << i << endl;
    //             int count=0;
    //             for(int t = 0; t < 4096; t++){
    //                 if(read[t] != decompress[t])
    //                 {
    //                     count = count + 1;
    //                 }
    //             }
    //             cout << "count is: " << count << endl;
    //             FILE* foutput1 = fopen("web_decomp", "wb");  
    //             fseek(foutput1, i*4096, SEEK_SET); 
    //             fwrite(decompress, 1, 4096, foutput1);
    //             fclose(foutput1);



    //             free(reference_data);
    //             free(compressed);
    //             free(decompressed);
    //             free(decompress);
    //         }
    //         else{
    //             continue;
    //         }
    //     }
    //     else{
    //         continue;
    //     }
    //     free(read);

    // }
    // fclose(ftest);
    // fclose(finfo);
    // fclose(finput);
    return 0;
}