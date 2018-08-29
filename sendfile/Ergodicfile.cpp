#include <io.h>  
#include <fstream>  
#include <string>  
#include <vector>  
#include <iostream>
#include"findname.h"

using namespace std;


//获取所有的文件名  
void GetAllFiles(string path, vector<string>& files)
{

	long   hFile = 0;
	//文件信息    
	struct _finddata_t fileinfo;//用来存储文件信息的结构体    
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)  //第一次查找  
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))  //如果查找到的是文件夹  
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)  //进入文件夹查找  
				{
					//files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					GetAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else //如果查找到的不是是文件夹   
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));  //将文件路径保存，也可以只保存文件名:    p.assign(path).append("\\").append(fileinfo.name)  
			}

		} while (_findnext(hFile, &fileinfo) == 0);

		 
	}
	else
	{
		files.push_back(p.assign(path));
	}
	_findclose(hFile); //结束查找  
}

//获取特定格式的文件名  
void GetAllFormatFiles(string path, vector<string>& files, string format)
{
	//文件句柄    
	long   hFile = 0;
	//文件信息    
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					//files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
					GetAllFormatFiles(p.assign(path).append("\\").append(fileinfo.name), files, format);
				}
			}
			else
			{
				files.push_back(p.assign(fileinfo.name));  //将文件路径保存，也可以只保存文件名:    p.assign(path).append("\\").append(fileinfo.name)  
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}

bool IsDir(string path, vector<string>& files)
{
	
	long   hFile = 0;
	//文件信息    
	struct _finddata_t fileinfo;//用来存储文件信息的结构体    
	string p;
	if ((hFile = _findfirst(p.assign(path).c_str(), &fileinfo)) != -1)  //第一次查找  
	{
		if ((fileinfo.attrib &  _A_SUBDIR))  //如果查找到的是文件夹  
		{
			return true;
		}
		else //如果查找到的不是是文件夹   
		{
			return false;
		}
		_findclose(hFile); //结束查找  
	}

}

int Isfile(string path, vector<string>& files, int a)
{

	long   hFile = 0;
	//文件信息    
	struct _finddata_t fileinfo;//用来存储文件信息的结构体    
	string p;
	if ((hFile = _findfirst(p.assign(path).c_str(), &fileinfo)) != -1)  //第一次查找  
	{
		if ((fileinfo.attrib &  _A_SUBDIR))  //如果查找到的是文件夹  
		{
			a = a + 1;
			return a;
		}
		else //如果查找到的不是是文件夹   
		{
			a = a + 2;
			return a;
		}
		_findclose(hFile); //结束查找  
	}

}

// 该函数有两个参数，第一个为路径字符串(string类型，最好为绝对路径)；  
// 第二个参数为文件夹与文件名称存储变量(vector类型,引用传递)。  
// 在主函数中调用格式(并将结果保存在文件"AllFiles.txt"中，第一行为总数)：  

//int main()
//{
	//string filePath = "C:\\222";
	//vector<string> files;
	//char * distAll = "AllFiles.txt";

	//读取所有的文件，包括子文件的文件  
	//GetAllFiles(filePath, files);  

	//读取所有格式为jpg的文件  
	//string format = ".dll";
	//GetAllFormatFiles(filePath, files, format);
	//ofstream ofn(distAll);
	//int size = files.size();
	//ofn << size << endl;

	//for (int i = 0; i<size; i++)
	//{
		//ofn << files[i] << endl; // 写入文件  
		//cout << files[i] << endl;
		//cout << files[i] << endl;//输出到屏幕  
	//}
	//cout << files[0] << endl;
	//ofn.close();

	//return 0;
//}
