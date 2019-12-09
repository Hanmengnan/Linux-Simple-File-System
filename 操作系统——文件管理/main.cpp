#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<string>
#include<cstring>
#include<queue>
using namespace std;

fstream mydisk;
fstream mydata;
int userstate = 0;
int currentcd = 0;
int dataposition = 0;

typedef struct item {
	char name[10];//文件名
	int type;//文件类型（文件/文件夹）
	int size;//文件大小
	int code;//文件的状态码
	int  datab;//文件内容开始指针
	int datae;//文件内容结束指针
	int father;//父文件夹指针
	int son;//子目录指针
	int brother;//同级指针
}item;

void init() {

	item file1;//1
	strcpy(file1.name, "bin");
	file1.brother = -2;
	file1.son = 0;
	file1.father = -1;
	file1.datab = 0;
	file1.datae = 0;
	file1.size = 1;
	file1.code = 1;
	file1.type = 0;
	int position_file1 = mydisk.tellp();
	mydisk.write((char*)&file1, sizeof(file1));

	item file2;//1.1
	int position_file2 = mydisk.tellp();
	strcpy(file2.name, "bin.txt");
	file2.father = position_file1;
	file2.brother = -2;
	file2.son = -1 * position_file2;
	file2.datab = 0;
	file2.datae = 0;
	file2.size = 1;
	file2.code = 0;
	file2.type = 1;

	mydisk.write((char*)&file2, sizeof(file2));



	item file3;//1.2
	int position_file3 = mydisk.tellp();
	strcpy(file3.name, "bin2.txt");
	file3.father = position_file1;
	file3.brother = -2;
	file3.son = -1 * position_file3;
	file3.datab = 0;
	file3.datae = 0;
	file3.size = 1;
	file3.code = 1;
	file3.type = 1;

	mydisk.write((char*)&file3, sizeof(file3));

	item file4;//2
	int position_file4 = mydisk.tellp();
	strcpy(file4.name, "help.txt");
	file4.father = -1;
	file4.brother = -2;
	file4.son = -1 * position_file4;
	file4.datab = 0;
	file4.datae = 0;
	file4.size = 1;
	file4.code = 0;
	file4.type = 1;

	mydisk.write((char*)&file4, sizeof(file4));


	mydisk.seekp(position_file2);
	file2.brother = position_file3;
	mydisk.write((char*)&file2, sizeof(file2));

	mydisk.seekp(position_file1);
	file1.son = position_file2;
	file1.brother = position_file4;
	mydisk.write((char*)&file1, sizeof(file1));

	return;
}

int find_writeposition() {
	int p1, p2;
	item * tempfile = (item*)malloc(sizeof(item));
	mydisk.seekg(0, ios::beg);
	mydisk.read((char*)tempfile, sizeof(item));
	while (true)
	{
		p1 = mydisk.tellg();
		mydisk.read((char*)tempfile, sizeof(item));
		p2 = mydisk.tellg();
		if (-1 == p2)
		{
			free(tempfile);
			mydisk.clear();
			return p1;
		}
	}

	return mydisk.tellg();
}
void dir() {

	cout << ".." << endl;

	if (currentcd < 0) {
		if (currentcd == -1) {
			currentcd = -1*find_writeposition();
		}
		return;
	}
	mydisk.seekg(currentcd, ios::beg);
	mydisk.seekp(currentcd, ios::beg);

	printf("%-10s%-10s%-10s%-10s\n", "Name", "Size","Type","State");
	int a = mydisk.tellg();
	item *temp = (item*)malloc(sizeof(item));
	if (mydisk.read((char*)temp, sizeof(item)))

		printf("%-10s%-10d%-10d%-10d\n", temp->name, int(temp->size), int(temp->type), int(temp->code));
	while (int(temp->brother)!=-2)
	{
		mydisk.seekg(int(temp->brother), ios::beg);
		mydisk.read((char*)temp, sizeof(item));
		printf("%-10s%-10d%-10d%-10d\n", temp->name, int(temp->size), int(temp->type), int(temp->code));
	}
	free(temp);
	return;
}

