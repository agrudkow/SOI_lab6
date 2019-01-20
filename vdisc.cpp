#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include "vdisc.h"

#define BLOCK_SIZE 2048
#define NAME_SIZE 256

using namespace std;

struct superBlock{
  char name[NAME_SIZE];
  unsigned int size;
  unsigned int freeSpace;
};

struct node{
  unsigned int nodeId;
  char name[NAME_SIZE];
  unsigned int fileSize;
  unsigned int nextNode;
  char in_use;
  char is_last;
  //2B - offset
};

struct dataBlock{
  char data[BLOCK_SIZE];
};

//Conver string to integer
constexpr unsigned int str2int(const char* str, int h = 0){
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

void interface() {
  string command, pwd = "";
  bool loopStatus = true;
  while (loopStatus) {
    cout << pwd << "$ ";
    getline(cin, command);
    istringstream iss(command);

    switch (str2int((const char*)command.data())) {
      case str2int("exit"): loopStatus = false; break;
      case str2int("print"): printf("%lu\n", sizeof(node)); break;
      case str2int("create"): createVDisc("dysk1.img", 1024); break;
      case str2int("open"): openVDisc("dysk1.img"); break;
      case str2int("delete"): deleteVDisc("dysk1.img"); break;
      default: cout << "Invalid command \n"; continue;
    }
  }
}

int createVDisc(string name, unsigned int size) {
  unsigned int discSize = size, freeSpace = size - 1;
  //creating a file of given size
  ofstream vdisc(name, ios::binary | ios::out);
  if (!vdisc) return 1;
  //Setting secified size
  vdisc.seekp(size - 1);
  vdisc.write("", 1);

  vdisc.seekp(0);

  vdisc.write((char*)name.data(), NAME_SIZE);
  vdisc.write((char*)&discSize, sizeof(unsigned int));
  vdisc.write((char*)&freeSpace, sizeof(unsigned int));


  vdisc.close();
  return 0;
}

int openVDisc(string name) {
  char discName[NAME_SIZE];
  //char discSize[4];
  //char discFreeSpace[4];
  unsigned int discSize , freeSpace;

  ifstream vdisc(name, ios::binary | ios::in);
  if (!vdisc) return 1;

  vdisc.read(discName, (int)NAME_SIZE);
  vdisc.read((char*)&discSize, 4);
  vdisc.read((char*)&freeSpace, 4);
  cout << discSize << endl;
  cout << freeSpace << endl;

  vdisc.close();
  return 0;
}

void closeVDisc(string name) {
  /* code */
}

int deleteVDisc(string name) {
  if (remove((const char*)name.data()) != 0)
    perror("Error deleting vdisc");
  else
    cout << "Vdisc successfully deleted" << endl;

  return 0;
}

void renameVDisc(string oldName, string newName) {
  /* code */
}
