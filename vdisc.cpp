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
  string command, pwd = "", fileName;
  bool loopStatus = true;
  while (loopStatus) {
    cout << pwd << "$ ";
    getline(cin, command);
    istringstream iss(command);

    switch (str2int((const char*)command.data())) {
      case str2int("exit"): loopStatus = false; break;
      case str2int("print"): printf("%lu\n", sizeof(node)); break;
      case str2int("create"): createVDisc("dysk2.txt", 8); break;
      case str2int("open"): openVDisc("dysk2.txt"); break;
      case str2int("delete"): deleteVDisc("dysk2.txt"); break;
      case str2int("copy"): cin >> fileName; copyToVdisc(fileName, "dysk2.txt"); break;
      default: cout << "Invalid command \n"; continue;
    }
  }
}

unsigned int nodeOffset(unsigned int id, fstream &vdisc) {
  unsigned int size, numOfNodes, offset, posP, posG;
  posP = vdisc.tellp();
  posG = vdisc.tellg();
  vdisc.seekg(0);
  vdisc.seekg(NAME_SIZE);
  vdisc.read((char*)&size, sizeof(size));
  numOfNodes = (size - sizeof(superBlock)) / (sizeof(node) + BLOCK_SIZE);
  if (id > numOfNodes)
    return 0;
  offset = sizeof(superBlock) + (id - 1) * sizeof(node);

  vdisc.seekp(posP);
  vdisc.seekg(posG);

  return offset;
}

unsigned int findNextFreeNode(fstream &vdisc){
  char in_use = 1, is_last = 0;
  unsigned int fileSize, nodeId = 0, offset, posP, posG;
  posP = vdisc.tellp();
  posG = vdisc.tellg();

  while (in_use != 0 && is_last != 1) {
      nodeId++;
      //cout << nodeId << endl;
      offset = nodeOffset(nodeId, vdisc);
      //cout << offset << endl;
      vdisc.seekg(offset + sizeof(nodeId) + NAME_SIZE + 2 * sizeof(fileSize));
      vdisc.read((char*)&in_use, sizeof(in_use));
      vdisc.read((char*)&is_last, sizeof(is_last));
  }
  vdisc.seekp(posP);
  vdisc.seekg(posG);

  cout << nodeId << endl;

  return nodeId;
}

unsigned int dataBlockOffset(fstream &vdisc, unsigned int nodeId){
  unsigned int size, numOfNodes, offset, posP, posG;
  posP = vdisc.tellp();
  posG = vdisc.tellg();

  vdisc.seekg(0);
  vdisc.seekg(NAME_SIZE);
  vdisc.read((char*)&size, sizeof(size));
  numOfNodes = (size - sizeof(superBlock)) / (sizeof(node) + BLOCK_SIZE);

  vdisc.seekg(0);
  vdisc.seekg(sizeof(superBlock) + numOfNodes * sizeof(node) + nodeId * sizeof(dataBlock));
  offset = vdisc.tellg();

  vdisc.seekp(posP);
  vdisc.seekg(posG);

  return offset;
}
//DO USUNIECIA
unsigned int findNodeId(unsigned int id, fstream &vdisc){
  unsigned int nodeId, offset;
  char is_last;
  vdisc.seekp(sizeof(superBlock));

  while (true) {
    offset = vdisc.tellg();
    vdisc.read((char*)&nodeId, sizeof(nodeId));
    vdisc.seekp(vdisc.tellg() + NAME_SIZE + 2 * sizeof(int) + sizeof(char));
    vdisc.read((char*)&is_last, sizeof(is_last));
    if (nodeId == id && is_last != 1)
      return offset;
    else if (is_last == 1)
      return 0;
    }

  return 0;
}