int search(string file) {
	int posison;
	queue<item> filetree;

	mydisk.seekp(currentcd, ios::beg);
	mydisk.seekg(currentcd, ios::beg);

	item * tempfile = (item*)malloc(sizeof(item));

	mydisk.read((char*)tempfile, sizeof(item));

	filetree.push(*tempfile);

	mydisk.seekp(currentcd, ios::beg);
	mydisk.seekg(currentcd, ios::beg);

	int headposition = currentcd;
	while (!filetree.empty())
	{
		item filehead = filetree.front();

		if (filehead.name == file) {
			return headposition;

		}
		if (filehead.brother != 0) {
			headposition = filehead.brother;
			mydisk.seekg(filehead.brother, ios::beg);
			mydisk.read((char*)tempfile, sizeof(item));
			filetree.push(*tempfile);
		}

		filetree.pop();

	}
	return -1;
}

void fileclose(string filename) {
	int position = search(filename);
	if (position == -1) {
		cout << "Filename Error" << endl;
		return;
	}
	mydisk.seekg(position, ios::beg);
	item * targetfile = (item*)malloc(sizeof(item));

	mydisk.read((char*)targetfile, sizeof(item));
	targetfile->code = 0;

	mydisk.seekp(position, ios::beg);
	mydisk.write((char*)targetfile, sizeof(item));
	cout << "File Close!" << endl;

}

void fileopen(string filename) {
	int position = search(filename);
	if (position == -1) {
		cout << "Filename Error" << endl;
		return;
	}
	mydisk.seekg(position, ios::beg);
	item * targetfile = (item*)malloc(sizeof(item));

	mydisk.read((char*)targetfile, sizeof(item));
	targetfile->code = 1;

	mydisk.seekp(position, ios::beg);
	mydisk.write((char*)targetfile, sizeof(item));
	cout << "File Open!" << endl;

}







void writef(string data, string filename) {
	int position = search(filename);
	if (position == -1) {
		cout << "Not exist!" << endl;
		return;
	}
	item*tempfile = (item*)malloc(sizeof(item));
	mydisk.seekg(position, ios::beg);
	mydisk.read((char*)tempfile, sizeof(item));

	if (tempfile->type == 0) {
		cout << "Filetype Error!" << endl;
		return;
	}
	if (tempfile->code == 0) {
		cout << "Filestate Error!" << endl;
		return;
	}

	tempfile->datab = dataposition;
	tempfile->datae = dataposition + data.length();
	tempfile->size = data.length()+1;

	mydisk.seekp(position, ios::beg);
	mydisk.write((char *)tempfile, sizeof(item));

	position = tempfile->father;
	while (position!=-1)//***************
	{
		mydisk.seekg(position, ios::beg);
		mydisk.read((char*)tempfile, sizeof(item));

		tempfile->size+=data.length()+1;
		mydisk.seekp(position, ios::beg);
		mydisk.write((char *)tempfile, sizeof(item));
		position = tempfile->father;
	}

	mydata.seekp(dataposition, ios::beg);
	mydata.write(data.c_str(), data.length() + 1);
	dataposition += data.length() + 1;
	free(tempfile);
}



void readf(string filename) {

	int position = search(filename);
	if (position == -1) {
		cout << "Not exist!" << endl;
		return;
	}
	item*tempfile = (item*)malloc(sizeof(item));

	string data;
	mydisk.seekg(position, ios::beg);
	mydisk.read((char*)tempfile, sizeof(item));

	if (tempfile->type == 0) {
		cout << "Filetype Error!" << endl;
		return;
	}
	if (tempfile->code == 0) {
		cout << "Filestate Error!" << endl;
		return;
	}
	mydata.seekg(tempfile->datab, ios::beg);
	mydata.read((char*)data.c_str(), tempfile->datae - tempfile->datab + 1);
	printf("\033[2J");
	printf("%s\n", data.c_str());
	free(tempfile);
}


