#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>

#include "vdisc.h"

using namespace std;

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
      case str2int("print"): printf("Interface\n"); break;
      default: continue;
    }
  }
}

void createVDisc(/* arguments */) {
  /* code */
}

void openVDisc(/* arguments */) {
  /* code */
}

void closeVDisc(/* arguments */) {
  /* code */
}

void deleteVDisc(/* arguments */) {
  /* code */
}

void renameVDisc(/* arguments */) {
  /* code */
}
