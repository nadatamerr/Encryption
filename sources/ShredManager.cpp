#include <ShredManager.h>
#include <ThreadManager.h>
#include <MultiHeadQueue.h>
ShredManager::ShredManager () // added this in phase 2
{

}

ShredManager::ShredManager(char * p_file_name, uint16_t p_block_size, uint16_t p_shred_count,bool truncate)
{
    shred_count = p_shred_count;
    shreds = (Shred **) calloc (shred_count, sizeof(Shred *)); //allocate memory for 2d array of shreds
    for ( char i = 0 ; i  < shred_count; i++)
    {
        string fname = p_file_name;//copy path of shreds into fname
        fname.insert(fname.find('.'),1,i+'A');//the letters A,B,C... that would be added to the end of the shred name
        cout << fname << endl;
        if (truncate)
            shreds[i] = new Shred(fname.c_str(),p_block_size,truncate);// if true, it will initialize a shred
        else shreds[i] = new Shred(fname.c_str(),(p_block_size+16)&~15,truncate);// if false it will increase the block size and make a new shred
    }
}
bool ShredManager::encrypt (FileSpooler * fileSpooler, const char * key_file_name, const char * iv_file_name)
{
    AutoSeededRandomPool prng; //instantiate object from AutoSeededRandomPool class
    CryptoPP::byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], iv[ CryptoPP::AES::BLOCKSIZE ];// create variables to store the key and the initialization vector 
    memset( key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH );//initialize key
    memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );//initialize iv

    ifstream kf;
    kf.open(key_file_name,ios::in);//open file containing key
    if ( kf.is_open())//make sure opening file was successful
    {
        kf.read (reinterpret_cast<char*>(key),sizeof(key));//read key from file as an array of characters and store it in key
        kf.close(); //close file
    }

    prng.GenerateBlock(iv,sizeof(iv));//generate random iv
    Block * block = fileSpooler->getNextBlock();//instantiate an object of class block and store a block in it
    for (int i = 0 ;block != NULL; i ++)//loop until getNextBlock() doesnt return a block
    {
        block->encrypt(key,iv);//encrypt block
        *(shreds[i%shred_count]) << *block;//store encrypted block in the corresponding shred 
        delete (block);
        block = fileSpooler->getNextBlock();//store next block to be encrypted in block
    }
    ofstream f;
    f.open(iv_file_name,ios::out|ios::trunc);//create and open a file to write iv in
    if ( f.is_open())//make sure file was open successfully
    {
        f.write (reinterpret_cast<const char*>(iv),sizeof(iv)); //store iv in file
        f.close();
    }
    return true;
}
bool ShredManager::decrypt (FileSpooler * fileSpooler, const char * key_file_name, const char * iv_file_name)
{
    AutoSeededRandomPool prng; //instantiate object from AutoSeededRandomPool class
    CryptoPP::byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], iv[ CryptoPP::AES::BLOCKSIZE ];// create variables to store the key and the initialization vector 
    memset( key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH );//initialize key
    memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );//initialize iv

    ifstream f;
    f.open(key_file_name,ios::in);//open file that contains key
    if ( f.is_open())//make sure file was open successfully
    {
        f.read (reinterpret_cast<char*>(key),sizeof(key));//read key from file as an array of characters and store it in key
        f.close();
    }

    f.open(iv_file_name,ios::in); //open file containing iv
    if ( f.is_open())//make sure file was open successfully
    {
        f.read (reinterpret_cast<char*>(iv),sizeof(iv));//read iv from file as an array of characters and store it in iv
        f.close();
    }

    Block * block = NULL;
    for (int i = 0 ; i == 0 || block != NULL; i ++)
    {
        block = shreds[i%shred_count]->getNextBlock();//instantiate an object of class block and store a block in it
        if ( block == NULL) break;
        block->decrypt(key,iv);//decrypt block
        //block->print();
        fileSpooler->appendBlock(block);//write decrypted block in output file
        delete (block);
    }
    return false;
}
ShredManager::~ShredManager()
{
    for ( int i = 0 ; i  < shred_count; i++)
        delete(shreds[i]);
    free(shreds);
}
//*************

