#ifndef MULTIHEADQUEUE_H
#define MULTIHEADQUEUE_H

#include <includes.h>
#include <defines.h>
#include <vector>
#include <thread>

using namespace std;

template <typename T>
class MultiHeadQueue: private std::vector<T>{
    
    private:
        mutex mtx;
    public:
        MultiHeadQueue ():vector<T>(){
        }
        void enqueue(T & t)
        {
            mtx.lock();//start of critical section
            vector<T>::push_back(t);
            mtx.unlock();//end of critical section
        }
        bool dequeue(T & t,std::function <bool (T&,T&)> check)
        {
        //This function needed to be implemented by you

            mtx.lock();//start of critical section
            for (auto x = vector<T>::begin() ;x != vector<T>::end();x++)
            {
                if(check(t,x[0]))
                {
                    t=x[0];//check items passed with items in vector
                    vector<T>::erase(x);//if item was found in vector, then we erase it
                    
                    mtx.unlock();//end of critical section
                    
                    return true;
                }

            }
            mtx.unlock();//end of critical section
            return false;;
        }
        void dump (char * filename,char * p_key_file,char * p_iv_file)
        {
            mtx.lock(); //start of critical section 

            //This function needed to be implemented by you      
         
            CryptoPP::byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], iv[CryptoPP::AES::BLOCKSIZE ];//create variable to store key and iv
            memset( key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH );//initialize key
            memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );//initialize iv
   
            ifstream f;
            f.open(p_key_file,ios::in);//open key file
            if ( f.is_open())//check if file was open successfully
            {
                f.read (reinterpret_cast<char*>(key),sizeof(key));//read key from file
                f.close();
            }else
                cout<<"Unable to open key file.\n";

            f.open(p_iv_file,ios::in);//open iv file
            if ( f.is_open())//check if file was open successfully
            {
                f.read (reinterpret_cast<char*>(iv),sizeof(iv));//read iv from file
                f.close();

            }else
                cout<<"Unable to open iv file.\n";
            
            char* qbuffer = (char *) calloc (vector<T>::size(),sizeof(T));//allocate memory for qbuffer of the same size as the vector
            qbuffer = reinterpret_cast<char*>(vector<T>::data());//store vector data in qbuffer

            string ciphertext;         
            ofstream qw;
            qw.open(filename,ios::out|ios::trunc);//open file containing queue
            if (qw.is_open()){

                //use same encryption method as block, but using the size of the vector instead of blocksize
                //encrypt vector and store it in the queue file

                CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
                CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );
                CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink( ciphertext ) );
                stfEncryptor.Put( reinterpret_cast<const unsigned char*>( qbuffer ), vector<T>::size()*sizeof(T) );
                stfEncryptor.MessageEnd();
                qw.write(ciphertext.c_str(),ciphertext.size());
                qw.close();

            }else
                cout<<"Unable to open queue file.\n";


            mtx.unlock();//end of critical section
        }

        void load (char * filename,char * p_key_file,char * p_iv_file)
        {
            mtx.lock();//start of critical section
            ifstream f;
            f.open(filename,ios::in); //open queue file
            if ( f.is_open())
            {
                f.seekg(0,f.end);
                long sz = f.tellg();//get file size
                f.seekg(0,f.beg);
                T * buffer = (T *) calloc(sz/sizeof(T),sizeof(T));//allocate memory for buffer of size equal to the vector
                f.read (reinterpret_cast<char*>(buffer),sz);//read file into buffer
                f.close();
                // You need to add some neccessary code here

                CryptoPP::byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], iv[CryptoPP::AES::BLOCKSIZE ];//create variables to store key and iv
                memset( key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH );//initalize key
                memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );//initialize iv

                ifstream f;
                f.open(p_key_file,ios::in);//open key file
                if ( f.is_open())
                {
                    f.read (reinterpret_cast<char*>(key),sizeof(key));//read key from file
                    f.close();
                }else
                cout<<"Unable to open key file.\n";

                f.open(p_iv_file,ios::in);
                if ( f.is_open())
                {
                    f.read (reinterpret_cast<char*>(iv),sizeof(iv));
                    f.close();
                } else
                cout<< "Unable to open iv file.\n";

                string decryptedtext;

                //use same decryption method as block, but using the size of the vector instead of blocksize

                CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
                CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );
                CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink( decryptedtext ), CryptoPP::StreamTransformationFilter::ZEROS_PADDING) ;
                stfDecryptor.Put( reinterpret_cast<const unsigned char*>( buffer ), sz );
                stfDecryptor.MessageEnd();
                    
                memcpy (buffer,decryptedtext.c_str(),decryptedtext.size());

                //after decrypting queue file and push it back to the vector

                sz/=sizeof(T);
                for ( int i = 0 ; i < sz ; i++)
                    vector<T>::push_back(buffer[i]);
                free (buffer);
                
            }else{
                cout << "Unable to open file.\n";
            }
            mtx.unlock();//end of critical section
        }
        ~MultiHeadQueue(){}
};



#endif
