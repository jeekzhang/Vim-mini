#include <stdio.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cstdio>
#include <Windows.h>
#include <fcntl.h>
#include <string>
#include <cstdlib>
#include <vector>
using namespace std;

vector<string> buf;//当前记录的文本
vector<vector<string>> bf;//记录操作前的多个文本，用于撤销和反撤销
int co = 0;  //记录当前文本操作数，空则为0
int pre_x = 0;  //跳转之前光标在文档中的横坐标
int pre_y = 0;  //跳转之前光标在文档中的纵坐标

//光标置底，用于显示指令及状态
void bottom_cursor() {
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO scr;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出句柄 */
	GetConsoleScreenBufferInfo(hOut, &scr);
	if (scr.dwCursorPosition.Y != scr.dwMaximumWindowSize.Y - 20) {
		pre_x = scr.dwCursorPosition.X;
		pre_y = scr.dwCursorPosition.Y;
	}
	COORD pos = { 0, scr.dwMaximumWindowSize.Y - 20 };
	SetConsoleCursorPosition(hOut, pos);
}

//光标归位，用于字符的输出，区别于指令及状态的显示
void back_cursor() {
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO scr;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出句柄 */
	GetConsoleScreenBufferInfo(hOut, &scr);
	COORD pos = { pre_x, pre_y };
	SetConsoleCursorPosition(hOut, pos);
}

//normal模式下光标的移动
void move_cursor(char ch) {
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO scr;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出句柄 */
	GetConsoleScreenBufferInfo(hOut, &scr);
	COORD pos = { 0, 0 };
	short int x1 = scr.dwCursorPosition.X - 1;
	short int x2 = scr.dwCursorPosition.X + 1;
	short int y1 = scr.dwCursorPosition.Y - 1;
	short int y2 = scr.dwCursorPosition.Y + 1;
	if (ch == 'h')
		pos = { x1, scr.dwCursorPosition.Y };
	if (ch == 'j')
		pos = { scr.dwCursorPosition.X, y2 };
	if (ch == 'k')
		pos = { scr.dwCursorPosition.X, y1 };
	if (ch == 'l')
		pos = { x2, scr.dwCursorPosition.Y };
	short int a = max(buf.size() - 1, 0);
	if (pos.Y > 0)
		pos.Y = min(pos.Y, a);
	else
		pos.Y = 0;
	short int b = 0;
	int p = buf[pos.Y].size();
	if (a > 0)
		b = max(p - 1, 0);
	if (pos.X > 0)
		pos.X = min(pos.X, b);
	else
		pos.X = 0;
	SetConsoleCursorPosition(hOut, pos);
}

//normal模式下的删除，删除光标所在位置的字符
void delete_cursor() {
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO scr;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出句柄 */
	GetConsoleScreenBufferInfo(hOut, &scr);
	int x = scr.dwCursorPosition.X;
	int y = scr.dwCursorPosition.Y;
	bf.push_back(buf);
	co++;
	buf[scr.dwCursorPosition.Y].erase(scr.dwCursorPosition.X, 1);
	system("cls");
	for (int i = 0; i < buf.size(); i++) {
		for (int j = 0; j < buf[i].size(); j++) cout << buf[i][j];
		if (i + 1 < buf.size())
			cout << endl;
	}
	COORD pos = { x, y };
	SetConsoleCursorPosition(hOut, pos);
}

//在文本下寻找指定关键字，找到则光标移至其位置，反之则不移动
void search_x() {
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO scr;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出句柄 */
	GetConsoleScreenBufferInfo(hOut, &scr);
	bottom_cursor();
	cout << '/';
	string pattern;
	while (1) {
		char sc = _getch();
		if (sc == 13)
			break;
		cout << sc;
		pattern.push_back(sc);
	}
	int reco = 0;
	for (int i = 0; i < buf.size(); i++) {
		if (buf[i].find(pattern) != string::npos) {
			COORD pos = { buf[i].find(pattern), i };
			reco = 1;
			SetConsoleCursorPosition(hOut, pos);
			break;
		}
	}
	if (reco == 0)cout << "未找到";
}

