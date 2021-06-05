#include <Shred.h>

Shred::Shred(const char * p_file_name,uint16_t block_size,bool truncate){
    fileSpooler = new FileSpooler(p_file_name,block_size,truncate); //instantiate object of class FileSpooler that takes the parameters: file name, block size and truncate. 
}
void Shred::operator << (Block & b){
    fileSpooler->appendBlock(&b); //stores block from buffer to the file
}
Block * Shred::getNextBlock ()
{
    return fileSpooler->getNextBlock(); // return pointer to the next block
}

Shred::~Shred()
{
    delete (fileSpooler);
}