void createfile(string filename) {

	item * tempfile = new(item);
	item * tempfatherfile = new(item);
	strcpy(tempfile->name, filename.c_str());

	tempfile->father = -1;
	tempfile->brother = -2;
	tempfile->son = 0;
	tempfile->datab = 0;
	tempfile->datae = 0;
	tempfile->size = 1;
	tempfile->code = 0;


	if (filename.find(".") == string::npos)
		//文件类型判断
		tempfile->type = 0;
	else
		tempfile->type = 1;


	int lastposition = find_writeposition();//因为为新建文件 ，需要寻找写入位置


	if (currentcd < 0) {
		if (currentcd != -1) {
			mydisk.seekg(-1 * currentcd, ios::beg);
			mydisk.seekp(-1 * currentcd, ios::beg);
			mydisk.read((char*)tempfatherfile, sizeof(item));

			tempfile->father = -1 * currentcd;
			tempfile->brother = -2;
			tempfile->son = -1 * lastposition;

			mydisk.seekp(lastposition, ios::beg);
			mydisk.write((char*)tempfile, sizeof(item));

			tempfatherfile->son = lastposition;
			mydisk.seekp(-1 * currentcd, ios::beg);
			mydisk.write((char*)tempfatherfile, sizeof(item));
			currentcd = lastposition;
		}
		else {
			currentcd = lastposition;
			mydisk.seekg( currentcd, ios::beg);
			mydisk.seekp( currentcd, ios::beg);

			mydisk.write((char*)tempfile, sizeof(item));
			currentcd = lastposition;
		}
		
	}
	else if (currentcd == 0) {
		mydisk.seekg(currentcd, ios::beg);
		mydisk.seekp(currentcd, ios::beg);

		mydisk.read((char*)tempfatherfile, sizeof(item));

		tempfile->father = -1;
		tempfile->brother = tempfatherfile->brother;;
		tempfile->son = -1 * lastposition;

		mydisk.seekp(lastposition, ios::beg);
		mydisk.write((char*)tempfile, sizeof(item));

		tempfatherfile->brother = lastposition;
		mydisk.seekp(currentcd, ios::beg);
		mydisk.write((char*)tempfatherfile, sizeof(item));
		currentcd = 0;
	}
	else {
		mydisk.seekg(currentcd, ios::beg);
		mydisk.seekp(currentcd, ios::beg);

		mydisk.read((char*)tempfatherfile, sizeof(item));

		tempfile->father = tempfatherfile->father;
		tempfile->brother = tempfatherfile->brother;
		tempfile->son = -1 * lastposition;

		mydisk.seekp(lastposition, ios::beg);
		mydisk.write((char*)tempfile, sizeof(item));

		tempfatherfile->brother = lastposition;
		mydisk.seekp(currentcd, ios::beg);
		mydisk.write((char*)tempfatherfile, sizeof(item));
	}

	free(tempfile);
	free(tempfatherfile);

}


void removefile(string filename) {

	int position = search(filename);
	item * tempfile = new(item);
	item * tempfatherfile = new(item);

	mydisk.seekg(position, ios::beg);
	mydisk.read((char*)tempfile, sizeof(item));//当前节点

	if (position == currentcd) {//是否为第一层节点
		if (tempfile->father == -1) {			
		}
		else {
			mydisk.seekg(tempfile->father, ios::beg);
			mydisk.read((char*)tempfatherfile, sizeof(item));

			tempfatherfile->son = tempfile->brother;
			mydisk.seekp(tempfile->father, ios::beg);
			mydisk.write((char*)tempfatherfile, sizeof(item));
			
		}
		currentcd = tempfile->brother;
	}

	else {
		mydisk.seekg(currentcd, ios::beg);
		mydisk.read((char*)tempfatherfile, sizeof(item));

		while (tempfatherfile->brother != position)
		{
			mydisk.seekg(tempfatherfile->brother, ios::beg);
			mydisk.read((char*)tempfatherfile, sizeof(item));
		}

		tempfatherfile->brother = tempfile->brother;

		int p = search(tempfatherfile->name);

		mydisk.seekp(p, ios::beg);
		mydisk.write((char*)tempfatherfile, sizeof(item));
	}
	free(tempfile);
	free(tempfatherfile);
}