int createVDisc(string name, unsigned int size) {
  unsigned int discSize = size * 1024, freeSpace = size * 1024, numOfNodes, nodeId, nextNodeId, offset;
  char in_use = 0, is_last = 0;
  //Counting size of user space
  freeSpace -= sizeof(superBlock);
  numOfNodes = freeSpace / (sizeof(node) + BLOCK_SIZE);
  cout << numOfNodes << endl;
  freeSpace = numOfNodes * BLOCK_SIZE;

  //creating a file of given size
  fstream disc(name, fstream::binary | fstream::out | fstream::in);
  disc.close();
  fstream vdisc;
  vdisc.open(name, fstream::out | fstream::in);
  if (!vdisc) return 1;
  //Setting secified size
  vdisc.seekp(size * 1024 - 1);
  vdisc.write("", 1);
  vdisc.seekp(0);

  vdisc.write((char*)name.data(), NAME_SIZE);
  vdisc.write((char*)&discSize, sizeof(unsigned int));
  vdisc.write((char*)&freeSpace, sizeof(unsigned int));
  vdisc.seekp(0);

  for (int i = 0; i < numOfNodes; i++) {
    nodeId = i + 1;
    nextNodeId = nodeId + 1;
    offset = nodeOffset(nodeId, vdisc);
    vdisc.seekp(offset);
    //cout <<"Pos: "<< vdisc.tellp()<<endl;
    vdisc.write((char*)&nodeId, sizeof(nodeId));
    cout << nodeId << endl;
    vdisc.seekp(vdisc.tellp() + NAME_SIZE);
    //cout << vdisc.tellp() << endl;
    vdisc.seekp(vdisc.tellp() + sizeof(size));
    //cout << vdisc.tellp() << endl;
    vdisc.write((char*)&nextNodeId, sizeof(nodeId));
    //cout << vdisc.tellp() << endl;
    vdisc.write((char*)&in_use, sizeof(in_use));
    if ((i + 1) != numOfNodes)
      vdisc.write((char*)&is_last, sizeof(is_last));
    else{
      is_last = 1;
      vdisc.write((char*)&is_last, sizeof(is_last));
    }
    }
  vdisc.seekp(0);

  vdisc.close();
  return 0;
}

int openVDisc(string name) {
  char discName[NAME_SIZE];
  unsigned int discSize , freeSpace, nodeId, nextNodeId, size, id=0, offset;
  char is_last, fileName[NAME_SIZE], in_use;
  bool endOfDisc = false;

  fstream vdisc;
  vdisc.open(name, ios::in | ios::out);
  if (!vdisc) return 1;

  vdisc.read(discName, (int)NAME_SIZE);
  vdisc.read((char*)&discSize, sizeof(discSize));
  vdisc.read((char*)&freeSpace, sizeof(discSize));
  vdisc.seekg(0);
  cout << discName << endl;
  cout << discSize << endl;
  cout << freeSpace << endl;

  while (!endOfDisc) {
    id++;
    //cout << id << " ";
    offset = nodeOffset(id, vdisc);
    //cout << offset << endl;
    vdisc.seekg(offset);
    vdisc.read((char*)&nodeId, sizeof(nodeId));
    cout << nodeId <<"  ";
    vdisc.read(fileName, sizeof(fileName));
    cout.width(20);
    cout << fileName << "  ";
    vdisc.read((char*)&size, sizeof(size));
    cout.width(5);
    cout << size << "  ";
    vdisc.read((char*)&nextNodeId, sizeof(nextNodeId));
    cout.width(5);
    cout << nextNodeId << "  ";
    vdisc.read((char*)&in_use, sizeof(in_use));
    cout.width(5);
    cout << (int)in_use << "  ";
    //vdisc.seekp(vdisc.tellg() +  NAME_SIZE + 2 * sizeof(discSize) + sizeof(is_last));
    vdisc.read((char*)&is_last, sizeof(is_last));
    cout.width(5);
    cout << (int)is_last << endl;
    if (is_last == 1) endOfDisc = true;
    getchar();
  }
/*
  do {

    vdisc.read((char*)&nodeId, sizeof(nodeId));
    cout << nodeId <<"  ";
    vdisc.read(fileName, sizeof(fileName));
    cout.width(20);
    cout << fileName << "  ";
    vdisc.read((char*)&size, sizeof(size));
    cout.width(5);
    cout << size << "  ";
    vdisc.read((char*)&nextNodeId, sizeof(nextNodeId));
    cout.width(5);
    cout << nextNodeId << "  ";
    vdisc.read((char*)&in_use, sizeof(in_use));
    cout.width(5);
    cout << (int)in_use << "  ";
    //vdisc.seekp(vdisc.tellg() +  NAME_SIZE + 2 * sizeof(discSize) + sizeof(is_last));
    vdisc.read((char*)&is_last, sizeof(is_last));
    cout.width(5);
    cout << (int)is_last << endl;
    vdisc.seekp(vdisc.tellg() +  2);
  } while((int)is_last != 1);
*/
  vdisc.seekg(0);

  vdisc.close();
  return 0;
}