MultithreadedShredManager::MultithreadedShredManager(char * p_file_name, uint16_t p_block_size, uint16_t p_shred_count,bool p_truncate) : ShredManager()
{
    //initialize the class members with the parameters given
    file_name = p_file_name;
    shred_count = p_shred_count;
    block_size = p_block_size;
    truncate = p_truncate;
    shreds = (Shred ** ) calloc (shred_count,sizeof(Shred*));
}
bool MultithreadedShredManager::encrypt (FileSpooler * p_fileSpooler, char * key_file_name,  char * iv_file_name, char * q_file_name)
{
    //This function needed to be implemented by you

    ThreadManager tm;//instantiate an object from class threadManager
        
    long num_block = p_fileSpooler->getBlockCount(); //declare a variable to story number of blocks
    //get number of blocks using the function getBlockCount from class fileSpooler
    Lottery l(num_block);//instantiate onject of the lottery using number of blocks
    
    MultiHeadQueue <sb_block_index> que;//instantiate object of class MultiHeadQueue passing struct sb_block_index as the type for the template

    AutoSeededRandomPool prng; //instantiate object from AutoSeededRandomPool class
    CryptoPP::byte iv[ CryptoPP::AES::BLOCKSIZE ];// create variable to store the initialization vector 
    memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );//initialize iv
    prng.GenerateBlock(iv,sizeof(iv));//generate random iv

    for ( char i = 0 ; i  < shred_count; i++)//loop on all shreds
    {

        string fname = file_name;//store path of output file in fname
        fname.insert(fname.find('.'),1,i+'A');//create different shred names using letters A,B... for each shred

        if(truncate)
            shreds[i] = new EncryptShredThread(p_fileSpooler,key_file_name,iv_file_name,fname.c_str(), block_size,&l,&que,i+'A',truncate);
        //for each shred instantiate object of class EncryptShredThread and store it in array of shreds
        else
            shreds[i] = new EncryptShredThread(p_fileSpooler,key_file_name,iv_file_name,fname.c_str(),(block_size+16)&~15,&l,&que,i+'A',truncate);
        //do the same, while padding incomplete blocks
    
        tm += dynamic_cast<Thread*>(shreds[i]); //assign each shred to a separate thread

    }
    ofstream ivf;
    ivf.open(iv_file_name,ios::out);//create andopen file to store iv
   	if (ivf.is_open()){//make sure file is open successfully
        ivf.write (reinterpret_cast<const char*>(iv),sizeof(iv));//write iv in file
   		ivf.close();
   	}else
        cout << "Unable to open iv file.\n";
    
    tm.start(); //calling start function in thread manager class to start the thread
    tm.barrier();//calling the barrier function in thread manager to wait and join the threads

    que.dump(q_file_name, key_file_name, iv_file_name); //calling dump function in class multiheadqueue to encrypt the vector and store it in the queue file

    return true;
}
bool MultithreadedShredManager::decrypt (FileSpooler * p_fileSpooler, char * key_file_name,  char * iv_file_name, char * q_file_name)
{
    //This function needed to be implemented by you

    ThreadManager tm;//instantiate object of class thread manager
    MultiHeadQueue <sb_block_index> que;//instantiate object of class MultiHeadQueue passing struct sb_block_index as the type for the template
    
    que.load(q_file_name, key_file_name, iv_file_name);//calling load function in class multiheadqueue to read encrypted queue file\
                                                        //decrypt it and store it in the vector
    
       for ( char i = 0 ; i  < shred_count; i++) //loop on all shreds
    {
        string fname = file_name;//store path of output file in fname
        fname.insert(fname.find('.'),1,i+'A');//store name of every shred using letters A,B...


        if(truncate)
            shreds[i] = new  DecryptShredThread(p_fileSpooler,key_file_name,iv_file_name,fname.c_str(), block_size,&que,i+'A',truncate);
        //for each shred instantiate object of class DecryptShredThread and store it in array of shreds

        else
            shreds[i] = new  DecryptShredThread(p_fileSpooler,key_file_name,iv_file_name,fname.c_str(), (block_size+16)&~15,&que,i+'A',truncate);
        //do the same, while padding incomplete blocks

        tm += dynamic_cast<Thread*>(shreds[i]);//assign each shred to a separate thread
    }
      
    tm.start(); //calling start function in thread manager class to start the thread
    tm.barrier();//calling the barrier function in thread manager to wait and join the threads

    return true;
}

MultithreadedShredManager::~MultithreadedShredManager()
{

}
