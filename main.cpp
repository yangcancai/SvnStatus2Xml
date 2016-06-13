#include "tinyxml2.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <map>
#include <direct.h>  
#include <io.h> 
#include <vector>
using namespace tinyxml2;
using namespace std;
//检查文件夹是否存在，不存在则创建之  
//文件夹存在返回 0  
//文件夹创建失败返回-1  
//文件夹创建失败返回1  
int CheckDir(string Dir)
{
	FILE *fp = NULL;
	string TempDir;
	TempDir = Dir;
	TempDir += "\\.temp.fortest";
	fopen_s(&fp,TempDir.data(), "w");
	if (!fp)
	{
		if (_mkdir(Dir.data()) == 0)
		{
			return 1;//文件夹创建成功  
		}
		else
		{
			return -1;//can not make a dir;  
		}
	}
	else
	{
		fclose(fp);
	}
	return 0;
}
//字符串分割函数
std::vector<std::string> split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;//扩展字符串以方便操作
	int size = str.size();

	for (int i = 0; i<size; i++)
	{
		pos = str.find(pattern, i);
		if (pos<size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}
void help()
{
	// version and reights reserved.
	printf("SvnStatus2xml Ver 1.0 for Win64(x86)\n");
	printf("Copyright (c) 2016,2020, Cam and/or its affiliates. All rights reserved.\n\n");
	printf("Cam is a developer name.\n\n"
		"Usage: SvnStatus2xml [OPTIONS]\n"
		"--status_xml the svn status xml, such as revision.xml\n"
		"--out_put_dir the dst xml directory\n");

}
/*
函数入口：文件夹的绝对路径
const char*  dirPath

函数功能：删除该文件夹，包括其中所有的文件和文件夹

返回值：  0  删除
-1  路径不对，或其它情况，没有执行删除操作
*/
int  removeDir(const char*  dirPath)
{

	struct _finddata_t fb;   //查找相同属性文件的存储结构体  
	char  path[250];
	long    handle;
	int  resultone;
	int   noFile;            //对系统隐藏文件的处理标记  

	noFile = 0;
	handle = 0;


	//制作路径  
	strcpy_s(path, dirPath);
	strcat_s(path, "/*");

	FILE *fp = NULL;
	string TempDir;
	TempDir = dirPath;
	TempDir += "\\.temp.fortest";
	fopen_s(&fp, TempDir.data(), "w");
	if (!fp)
	{
		return -1;
	}
	else
	{
		fclose(fp);
		handle = _findfirst(path, &fb);
		//找到第一个匹配的文件  
		if (handle != 0)
		{
			//当可以继续找到匹配的文件，继续执行  
			while (0 == _findnext(handle, &fb))
			{
				//windows下，常有个系统文件，名为“..”,对它不做处理  
				noFile = strcmp(fb.name, "..");

				if (0 != noFile)
				{
					//制作完整路径  
					memset(path, 0, sizeof(path));
					strcpy_s(path, dirPath);
					strcat_s(path, "/");
					strcat_s(path, fb.name);
					//属性值为16，则说明是文件夹，迭代  
					if (fb.attrib == 16)
					{
						removeDir(path);
					}
					//非文件夹的文件，直接删除。对文件属性值的情况没做详细调查，可能还有其他情况。  
					else
					{
						remove(path);
					}
				}
			}
			//关闭文件夹，只有关闭了才能删除。找这个函数找了很久，标准c中用的是closedir  
			//经验介绍：一般产生Handle的函数执行后，都要进行关闭的动作。  
			_findclose(handle);
		}
		//移除文件夹  
		resultone = _rmdir(dirPath);
	}
	return  resultone;
}
void Exit()
{
	printf("\nPlease type enter key to exit. \n");
	getchar();
}
// 参数说明:  
// in， 源字符串  
// out, 存放最后结果的字符串  
// outlen，out最大的大小  
// src，要替换的字符串  
// dst，替换成什么字符串  
char *strrpl(char *in, char *out, int outlen, const char *src, char *dst)
{
	char *p = in;
	unsigned int  len = outlen - 1;

	// 这几段检查参数合法性  
	if ((NULL == src) || (NULL == dst) || (NULL == in) || (NULL == out))
	{
		return NULL;
	}
	if ((strcmp(in, "") == 0) || (strcmp(src, "") == 0))
	{
		return NULL;
	}
	if (outlen <= 0)
	{
		return NULL;
	}

	while ((*p != '\0') && (len > 0))
	{
		if (strncmp(p, src, strlen(src)) != 0)
		{
			int n = strlen(out);

			out[n] = *p;
			out[n + 1] = '\0';

			p++;
			len--;
		}
		else
		{
			strcat_s(out, outlen, dst);
			p += strlen(src);
			len -= strlen(dst);
		}
	}
	return out;
}
char *replacestr(char *strbuf, char sc, char dc)
{
	char *p = strbuf;
	while (*strbuf != '\0')
	{
		if (*strbuf == sc)
			*strbuf = dc;
		strbuf++;
	}
	return p;
}
int main(int argc, const char ** argv)
{
	const char* xml_file;
	const char* out_put_dir;
	try
	{
		// help
		if (argc == 2 && strcmp(argv[1], "--help") == 0)
		{
			help();
			Exit();
			return 0;
		}
		else if (argc >= 2)
		{
			// SvnStatus2Xml revisiion.xml
			if (argc == 2)
			{
				xml_file = argv[1];
				out_put_dir = "./";

			}
			// SvnStatus2Xml --status_xml revision.xml 
			else if (argc == 3 && strcmp(argv[1], "--status_xml") == 0)
			{
				xml_file = argv[2];
				out_put_dir = "./";

			}
			// SvnStatus2Xml --status_xml revison.xml --out_put_dir d://
			else if (argc == 5 && strcmp(argv[1], "--status_xml") == 0 &&
				strcmp(argv[3], "--out_put_dir") == 0)
			{
				xml_file = argv[2];
				out_put_dir = argv[4];
			}
			else
			{
				xml_file = "revision.xml";
				out_put_dir = "./";
			}
		}
		else
		{
			xml_file = "revision.xml";
			out_put_dir = "./";
		}
		string kkk = out_put_dir;
		kkk += "version";
		removeDir(kkk.data());
		FILE* fp;
		fopen_s(&fp, xml_file, "r");
		if (!fp) {
			printf("Error opening test file '%s'.\n"
				"Is your working directory the same as where \n"
				"the SvnStatus2Xml.exe and %s file are?\n\n"
				, xml_file, xml_file);
			Exit();
			return 0;
		}
		fclose(fp);
		printf("begain process..\n");
		XMLDocument doc;
		if (doc.LoadFile(xml_file) != XML_SUCCESS)
		{
			printf("Load %s File Error or xml format Error!!!\n",xml_file);
			Exit();
			return 0;
		}
		XMLDocument* mainDoc = new XMLDocument();
		XMLNode* element = mainDoc->InsertEndChild(mainDoc->NewElement("body"));
		map<string, XMLDocument*> mapVersion;
		mapVersion.insert(pair<string, XMLDocument*>("mainswf_version", mainDoc));
		XMLDocument* docTemp = NULL;
		std::map<string, XMLDocument*>::iterator it;
		XMLDocument* all = new XMLDocument();
		XMLNode* element_all = all->InsertEndChild(all->NewElement("body"));
		bool b = false;
		for (XMLElement* entry = doc.FirstChildElement("status")->FirstChildElement("target")->FirstChildElement("entry");
			entry;
			entry = entry->NextSiblingElement("entry"))
		{
			b = true;
			char* p = (char*)entry->Attribute("path");
			const char* cpathStr = replacestr(p, '\\', '/');
			XMLElement* status = entry->FirstChildElement("wc-status");
			XMLElement* revision = status->FirstChildElement("commit");
			if (strchr(cpathStr, '.'))
			{
				if (revision != NULL)
				{
					const char* crevisionStr = revision->Attribute("revision");
					// floder or only main swf file
					std::vector<std::string> v = split(cpathStr, "/");
					if (v.size() > 1)
					{
						string group_xml_name = v[0];
						it = mapVersion.find(group_xml_name);
						if (it != mapVersion.end())
						{
							docTemp = it->second;
						}
						else
						{
							XMLDocument* newXml = new XMLDocument();
							XMLNode* e = newXml->InsertEndChild(newXml->NewElement("body"));
							mapVersion.insert(pair<string, XMLDocument*>(group_xml_name, newXml));
							docTemp = newXml;
						}
					}
					else
					{
						docTemp = mainDoc;
					}
					// find if exist
					XMLElement* item = docTemp->NewElement("item");
					item->SetAttribute("path", cpathStr);
					item->SetAttribute("version", crevisionStr);
					element = docTemp->FirstChild();
					element->InsertEndChild(item);


					XMLElement* item_all = all->NewElement("item");
					item_all->SetAttribute("path", cpathStr);
					item_all->SetAttribute("version", crevisionStr);
					element_all = all->FirstChild();
					element_all->InsertEndChild(item_all);

					//if (v.size() > 1)
				}
				else
				{
					//printf("unversioned\n");
				}
			}
		}
		if (b == false)
		{
			printf("%s non enry node!!\n", xml_file);
			delete all;
			delete mainDoc;
			Exit();
			return 0;
		}
		else
		{
			string dir = out_put_dir;
			// check if exist the version floder
			int code = CheckDir((dir + "version"));
			if (code < 0)
			{
				printf("create %sversion floder error !!!!\n", dir.data());
				return 0;
			}
			// asset xml push to all xml
			it = mapVersion.find("asset");
			if (it != mapVersion.end())
			{
				docTemp = it->second;
				for (std::map<string, XMLDocument*>::iterator it_map = mapVersion.begin(); it_map != mapVersion.end(); it_map++)
				{
					if (it_map->first == "asset"||
						it_map->first=="mainswf_version")
						continue;
					XMLDocument* mapXmlDoc = it_map->second;
					//printf("begin %s\n", it_map->first.data());
					int total = 0;
					for (XMLElement* item_ass = docTemp->FirstChildElement("body")->FirstChildElement("item");
						item_ass;
						item_ass = item_ass->NextSiblingElement("item"))
					{
						total++;
						const char* cpathStr = item_ass->Attribute("path");
						const char* crevisionStr = item_ass->Attribute("version");

						XMLElement* item_all = mapXmlDoc->NewElement("item");
						item_all->SetAttribute("path", cpathStr);
						item_all->SetAttribute("version", crevisionStr);
						XMLNode* element_first = mapXmlDoc->FirstChild();
						element_first->InsertEndChild(item_all);
					}
					//printf("end %s\n\n", it_map->first.data());
				}
			}
			for (it = mapVersion.begin(); it != mapVersion.end(); it++)
			{
				docTemp = it->second;
				string  str = dir + "version\\" + it->first;
				str += ".xml";
				XMLError error = docTemp->SaveFile(str.data());
				if (error != XML_SUCCESS)
				{
					printf("save %s xml file error code = %d!!\n", str.data(), error);
				}
				else
				{
					//	printf("save %s xml file success!!\n", str.data());
				}
				delete docTemp;
			}
			string  str = dir + "version\\all_version.xml";
			if (all->SaveFile(str.data()) != XML_SUCCESS)
			{
				printf("save version\\all_version.xml xml file error!!\n");
			}
			delete all;
			printf("all finished.\n\n");
			Exit();
		}
	}
	catch (exception e)
	{
		printf("Exception: %s",e.what());
		Exit();
	}
	return 0;
}