//撤销操作，撤销上一步的插入和删除
void paste() {
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO scr;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出句柄 */
	GetConsoleScreenBufferInfo(hOut, &scr);
	int x = scr.dwCursorPosition.X;
	int y = scr.dwCursorPosition.Y;
	if (co - 1 >= 0) {
		buf = bf[co - 1];
		co--;
	}
	system("cls");
	for (int i = 0; i < buf.size(); i++) {
		for (int j = 0; j < buf[i].size(); j++) cout << buf[i][j];
		if (i + 1 < buf.size())
			cout << endl;
	}
	COORD pos = { x, y };
	if (co > 1)
		SetConsoleCursorPosition(hOut, pos);
}

//反撤销模块
void antipaste() {
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO scr;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出句柄 */
	GetConsoleScreenBufferInfo(hOut, &scr);
	int x = scr.dwCursorPosition.X;
	int y = scr.dwCursorPosition.Y;
	if (co + 1 < bf.size()) {
		buf = bf[co + 1];
		co++;
	}
	system("cls");
	for (int i = 0; i < buf.size(); i++) {
		for (int j = 0; j < buf[i].size(); j++) cout << buf[i][j];
		if (i + 1 < buf.size())
			cout << endl;
	}
	COORD pos = { x, y };
	if (co > 1)
		SetConsoleCursorPosition(hOut, pos);
}

//退出程序
int exit_editor() {
	exit(0);
	return -1;
}


//文本编辑模式
void text_editor_mode() {
	back_cursor();
	//进入编辑文本模式
	while (1) {
		int input_ch = _getch();
		//insert模式下的光标移动
		if (input_ch == 224) {
			int i_ch = _getch();
			HANDLE hOut;
			CONSOLE_SCREEN_BUFFER_INFO scr;
			hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleScreenBufferInfo(hOut, &scr);
			COORD pos = { 0, 0 };
			short int x1 = scr.dwCursorPosition.X - 1;
			short int x2 = scr.dwCursorPosition.X + 1;
			short int y1 = scr.dwCursorPosition.Y - 1;
			short int y2 = scr.dwCursorPosition.Y + 1;
			if (i_ch == 75)
				pos = { x1, scr.dwCursorPosition.Y };
			if (i_ch == 80)
				pos = { scr.dwCursorPosition.X, y2 };
			if (i_ch == 72)
				pos = { scr.dwCursorPosition.X, y1 };
			if (i_ch == 77)
				pos = { x2, scr.dwCursorPosition.Y };
			short int a = max(buf.size() - 1, 0);
			if (pos.Y > 0)
				pos.Y = min(pos.Y, a);
			else
				pos.Y = 0;
			short int b = 0;
			int p = buf[pos.Y].size();
			if (a > 0)
				b = max(p - 1, 0);
			if (pos.X > 0)
				pos.X = min(pos.X, b);
			else
				pos.X = 0;
			SetConsoleCursorPosition(hOut, pos);
		}
		else
		{
			if (input_ch == 27) {
				bf.push_back(buf);
				co++;
				bottom_cursor();
				cout << 'n';
				back_cursor();
				return;
			}
			else {
				HANDLE hOut;
				CONSOLE_SCREEN_BUFFER_INFO scr;
				hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出句柄 */
				GetConsoleScreenBufferInfo(hOut, &scr);
				int x = scr.dwCursorPosition.X;
				int y = scr.dwCursorPosition.Y;

				if (input_ch == 13) {
					string sc = buf[y].substr(x);
					buf[y] = buf[y].substr(0, x);
					if (y < buf.size())buf.insert(buf.begin() + y + 1, 1, sc);
					else buf.push_back(sc);
					x = 0;
					y++;
				}
				else {
					if (y < buf.size()) {
						if (x < buf[y].size())buf[y].insert(x, 1, input_ch);
						else buf[y].push_back(input_ch);
					}
					else {
						string tc;
						tc.push_back(input_ch);
						buf.push_back(tc);
					}
					x++;
				}

				system("cls");
				bottom_cursor();
				cout << "i";
				back_cursor();
				for (int i = 0; i < buf.size(); i++) {
					for (int j = 0; j < buf[i].size(); j++) cout << buf[i][j];
					if (i + 1 < buf.size())
						cout << endl;
				}
				COORD pos = { x, y };
				SetConsoleCursorPosition(hOut, pos);
			}
		}
	}
}

