#include <FileSpooler.h>

Block * FileSpooler::getBlockLockFree()
{
    Block * b = new Block (block_size);//instantiate an object of class block
    if (b->load(f))//if block was stored in buffer successfully, return block pointer
        return b;
    else {//if unsuccessful, delete block pointer and return NULL
        delete (b);
        return NULL;
    }
}


FileSpooler::FileSpooler(const char * p_file_name, uint16_t p_block_size, bool truncate)
{
    //This function needed to be implemented by you

    //open file and initialize filesize to zero
    fsize = 0;
    if ( truncate ){
        f.open(p_file_name,ios::in|ios::out|ios::trunc);
    }

    else {f.open(p_file_name,ios::in|ios::out);
    }

    if ( f.is_open())
    {
        long cur = f.tellg();//returns current position of pointer in the file
        f.seekg(0,f.end); //move pointer to end of file
        fsize = f.tellg (); //store position of pointer in fsize to store the size of the file
        f.seekg(cur,f.beg);//rewind pointer to beginning of the file
    }
    else
        cout << "Unable to open file.\n";

    block_size = p_block_size;
}
 Block * FileSpooler::operator [] (long index)
 {
    mtx.lock(); //start of critical section
    if ( index*block_size >= fsize) 
    {
        mtx.unlock();
        return NULL;
    }
    long cur = f.tellg ();//returns current position 
    f.seekg(index*block_size,f.beg);//move pointer from beginning to the position of the block (block number * block size)
    Block * b  = getBlockLockFree();//returns pointer to the next block
    f.seekg(cur,f.beg); //rewind pointer to beginning of file
    mtx.unlock();//end of critical section
    return b;
 }
long FileSpooler::getBlockCount()
{
    if (fsize%block_size == 0 )//if file size is a multiple of blocksize, return fsize/block_size which is the number of blocks
    {
        return fsize/block_size;

    }
    else //if file size is not a multiple of block_size then return fsize/block_size + 1 to compensate for the incomplete block size
    {
        return fsize/block_size + 1;
    }
}


Block * FileSpooler::getNextBlock()
{
    mtx.lock(); //start of critical section
    Block * b  = getBlockLockFree(); //returns pointer to the next block
    mtx.unlock ();//end of critical section
    return b;
}
void FileSpooler::appendBlock (Block * b)
{
    mtx.lock();//start of critical section
    b->store(f);//takes data from buffer to file
    mtx.unlock();//end of critical section
}
void FileSpooler::writeBlockAt(Block *b,long block_index)// 
{    //This function needed to be implemented by you

    mtx.lock();//start of critical section
    f.seekp(block_index*block_size); //move pointer to the position of the block that was decrypted to write in the file
    b->store(f);//write from the buffer into the file
    mtx.unlock();//end of critical section
}
FileSpooler::~FileSpooler()
{
    if ( f.is_open()) f.close(); //if file is open, close it
}
