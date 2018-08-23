#ifndef FINDNAME_H
#define FINDNAME_H
#include <io.h>  
#include <fstream>  
#include <string>  
#include <vector>  
#include <iostream>
#include"findname.h"

using namespace std;

void GetAllFiles(string path, vector<string>& files);
void GetAllFormatFiles(string path, vector<string>& files, string format);
bool IsDir(string path, vector<string>& files);
int Isfile(string path, vector<string>& files, int a);

#endif
