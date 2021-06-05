#include <CommandLineArguments.h>


CommandLineArguments::CommandLineArguments()
{
    //initializing each variable that will store the command line arguments with zeros
    memset(input_file_name,0,COMMAND_LINE_ARGUMENT_MAX_SIZE);
    memset(output_file_name,0,COMMAND_LINE_ARGUMENT_MAX_SIZE);
    memset(working_dir,0,COMMAND_LINE_ARGUMENT_MAX_SIZE);
    memset(command,0,COMMAND_LINE_ARGUMENT_MAX_SIZE);
    memset(key_file,0,COMMAND_LINE_ARGUMENT_MAX_SIZE);
    memset(error_string,0,ERROR_STRING_MAX_SIZE);
	//memset(processType, 0, COMMAND_LINE_ARGUMENT_MAX_SIZE);

    //initalizing block_size and shreds with 1024 and 10 respectively
    block_size = 1024;
    shreds = 10;
    commands["import"] = new SafeBoxImport(); //if command given is import, then instantiate object of class SafeBoxImport
    commands["export"] = new SafeBoxExport(); //if command given is export, then instantiate object of class SafeBoxExport
	commands["asc"] = new SafeBoxList(command);//if command given is asc, then instantiate object of class list
	commands["desc"] = new SafeBoxList(command);//if command given is desc, then instantiate object of class list
	commands["delete"] = new SafeBoxDelete(input_file_name);//if command given is delete, then instantiate object of class delete
}
bool CommandLineArguments::parser(int argc,char ** argv)
{
    if ( argc %2 == 0 || argc < 2) return false;
    else
    {
        for ( uint8_t i = 1 ; i < argc ; i +=2)
        {
            //loop to store each command line argument in its corresponding variable
            if ( strcmp(argv[i],"--input-file") == 0)
                strncpy(input_file_name,argv[i+1],COMMAND_LINE_ARGUMENT_MAX_SIZE-1);
            else if ( strcmp(argv[i],"--output-file") == 0)
                strncpy(output_file_name,argv[i+1],COMMAND_LINE_ARGUMENT_MAX_SIZE-1);
            else if ( strcmp(argv[i],"--command") == 0)
                strncpy(command,argv[i+1],COMMAND_LINE_ARGUMENT_MAX_SIZE-1);
            else if ( strcmp(argv[i],"--working-dir") == 0)
                strncpy(working_dir,argv[i+1],COMMAND_LINE_ARGUMENT_MAX_SIZE-1);
            else if ( strcmp(argv[i],"--key-file") == 0)
                strncpy(key_file,argv[i+1],COMMAND_LINE_ARGUMENT_MAX_SIZE-1);
            else if ( strcmp(argv[i],"--block-size") == 0)
                block_size =atoi(argv[i+1]);
            else if ( strcmp(argv[i],"--shreds") == 0)
                shreds =atoi(argv[i+1]);
			/*else if (strcmp(argv[i], "--process-type") == 0)
				strncpy(processType, argv[i + 1], COMMAND_LINE_ARGUMENT_MAX_SIZE - 1);*/
            else
            {
                //create error message for incorrect argument
                sprintf(error_string,"undefined parameter: %s\n",argv[i]);
                return false;
            }
        }

		if (strcmp(input_file_name, "") == 0 || strcmp(output_file_name, "") == 0 ||
			strcmp(command, "") == 0 || strcmp(working_dir, "") == 0 ||
			strcmp(key_file, "") == 0 || block_size == 0 || shreds == 0 || processType == 0) return false;
            //if user didnt provide an argument for any of the required command line args, return false
        else return true;
    }
}
bool CommandLineArguments::execute()
{
    if ( commands[command] )
    {
        SafeBox * sb = commands[command]->clone();//creating pointer to SafeBox, that points to either an object of class SafeBoxImport or SafeBoxExport, depending on command
        sb->process (processType,
					input_file_name,
                    output_file_name,
                    working_dir,
                    key_file,
                    block_size,
                    shreds);//calling the function process that corresponds to the given command (import/export)
        delete (sb);
        return true;
    }
    else 
    {
        sprintf(error_string,"undefined command: %s\n",command);
        return false;
    }
}
char * CommandLineArguments::getErrorString()
{
    return error_string; //returns error message if any occured in the function above
}
CommandLineArguments::~CommandLineArguments()
{
    
}
