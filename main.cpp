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

vector<string> buf;//��ǰ��¼���ı�
vector<vector<string>> bf;//��¼����ǰ�Ķ���ı������ڳ����ͷ�����
int co = 0;  //��¼��ǰ�ı�������������Ϊ0
int pre_x = 0;  //��ת֮ǰ������ĵ��еĺ�����
int pre_y = 0;  //��ת֮ǰ������ĵ��е�������

//����õף�������ʾָ�״̬
void bottom_cursor() {
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO scr;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼������ */
	GetConsoleScreenBufferInfo(hOut, &scr);
	if (scr.dwCursorPosition.Y != scr.dwMaximumWindowSize.Y - 20) {
		pre_x = scr.dwCursorPosition.X;
		pre_y = scr.dwCursorPosition.Y;
	}
	COORD pos = { 0, scr.dwMaximumWindowSize.Y - 20 };
	SetConsoleCursorPosition(hOut, pos);
}

//����λ�������ַ��������������ָ�״̬����ʾ
void back_cursor() {
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO scr;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼������ */
	GetConsoleScreenBufferInfo(hOut, &scr);
	COORD pos = { pre_x, pre_y };
	SetConsoleCursorPosition(hOut, pos);
}

//normalģʽ�¹����ƶ�
void move_cursor(char ch) {
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO scr;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼������ */
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

//normalģʽ�µ�ɾ����ɾ���������λ�õ��ַ�
void delete_cursor() {
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO scr;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼������ */
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

//���ı���Ѱ��ָ���ؼ��֣��ҵ�����������λ�ã���֮���ƶ�
void search_x() {
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO scr;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼������ */
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
	if (reco == 0)cout << "δ�ҵ�";
}

//����������������һ���Ĳ����ɾ��
void paste() {
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO scr;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼������ */
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

//������ģ��
void antipaste() {
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO scr;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼������ */
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

//�˳�����
int exit_editor() {
	exit(0);
	return -1;
}


//�ı��༭ģʽ
void text_editor_mode() {
	back_cursor();
	//����༭�ı�ģʽ
	while (1) {
		int input_ch = _getch();
		//insertģʽ�µĹ���ƶ�
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
				hOut = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼������ */
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

//�ı�����ģʽ�������ڳ���ǰĿ¼
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
		cout << "   " << name + ".txt" + "�������";
		back_cursor();
	}
}

//��ȡ�ļ�ģʽ
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
	infile.open(file.data());  //���ļ����������ļ���������
	if (!infile.is_open()) {
		cout << "�޷��ҵ�";
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
	infile.close();  //�ر��ļ�������
}


int main() {

	int exit_ret = 0;//flag���ڱ���˳������Ƿ����
	char back_space[30];
	bf.push_back(buf);
	co++;//���ı���ʼ״̬��Ϊ��ֵ����bf
	memset(back_space, ' ', 29);
	back_space[29] = '\0';//�ո��ı�����������
	char pre_c = '!';//pre_c��һ���õ�ָ���ֵ

	//����ָ��
	while (1) {
		char fs = _getch();
		//����ƶ�
		if (fs == 'h' || fs == 'j' || fs == 'k' || fs == 'l')
			move_cursor(fs);
		//ɾ������
		else if (fs == 'x') {
			delete_cursor();
		}
		//��������
		else if (fs == 'u') {
			paste();
		}
		//����������
		else if (fs == 'v') {
			antipaste();
		}
		else {
			string ch;
			//�ж�oָ����Ƿ���openָ��
			if (fs == 'o')
				pre_c = fs;
			if (fs == 'p' && pre_c == 'o')
				pre_c = fs;
			if (fs == 'e' && pre_c == 'p')
				pre_c = fs;
			if (fs == 'n' && pre_c == 'e')
				pre_c = fs;
			//�ı�����ģʽ
			if (fs == 'i') {
				bottom_cursor();
				cout << back_space;
				bottom_cursor();
				cout << fs;
				text_editor_mode();
			}
			//�ı�����ģʽ
			else if (fs == 47) {
				bottom_cursor();
				cout << back_space;
				search_x();
			}
			//�ı��˳�ģʽ
			else if (fs == 'q') {
				bottom_cursor();
				cout << back_space;
				bottom_cursor();
				cout << fs;
				exit_ret = exit_editor();
			}
			//�ı�����ģʽ
			else if (fs == 'w') {
				bottom_cursor();
				cout << back_space;
				bottom_cursor();
				cout << fs;
				save_text_mode();
			}
			//oָ����ж�
			else if (fs == 'o') {
				bottom_cursor();
				cout << back_space;
				bottom_cursor();
				cout << fs;
			}
			//�ı���ģʽ
			else if (fs == 32 && pre_c == 'n') {
				cout << fs;
				read_file();
			}
			//�޷�����ָ�������
			else if (fs >= 20 && fs <= 126) {
				if (pre_c != '!' && pre_c != 'n')cout << fs;

			}
		}
		//��������
		if (exit_ret == -1)
			break;
	}
	return 0;
}