int copyToVdisc(string fileName, string discName) {
  fstream inFile;
  fstream vdisc;
  unsigned int fileSize, fileSizeLeftToCopy, freeSpace, nodeId = 0, nextNodeId, offset;
  char in_use = 0, is_last = 0;
  char memChunck[BLOCK_SIZE];
  char Name[NAME_SIZE];
  bool endOfFile = false;

  vdisc.open(discName, fstream::out | fstream::in);
  if (!vdisc)
    return  -1;

  vdisc.read(Name, NAME_SIZE);
  vdisc.seekg(0);
  cout << Name << endl;
  vdisc.seekp(sizeof(superBlock) - sizeof(freeSpace));
  vdisc.read((char*)&freeSpace, sizeof(freeSpace));
  vdisc.seekg(0);

  inFile.open(fileName, fstream::out | fstream::in);
  if (inFile) {
    inFile.seekg( 0, ios::end );
    fileSize = inFile.tellg();
    inFile.seekg( 0, ios::beg );

    cout << fileSize << " " << freeSpace << endl;

    if(fileSize > freeSpace){
      vdisc.close();
      inFile.close();
      return -1;
    }

    fileSizeLeftToCopy = fileSize;

    while (!endOfFile) {
      vdisc.seekp(0);
      nodeId = findNextFreeNode(vdisc);
      offset = nodeOffset(nodeId, vdisc);
      cout << fileSizeLeftToCopy << endl;
      vdisc.seekp(offset + sizeof(nodeId));
      vdisc.write((char*)fileName.data(), NAME_SIZE);
      vdisc.write((char*)&fileSize, sizeof(fileSize));
      //nextNodeId = findNextFreeNode(vdisc);
      //vdisc.write((char*)&nextNodeId, sizeof(nextNodeId));
      vdisc.seekp(vdisc.tellp() + sizeof(nextNodeId));
      if (in_use == 0)
        in_use = 1;
      else
        in_use = 2;
      vdisc.write((char*)&in_use, sizeof(in_use));

      offset = dataBlockOffset(vdisc, nodeId);
      vdisc.seekp(offset);
      inFile.seekg(0);
      if (fileSizeLeftToCopy >= BLOCK_SIZE){
        inFile.read((char*)&memChunck, sizeof(memChunck));
        vdisc.write((char*)&memChunck, sizeof(memChunck));
        fileSizeLeftToCopy -= BLOCK_SIZE;
        freeSpace -=BLOCK_SIZE;
        if (fileSizeLeftToCopy == 0) endOfFile = true;
      } else {
        inFile.read((char*)&memChunck, fileSizeLeftToCopy);
        vdisc.write((char*)&memChunck, fileSizeLeftToCopy);
        freeSpace -=BLOCK_SIZE;
        endOfFile = true;
      }
      getchar();
    }

    vdisc.seekp(sizeof(superBlock) - sizeof(freeSpace));
    vdisc.write((char*)&freeSpace, sizeof(freeSpace));
    vdisc.seekp(0);

    vdisc.close();
    inFile.close();
    return 0;
  }
  inFile.close();
  return -1;
}

void copyToMem(/* arguments */) {
  /* code */
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
