#include <CommandLineArguments.h>
#include <Lottery.h>

AVLTree <avl_shelf_index_t> avltree;

int main(int argc, char **argv)
{    
    time_t start = time(NULL);//variable of type time that starts when running the program
    CommandLineArguments cmlarg; //instantiate object of class CommandLineArguments 
    if (cmlarg.parser(argc,argv)) //making sure command line was parsed successfully
    {
        if (!cmlarg.execute()) //error message if command line is incorrect
        {
            if ( strcmp(cmlarg.getErrorString(),"") != 0 )
                printf ("%s\n",cmlarg.getErrorString());
        }        
    }
    else
    {          //error message if command line is not parsed correctly
        if ( strcmp(cmlarg.getErrorString(),"") != 0 )
            printf ("%s",cmlarg.getErrorString());
        printf ("usage: %s --command <import/export> --process-type <asc/desc> --input-file <file-name> --output-file <file-name> --working_dir <working directory full path> --key-file <Full path to file containing the AES key> --block-size <size of a block> --shreds <# of shreds>\n",argv[0]);
        exit(1);
   }
    time_t end = time(NULL);//returns total time the program took to run
    printf ("Total time: %lu\n",end-start); //print total time elapsed 
   
 
    return 0;
}