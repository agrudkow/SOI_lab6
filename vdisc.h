#ifndef VDISC_H
#define VDISC_H
//This header file contain basic operations for handling our virtual disc
/*------------------dics-structure-----------------------
***Suberbolck
*256B - disc name
*4B - size
*4B - free space
***node
*4B - node id
*256B - file name
*1B - in_use flag(can bo 0 - not in use, 1 - in use, begining of the file, or 2 - in use, continuation of the file)
*1B -is_last flag, 1 if it is last node, 0 if not
*4B - file size
*4B - next node (if does not exist equals 0)
*2b - offset
*** data block
*2kB
*-------------------------------------------------------*/
//user interface -- contain main loop of the menagement system
void interface();

int createVDisc(std::string name, unsigned int size);

int openVDisc(std::string name);

void closeVDisc(std::string name);

int deleteVDisc(std::string name);

void renameVDisc(std::string oldName, std::string newName);



#endif
