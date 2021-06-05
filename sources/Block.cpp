#include <Block.h>
#include <stdio.h>

Block::Block (uint32_t p_block_size)
{
    // allocating memory for buffer
    block_size = p_block_size;
    read_size = 0;
    buffer = (char *) calloc (block_size+1+(block_size-(block_size%CryptoPP::AES::BLOCKSIZE)),sizeof(char));
}
bool Block::load(fstream & f)
{
//if input file was open successfully, it will read a block and store it in the buffer and store its actual size in read_size

    if ( f.is_open())
    {
        f.read(buffer,block_size);
        read_size = f.gcount();
        if ( f.eof() ) f.clear(); 
        if (f || f.gcount() > 0 ) return true;
        else return false;
    }
    else return false;
}
bool Block::store(fstream & f)
{
//if the file is open, read a block into the buffer and retrun true. lse, return false.

    if ( f.is_open())
    {
        f.write(buffer,read_size);
        return true;
    }
    else return false;
}

void Block::encrypt(CryptoPP::byte * key, CryptoPP::byte * iv)
{
    //instantiate object of class AES::Encryption that takes the key and its length
    //pass the aesEncryption object and the iv to object of CBC Mode class
    //store encrypted text in string ciphertext
    //store size of ciphertext in read_size
    //store ciphertext in buffer

    std::string ciphertext;
    CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );
    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink( ciphertext ) );
    stfEncryptor.Put( reinterpret_cast<const unsigned char*>( buffer ), read_size );
    stfEncryptor.MessageEnd();
    memcpy (buffer,ciphertext.c_str(),ciphertext.size());
    read_size = ciphertext.size();
}
void Block::decrypt(CryptoPP::byte * key, CryptoPP::byte * iv)
{

    //instantiate object of class AES::Decryption that takes the key and its length
    //pass the aesDecryption object and the iv to object of CBC Mode class
    //store decrypted text in string plaintext
    //store size of plaintext in read_size
    //store plaintext in buffer

    
    std::string decryptedtext;
    CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );
    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink( decryptedtext ) );
    stfDecryptor.Put( reinterpret_cast<const unsigned char*>( buffer ), read_size );
    stfDecryptor.MessageEnd();
    read_size = (uint16_t) decryptedtext.size();
    memcpy (buffer,decryptedtext.c_str(),read_size);
}
void Block::print ()
{
    cout << buffer;//prints content of buffer
}
Block::~Block()
{
    if ( buffer != NULL) free (buffer);
}
