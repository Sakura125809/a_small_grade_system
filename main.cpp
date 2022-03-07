#include<iostream>
#include<fstream>
#include<stdio.h>
#include<conio.h>

#include<unordered_map>
#include<string>
#include<vector>

#include<windows.h>
#include <mmsystem.h>
#include<dsound.h>
#pragma comment(lib, "Winmm.lib")

using namespace std;

struct Student{
	string _id; //学号
	string _name; //姓名
	int _chinese; //语文成绩
	int _math; //数学成绩
	int _english; //英语成绩
	int _sum; //成绩总分
	int _level; //成绩排名
	Student * next; //链表结点的后向指针
	Student * prev; //链表结点的前向指针

	Student(string id, string name, int chinese, int math,
	int english, int sum, int level, Student * n, Student * p):
		_id(id), _name(name), _chinese(chinese), _math(math), _english(english),
		_sum(sum), _level(level), next(n), prev(p){} //构造函数

	~Student(){} //析构函数
};

class List{ //双向链表
	public:
		Student * head; //链表头
		Student * tail; //链表尾
		int length; //链表长度
		unordered_map<string, Student *> dict; // 哈希表

		List(){ //构造函数
			this->head = new Student("null", "null", 0, 0, 0, 0, 0, nullptr, nullptr);
			this->tail = new Student("null", "null", 0, 0, 0, 0, 0, nullptr, nullptr);
			this->head->next = this->tail;
			this->tail->prev = this->head;
			this->length = 0;
		}

		~List(){ //析构函数
			delete head;
			delete tail;
		}

		void insert(Student * node){ //链表中插入节点，使用链表插入排序
			this->dict[node->_id] = node; //哈希表中存储学号与结点，姓名与结点的映射信息，便于查找
			this->dict[node->_name] = node;
			if(this->length == 0){ //如果此时链表为空
				this->head->next = node;
				node->prev = this->head;
				this->tail->prev = node;
				node->next = this->tail;
				node->_level = 1;
			}
			else{
				Student * _left = this->head;//安排一个动指针遍历链表
				while(_left->next != this->tail && node->_sum <= _left->next->_sum){
					//查找合适的插入点
					_left = _left->next;
				}
				Student * _right = _left->next;
				_left->next = node;//插入结点
				node->prev = _left;
				_right->prev = node;
				node->next = _right;
				node->_level = (_left->_level) + 1;//赋予新插入数据的排名
				while(_right != this->tail){//修改被插入数据后的所有数据排名
					_right->_level++;
					_right = _right->next;
				}
			}
			this->length++;
		}

		void del(string str){ //依据学号或姓名删除学生对象
			if(this->dict.find(str) == this->dict.end()){
				puts("查无此人，请重新核对学号或姓名！,请按任意键返回");
				return;
			}
			Student * node = this->dict[str];
			Student * _left = node->prev;
			Student * _right = node->next;
			_left->next = _right; //链表中删除对象
			_right->prev = _left;
			dict.erase(node->_id); //哈希表中删除对象
			dict.erase(node->_name);
			delete node; //释放内存
			while(_right != this->tail){//修改被插入数据后的所有数据排名
				_right->_level--;
				_right = _right->next;
			}
			this->length--;
		}

		void modify(string str, string id, string name, int chinese, int math, int english){
			//依据学号或者姓名，修改对象属性
			if(this->dict.find(str) == this->dict.end()){
				puts("查无此人，请重新核对学号或姓名！请按任意键返回");
				return;
			}
			this->del(str);//删除原有对象
			Student * node = new Student(id, name, chinese, math, english,
			chinese + math + english, 0, nullptr, nullptr);
			this->insert(node);//插入新的对象
		}

		void select(string str){
			//依据学号或者姓名, 查询并打印对象信息
			if(this->dict.find(str) == this->dict.end()){
				puts("查无此人，请重新核对学号或姓名！,请按任意键返回");
				return;
			}
			Student * node = this->dict[str];
			cout << "学号：" << node->_id << endl;
			cout << "姓名：" << node->_name << endl;
			cout << "语文：" << node->_chinese << endl;
			cout << "数学：" << node->_math << endl;
			cout << "英语：" << node->_english << endl;
			cout << "总分：" << node->_sum << endl;
			cout << "排名：" << node->_level << endl;
		}

		void show_all(void){
			//打印所有的成绩信息
			if(this->length == 0){
				puts("当前数据库为空！请按任意键返回");
				return;
			}
			printf("%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n","学号", "姓名", "语文", "数学", "英语", "总分", "名次");
			Student * p = this->head->next;
			while(p != this->tail){
				cout.setf(std::ios::left);
				cout.width(15);
				cout << p->_id;
				cout.width(15);
				cout << p->_name;
				printf("%-15d%-15d%-15d%-15d%-15d\n", p->_chinese, p->_math, p->_english, p->_sum, p->_level);

				p = p->next;
			}
		}
};

void showmenu(void);//显示菜单
void file_analy(const string& file_name, List * table);//解析本地文档，并将内容解析为链表
void Stringsplit(const string& str, const char split, vector<string>& res);//实现字符串分割
void file_write(const string& file_name, List * table);//将链表内容写入记事本

void write_in(List * table);//录入成绩，执行功能1
void score_del(List * table);//删除成绩，执行功能2
void modify(List * table);//修改信息，执行功能3
void select(List * table);//查询信息，执行功能4