//文本保存模式，保存在程序当前目录
void save_text_mode() {
	string name;
	if (_getch() == 32) {
		cout << ' ';
		while (1) {
			char ch = _getch();
			if (ch == 13 || ch == 32)break;
			name.push_back(ch);
			cout << ch;
		}
	}
	ofstream out(name + ".txt");
	if (out.is_open()) {
		ofstream fileout(name + ".txt", ios::trunc);
		for (int i = 0; i < buf.size(); i++) {
			for (int j = 0; j < buf[i].size(); j++) out << buf[i][j];
			if (i + 1 < buf.size())
				out << endl;
		}
		out.close();
		cout << "   " << name + ".txt" + "保存完成";
		back_cursor();
	}
}

//读取文件模式
void read_file() {
	system("cls");
	buf.clear();
	bottom_cursor();
	cout << "open ";
	string file;
	while (1) {
		char as = _getch();
		if (as == 13)
			break;
		cout << as;
		file.push_back(as);
	}
	file += ".txt";
	ifstream infile;
	infile.open(file.data());  //将文件流对象与文件连接起来
	if (!infile.is_open()) {
		cout << "无法找到";
		return;
	}
	back_cursor();

	string s;
	int count = 0;
	while (getline(infile, s)) {
		if (count > 0)
			cout << endl;
		count++;
		buf.push_back(s);
		cout << s;
	}
	infile.close();  //关闭文件输入流
}


int main() {

	int exit_ret = 0;//flag用于标记退出函数是否操作
	char back_space[30];
	bf.push_back(buf);
	co++;//把文本初始状态作为初值赋给bf
	memset(back_space, ' ', 29);
	back_space[29] = '\0';//空格文本用于行清屏
	char pre_c = '!';//pre_c赋一不用的指令初值

	//读入指令
	while (1) {
		char fs = _getch();
		//光标移动
		if (fs == 'h' || fs == 'j' || fs == 'k' || fs == 'l')
			move_cursor(fs);
		//删除操作
		else if (fs == 'x') {
			delete_cursor();
		}
		//撤销操作
		else if (fs == 'u') {
			paste();
		}
		//反撤销操作
		else if (fs == 'v') {
			antipaste();
		}
		else {
			string ch;
			//判断o指令后是否是open指令
			if (fs == 'o')
				pre_c = fs;
			if (fs == 'p' && pre_c == 'o')
				pre_c = fs;
			if (fs == 'e' && pre_c == 'p')
				pre_c = fs;
			if (fs == 'n' && pre_c == 'e')
				pre_c = fs;
			//文本键入模式
			if (fs == 'i') {
				bottom_cursor();
				cout << back_space;
				bottom_cursor();
				cout << fs;
				text_editor_mode();
			}
			//文本搜索模式
			else if (fs == 47) {
				bottom_cursor();
				cout << back_space;
				search_x();
			}
			//文本退出模式
			else if (fs == 'q') {
				bottom_cursor();
				cout << back_space;
				bottom_cursor();
				cout << fs;
				exit_ret = exit_editor();
			}
			//文本保存模式
			else if (fs == 'w') {
				bottom_cursor();
				cout << back_space;
				bottom_cursor();
				cout << fs;
				save_text_mode();
			}
			//o指令单独判断
			else if (fs == 'o') {
				bottom_cursor();
				cout << back_space;
				bottom_cursor();
				cout << fs;
			}
			//文本打开模式
			else if (fs == 32 && pre_c == 'n') {
				cout << fs;
				read_file();
			}
			//无法读入指令，不回显
			else if (fs >= 20 && fs <= 126) {
				if (pre_c != '!' && pre_c != 'n')cout << fs;

			}
		}
		//结束程序
		if (exit_ret == -1)
			break;
	}
	return 0;
}