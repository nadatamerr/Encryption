#include <Lottery.h>

Lottery::Lottery(unsigned long bit_count)
{
    //constructor initializing class members
    random_engine = new std::mt19937 (random_device());
    distribution = new std::uniform_int_distribution<std::size_t> (0,(bit_count<2)?0:bit_count-2);
    reserved = 0;
    if ( bit_count != 0 ) bit_count--;

    bitmap = new boost::dynamic_bitset <> (bit_count);
    final_block = false;
}

long Lottery::withdraw ()
{
    mtx.lock();//start of critical section
    if ( final_block)
    {
        mtx.unlock();
        return -1;//if this is the last block then unlock and return -1 as an indicator
    }
    else if (reserved == bitmap->size()) 
    {
        //if the number of reserved blocks are equal to the bitmap size (which is the number of blocks) then this is the final block so unlock and return size of bitmap as an indicator
        final_block = true;
        mtx.unlock();
        return bitmap->size();
    }
    
    auto randomNumber =(*distribution)(*random_engine);//generate random block number

    if (!bitmap->test(randomNumber))
    {
        bitmap->set(randomNumber);//if random number was not generated before then set it

        reserved++;//increment counter for reserved blocks
    }
    else {//if number generated is repeated
        auto r = (randomNumber +1)% bitmap->size();//generate another number
        for ( ; r != randomNumber; r = (r+1) % bitmap->size() )//loop until a unique block number is generated
        {
            if (!bitmap->test(r))
            {
                bitmap->set(r);//set unique random number
                reserved++;//increment counter of reserved blocks
                randomNumber =r;
                break;
            }
        }
    }
    mtx.unlock();//end of critical section
    return randomNumber;//return random block number generated
}

Lottery::~Lottery()
{
    delete (distribution);
    delete (random_engine);
    delete(bitmap);
}