int main(void){
	int flag;
	string file_name = "file.txt";
	List * table = new List();
	file_analy(file_name, table);
	mciSendString(TEXT("open flower_dance.wma alias mysong"), NULL, 0, NULL);
	//这一行是打开音频，并将其命名为mysong.
	mciSendString(TEXT("play mysong repeat"), NULL, 0, NULL);//打开音乐
	//插入音频
	do{
		showmenu();
		scanf("%d", &flag);//选择操作
		switch (flag) {
			case 1:
				//功能1：录入成绩
				write_in(table);
				break;
			case 2:
				//功能2：删除成绩
				score_del(table);
				break;
			case 3:
				//功能3：修改信息
				modify(table);
				break;
			case 4:
				//功能4：修改信息
				select(table);
				break;
			case 5:
				//功能5：显示所有的成绩信息
				table->show_all();
				break;
			case 6:
				//功能6：保存信息至本地
				file_write(file_name, table);
				break;
			case 0:
				//功能0：保存信息并退出系统
				goto end;
			default:
				//非法输入
				printf("输入错误，请重试！\n");
				getchar();
				break;
		}
		puts("按任意键继续：");
		getch();
	}while(1);

	end:
	file_write(file_name, table);
	mciSendString(TEXT("close mysong"), NULL, 0, NULL);//关闭音乐
	puts("再见！");
	return 0;
}

void showmenu(void){//显示菜单
system("cls");
printf("*----------------------------------------------------*\n");
printf("* 欢迎使用**成绩管理系统 *\n");
printf("*----------------------------------------------------*\n");
printf("* 1： 录入成绩*\n");
printf("* 2： 删除成绩*\n");
printf("* 3： 修改信息*\n");
printf("* 4： 成绩查询*\n");
printf("* 5： 显示所有成绩信息*\n");
printf("* 6： 保存信息至本地*\n");
printf("* 0： 保存信息并退出系统*\n");
printf("*----------------------------------------------------*\n");
printf("选择操作<0-6> \n");
}

void file_analy(const string& file_name, List * table){//解析本地文档，并将内容解析为链表
	ifstream in(file_name);//文件操作
	string s;
	vector<string> arr;//动态数组，储存分割后的字符串
	string id, name;
	int chinese, math, english, sum, level;
	while(getline(in,s)){//逐行读取文档内容
		Stringsplit(s, ' ', arr);//字符串分割
		id = arr[0];
		name = arr[1];
		chinese = stoi(arr[2]);
		math = stoi(arr[3]);
		english = stoi(arr[4]);
		sum = stoi(arr[5]);
		level = stoi(arr[6]);
		Student * node = new Student(id, name, chinese, math, english, sum, level, nullptr, nullptr);
		table->insert(node);//将对象插入链表
		arr.clear();
	}
}

void Stringsplit(const string& str, const char split, vector<string>& res){
	//实现字符串分割,也是一种分类算法的应用场景
	if (str == "")		return;
	//在字符串末尾也加入分隔符，方便截取最后一段
	string strs = str + split;
	size_t pos = strs.find(split);

	// 若找不到内容则字符串搜索函数返回 npos
	while (pos != strs.npos)
	{
		string temp = strs.substr(0, pos);
		res.push_back(temp);
		//去掉已分割的字符串,在剩下的字符串中进行分割
		strs = strs.substr(pos + 1, strs.size());
		pos = strs.find(split);
	}
}

void file_write(const string& file_name, List * table){//将链表内容写入记事本
	puts("信息保存中...");
	ofstream file_writer(file_name, ios_base::out);
	Student * p = table->head->next;
	while(p != table->tail){
		file_writer << p->_id << " " << p->_name << " ";
		file_writer << p->_chinese << " " << p->_math << " ";
		file_writer << p->_english << " " << p->_sum << " ";
		file_writer << p->_level << endl;

		p = p->next;
	}
	file_writer.close();
	puts("保存成功！");
}

void write_in(List * table){//录入成绩，执行功能1
	int n, chinese, math, english;
	string id, name;
	printf("请输入您要录入的人数：");
	scanf("%d", &n);
	printf("请输入全部学生的学号，姓名，语文成绩，数学成绩，英语成绩；\n");
	for(int i = 0; i < n; ++i){
		printf("----第%d个学生----\n", i+1);

		printf("学号:");
		cin >> id;

		printf("姓名：");
		cin >> name;

		printf("语文成绩：");
		scanf("%d", &chinese);

		printf("数学成绩：");
		scanf("%d", &math);

		printf("英语成绩：");
		scanf("%d", &english);

		Student * node = new Student(id, name, chinese, math, english,
					chinese + math + english, 0, nullptr, nullptr);//创建对象

		table->insert(node);//将对象插入链表
	}
}

void score_del(List * table){//删除成绩，执行功能2
	string str;
	printf("请输入你要删除的学生学号或姓名:");
	cin >> str;
	table->del(str);
}

void modify(List * table){//修改信息，执行功能3
	string str, id, name;
	int chinese, math, english;
	printf("请输入你要删除的学生学号或姓名:");
	cin >> str;

	printf("请输入该学生修改后的学号：");
	cin >> id;

	printf("请输入该学生修改后的姓名：");
	cin >> name;

	printf("请输入该学生修改后的语文成绩：");
	scanf("%d", &chinese);

	printf("请输入该学生修改后的数学成绩：");
	scanf("%d", &math);

	printf("请输入该学生修改后的英语成绩：");
	scanf("%d", &english);

	table->modify(str, id, name, chinese, math, english);
}

void select(List * table){//查询信息，执行功能4
	string str;
	printf("请输入你要查询的学生学号或姓名:");
	cin >> str;
	table->select(str);
}
