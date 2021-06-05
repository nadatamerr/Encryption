
#include <SafeBox.h>

SafeBox::SafeBox ()
{

}
SafeBox::~SafeBox()
{

}
SafeBoxImport::SafeBoxImport (){

}
void SafeBoxImport::process (char * input_file,
                            char * output_file,
                            char * working_dir,
                            char * key_file,
                            uint16_t block_size,
                            uint16_t shreds){
        printf ("Import file\n");
        char outf [1024];//array of chars to store path of output file
        char outiv [1024];//array of chars to store path of  iv file
        char outq [1024];//array of chars to store path of queue file
		
		char outshelf[1024];

        memset (outf,0,1024);//initialize array of chars
        memset (outiv,0,1024);//initialize array of chars
        memset (outq,0,1024);//initialize array of chars

		memset (outshelf, 0, 1024);
        
        sprintf (outf,"%s/%s",working_dir,output_file);//storing path of output file in outf
        sprintf (outiv,"%s/%s.iv",working_dir,output_file);//storing path of iv file in outiv
        sprintf (outq,"%s/%s.queue",working_dir,output_file);//storing path of queue file in outq

		sprintf	(outshelf, "%s/shelf.index", working_dir))

        //This function needed to be implemented by you

        MultithreadedShredManager shredManager(outf, block_size,shreds, true);
        //instantiate object of class MultiThreadedShredManager passing true as the truncate parameter to encrypt

        FileSpooler fileSpooler (input_file,block_size,false);
        //instantiate object of class FileSpooler passing false as the truncate parameter to encrypt

        shredManager.encrypt(&fileSpooler,key_file,outiv,outq);
        //calling encrypt function of class MultiThreadedShredManager


		ofstream sh;// create a file
		sh.open(outshelf, ios::out | ios::trunc);// open the file 
		if (sh.is_open())// make sure that the file is open 
		{
			avl_shelf_index_t *n = avlTree.getRoot();// create a node and store the root of the tree
			//call process to write the contant of the tree into the file 
			avlTree.process([](n)->void {
				if (n->deleted == false) {
					sh.write(n->input_file_name, sizeof(n->input_file_name));
					sh.write((char*)n->block_size, sizeof(n->block_size));
					sh.write((char*)n->blocks, sizeof(n->blocks));
					sh.write((char*)n->fsize, sizeof(n->fsize));
					sh.write((char*)n->shreds, sizeof(n->shreds));
					sh.write(n->outputfile, sizeof(n->outputfile));
					sh.write((char*)n->deleted, sizeof(n->deleted));
				}
				sh.write()
			}, "asc");	
			sh.close();// close the file
		}

}
SafeBox * SafeBoxImport::clone (){
    return new SafeBoxImport();//returns new object of class SafeBoxImport
}
SafeBoxImport::~SafeBoxImport(){

}

SafeBoxExport::SafeBoxExport (){

}
void SafeBoxExport::process (char * input_file,
                            char * output_file,
                            char * working_dir,
                            char * key_file,
                            uint16_t block_size,
                            uint16_t shreds){
        printf ("Export file\n");
        char inf [1024];//array of chars to store path of input file
        char iniv [1024];//array of chars to store path of iv file
        char inq [1024];//array of chars to store path of queue file

        memset (inf,0,1024);//initialize array of chars
        memset (iniv,0,1024);//initialize array of chars
        memset (inq,0,1024);//initialize array of chars

        sprintf (inf,"%s/%s",working_dir,input_file);//storing path of input file in inf
        sprintf (iniv,"%s/%s.iv",working_dir,input_file);//storing path of iv file in iniv
        sprintf (inq,"%s/%s.queue",working_dir,input_file);//storing path of queue file inq

        //This function needed to be implemented by you

		MultithreadedShredManager shredManager(inf, block_size, shreds, false);
        //instantiate object of class MultiThreadedShredManager passing false as the truncate parameter to decrypt
        
        FileSpooler fileSpooler (output_file,block_size,true);
        //instantiate object of class FileSpooler passing true as the truncate parameter to decrypt

        shredManager.decrypt(&fileSpooler,key_file,iniv,inq);
        //calling decrypt function of MultiThreadedShredManager
}
SafeBox * SafeBoxExport::clone (){
    return new SafeBoxExport(); //returns new object of class SafeBoxExport
}
SafeBoxExport::~SafeBoxExport(){
}

SafeBoxList::SafeBoxList(char* list) {
	listType = list;
}
void SafeBoxList::process(char * working_dir, char * key_file){
	
		avltree.process([](avlTree.getRoot())->void {
			cout << "File name: " << n->filename << endl;
			cout << "Block size: " << n->block_size << endl;
			cout << "Number of blocks: " << n->blocks << endl;
			cout << "File size: " << n->fsize << endl;
			cout << "Number of shreds: " << n->shreds << endl;
			cout << "Output file name: " << n->outputfile << endl;
			cout << "Flag deleted: " << n->deleted << endl;
		}, listType);
        

}
SafeBox * SafeBoxList::clone(char* list) {
	return new SafeBoxList(char* list); //returns new object of class SafeBoxList
}
SafeBoxList::~SafeBoxList() {
}



SafeBoxDelete::SafeBoxDelete(char* input_file_name) {
	filename = input_file_name;
}
void SafeBoxDelete::process(char * working_dir, char * key_file) {
	
	//decrypt shelf index


	NODE* s = avlTree.find(filename);
	s->deleted = true;

	ofstream sh;
	sh.open(outshelf, ios::out | ios::trunc);
	if (sh.is_open()) {
		avl_shelf_index_t *n = avlTree.getRoot();
		avlTree.process([](n)->void {
			if (n->deleted == false) {
				sh.write(n->input_file_name, sizeof(n->input_file_name));
				sh.write((char*)n->block_size, sizeof(n->block_size));
				sh.write((char*)n->blocks, sizeof(n->blocks));
				sh.write((char*)n->fsize, sizeof(n->fsize));
				sh.write((char*)n->shreds, sizeof(n->shreds));
				sh.write(n->outputfile, sizeof(n->outputfile));
				sh.write((char*)n->deleted, sizeof(n->deleted));
			}
			sh.write()
		}, "asc");
		sh.close();
	}

}
SafeBox * SafeBoxDelete::clone(char* input_file_name) {
	return new SafeBoxDelete(char* input_file_name); //returns new object of class SafeBoxList
}
SafeBoxDelete::~SafeBoxDelete() {
}