void cd(string filepath) {

	item * tempfile = (item*)malloc(sizeof(item));
	if (filepath != "..") {
		if (currentcd < 0)
		{
			if (currentcd == -1) currentcd = find_writeposition();
			else currentcd *= -1;
		}
		int position = search(filepath);

		mydisk.seekg(position, ios::beg);

		mydisk.read((char*)tempfile, sizeof(item));

		if (tempfile->type == 0) {
			currentcd = tempfile->son;
		}
		else
		{
			cout << "Filepath Error!" << endl;
		}
	}
	else {
		if (currentcd < 0)
		{
			mydisk.seekg(-1 * currentcd, ios::beg);
			mydisk.read((char*)tempfile, sizeof(item));

			currentcd = tempfile->father;

			if (currentcd != -1) {//非第一层
				mydisk.seekg(currentcd, ios::beg);
				mydisk.read((char*)tempfile, sizeof(item));
				currentcd = tempfile->son;
			}
			else
			{
				currentcd = 0;
			}
		}
		else if (currentcd > 0) {
			mydisk.seekg(currentcd, ios::beg);
			mydisk.read((char*)tempfile, sizeof(item));

			currentcd = tempfile->father;
			mydisk.seekg(currentcd, ios::beg);
			mydisk.read((char*)tempfile, sizeof(item));

			currentcd = tempfile->father;

			if (currentcd != -1) {
				mydisk.seekg(currentcd, ios::beg);
				mydisk.read((char*)tempfile, sizeof(item));
				currentcd = tempfile->son;
			}
			else {
				currentcd = 0;
			}
		}


	}
	return;
}

void userlogin() {
	fstream user;
	user.open("./user", ios::binary | ios::in | ios::out);

	string inputname;
	cout << "login as: ";
	cin >> inputname;
	string inputpassword;
	cout << "password: ";
	cin >> inputpassword;
	string userdata;
	getline(user, userdata);
	int index1 = userdata.find("[");
	int index2 = userdata.find("]");
	string username(userdata.begin() + index1 + 1, userdata.begin() + index2);
	int index3 = userdata.find("[", index2);
	string password(userdata.begin() + index3 + 1, userdata.end() - 1);
	if (inputname == username && inputpassword == password) {
		cout << "Welcome " << endl;
		userstate = 1;
	}
	else {
		cout << "Access Denied" << endl;
	}
	user.close();
}

void userlogout() {
	userstate = 0;
	cout << "GoodBye\n";
}

int main() {

	mydisk.open("./disk", ios::binary | ios::in | ios::out);
	//init();


	while (1)
	{
		cout << "> ";
		string cmd;
		cin >> cmd;
		if (cmd == "dir") {
			if (userstate == 0) {
				printf("Userstate Error!\n");
			}
			else {
				dir();
				cout << endl;
			}
		}
		else if (cmd == "login") {
			userlogin();
		}
		else if (cmd == "logout") {
			userlogout();
		}
		else if (cmd == "closef") {
			if (userstate == 0) {
				printf("Userstate Error!\n");
			}
			else {
				string filename;
				cout << "Input Filename:";
				cin >> filename;
				fileclose(filename);

			}
		}
		else if (cmd == "openf") {
			if (userstate == 0) {
				printf("Userstate Error!\n");
			}
			else {
				string filename;
				cout << "Input Filename:";
				cin >> filename;
				fileopen(filename);

			}
		}
		else if (cmd == "mkdir") {
			if (userstate == 0) {
				printf("Userstate Error!\n");
			}
			else {
				string filename;
				cout << "Input Filename:";
				cin >> filename;
				createfile(filename);

			}
		}
		else if (cmd == "rmdir") {
			if (userstate == 0) {
				printf("Userstate Error!\n");
			}
			else {
				string filename;
				cout << "Input Filename:";
				cin >> filename;
				removefile(filename);

			}
		}
		else if (cmd == "writef") {
			if (userstate == 0) {
				printf("Userstate Error!\n");
			}
			else {

				mydata.open("./data", ios::binary | ios::app | ios::out);
				mydata.seekp(0, ios::end);
				dataposition = int(mydata.tellp());
				string data, filename;
				cout << "Filename:";
				cin >> filename;
				cout << "Filedata:";
				cin >> data;
				writef(data, filename);
				mydata.close();

			}


		}
		else if (cmd == "readf") {
			if (userstate == 0) {
				printf("Userstate Error!\n");
			}
			else {
				mydata.open("./data", ios::binary | ios::in);
				mydata.seekg(0, ios::end);
				dataposition = int(mydata.tellg());
				string filename;
				cout << "Filename:";
				cin >> filename;
				readf(filename);
				mydata.close();
			}

		}
		else if (cmd == "exist") break;
		else if (cmd == "cd") {
			if (userstate == 0) {
				printf("Userstate Error!\n");
			}
			else {
				string filename;
				cout << "Input Filename:";
				cin >> filename;
				cd(filename);

			}
		}

	}

	mydisk.close();
	return 0;
}