#include<gl/GLUT.H>
#include<iostream>
#include <iostream>
#include<fstream>
#include<sstream>
#include <vector>
#include <ctime>
#include <map>
#include<algorithm>
#include <set>
#include <ctime>
#include <iostream>
#include<list>
using namespace std;
GLfloat modelview_matrix[16];
const int win_w = 700, win_h = 700;
GLfloat ex[3] = { 0.0, 0.0, 0.0 }; //观察者位置
GLfloat ey[3] = { 1.0, 0.0, 0.0 };
GLfloat ez[3] = { 4.0, 0.0, 0.0 };
GLfloat fx[3] = { 0.0, 0.0, 0.0 }; //观察对象位置
GLfloat fy[3] = { 0.0, 0.0, 0.0 };
GLfloat fz[3] = { 0.0, 0.0, 0.0 };
GLint view = 0;
GLfloat angle = 0.0f;
void reshape_func(int w, int h);
void display_func();
void drawBunny();
void Initial(void);
// 点击事件
void onMouse(int button, int state, int x, int y);
void RotateRect();
void absolute_translate(GLfloat x, GLfloat y, GLfloat z);
void absolute_rotate(GLfloat dgree, GLfloat vecx, GLfloat vecy, GLfloat vecz);
void absolute_scale(GLfloat factor);
void mouse_click_func(int button, int state, int x, int y);
void mouse_move_func(int x, int y);
class Point
{
public:
	int flag;
	int index;
	float _x, _y, _z;
	vector<int>listfacet;
	void addFacet(int i)
	{
		listfacet.push_back(i);
	}
	void getIndex(int index)
	{
		this->index = index;
	}
	Point(float _x, float _y, float _z)
	{
		this->_x = _x;
		this->_y = _y;
		this->_z = _z;
	}
	Point(int index, float _x, float _y, float _z)
	{
		this->index = index;
		this->_x = _x;
		this->_y = _y;
		this->_z = _z;
	}
	Point()
	{}
	// 重载
	bool operator<(const Point& b)const
	{
		//return false;
		return (_x < b._x) || (_x == b._x) && (_y < b._y) || (_x == b._x && _y == b._y && _z < b._z);
		//return true;
		// 相等比较 
	}
	bool isEque(Point point)
	{
		if (point._x == _x && point._y == _y && point._z == _z)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class Facet
{
	// 几点面片
public:int index;
	   // 体积
	   double volume;
	   // 面积
	   double s;
	   // 面片中点的数据
	   Point _vertex_1;
	   Point _vertex_2;
	   Point _vertex_3;
	   double sum;
	   vector<double>list;
	   Facet()
	   {}
	   bool operator<(const Facet& _b) const
	   {
		   if (_b.index < index)
		   {
			   return true;
		   }
		   else
		   {
			   return false;
		   }
	   }


	   // 求体积
	   void toString()
	   {
		   cout << "x= " << _vertex_1.index << " y= " << _vertex_2.index << " z= " << _vertex_3.index << endl;
	   }
};

void ReadPly();
map<int, Point>vecPoint;
vector<Facet>vecFacet;
set<Point>_listPoint;
Point getPoint;
void GetFieldcirculation(int index, int nField);
void onMouse(int button, int state, int x, int y);
void RotateRect();
int main(int argc, char*argv[])
{
	std::cout << "Hello World!\n" << endl;;
	clock_t startTime, endTime;
	startTime = clock();//计时开始
	ReadPly();
	endTime = clock();//计时结束
	GetFieldcirculation(3, 5);

	cout <<"总共多少个点: "<< _listPoint.size() << endl;
	cout << "The run time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

	// 画图
	glutInit(&argc, argv);
	// 设置初始显示模式，指定RGB颜色模式以及指定双缓存窗口
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("bunny");
	// 显示图像
	glutDisplayFunc(display_func);
	// 改变窗口大小时保持图像比例
	glutReshapeFunc(reshape_func);

	// 设置鼠标事件
	glutMouseFunc(onMouse);
	//glutMouseFunc(mouse_click_func);
	//glutMotionFunc(mouse_move_func);
	Initial();
	glutMainLoop();
}
void RotateRect()
{
	angle += 1;
	if (angle > 360)
	{
		angle = 0.0f;
	}
	Sleep(30);
	display_func();
}
// 鼠标事件
void onMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		glutIdleFunc(RotateRect);
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		glutIdleFunc(NULL);
	}
}
// 设置窗口
void reshape_func(int w, int h)
{
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.0, 100.0);
}
// 画图函数
void drawBunny()
{
	// 旋转设置
	cout << getPoint._x << " " << getPoint._y << " " << getPoint._z << endl;
	glRotatef(angle, 0, 1, 0);
	// 绘制透明物体
	for (auto it = vecFacet.begin();it != vecFacet.end();it++)
	{
			glPushMatrix();
			glBegin(GL_TRIANGLES);
			auto itt = _listPoint.find(it->_vertex_1);
			if (itt != _listPoint.end())
			{
				glColor3f(0.0, 1.0, 0.0);
				glVertex3f(it->_vertex_1._x /100.0f, it->_vertex_1._y / 100.0f, it->_vertex_1._z / 100.0f);
				//glColor3f(0.0, 1.0, 0.0);
				glVertex3f(it->_vertex_3._x /100.0f, it->_vertex_3._y / 100.0f, it->_vertex_3._z / 100.0f);
				glVertex3f(it->_vertex_2._x / 100.0f, it->_vertex_2._y / 100.0f, it->_vertex_2._z / 100.0f);
				glEnd(); //三角形
				glPopMatrix();
				continue;

			}
			 itt = _listPoint.find(it->_vertex_2);
			if (itt != _listPoint.end())
			{
				glColor3f(0.0, 1.0, 0.0);
				glVertex3f(it->_vertex_2._x / 100.0f, it->_vertex_2._y / 100.0f, it->_vertex_2._z / 100.0f);
				//glColor3f(1.0, 0.0, 1.0);
				glVertex3f(it->_vertex_3._x / 100.0f, it->_vertex_3._y / 100.0f, it->_vertex_3._z / 100.0f);
				glVertex3f(it->_vertex_1._x / 100.0f, it->_vertex_1._y / 100.0f, it->_vertex_1._z / 100.0f);

				glEnd(); //三角形
				glPopMatrix();
				continue;

			}
			 itt = _listPoint.find(it->_vertex_3);
			if (itt != _listPoint.end())
			{
				glColor3f(0.0, 1.0, 0.0);
				glVertex3f(it->_vertex_3._x / 100.0f, it->_vertex_3._y / 100.0f, it->_vertex_3._z / 100.0f);
				glVertex3f(it->_vertex_1._x / 100.0f, it->_vertex_1._y / 100.0f, it->_vertex_1._z / 100.0f);
				glVertex3f(it->_vertex_2._x / 100.0f, it->_vertex_2._y / 100.0f, it->_vertex_2._z / 100.0f);
				glEnd(); //三角形
				glPopMatrix();
				continue;
			} 
			if (getPoint.isEque(it->_vertex_1))
			{
				glColor3f(1.0, 0.0, 0.0);
				glVertex3f(it->_vertex_1._x/100.0f , it->_vertex_1._y/100.0f, it->_vertex_1._z/100.0f );
				glVertex3f(it->_vertex_3._x / 100.0f, it->_vertex_3._y / 100.0f, it->_vertex_3._z / 100.0f);
				glVertex3f(it->_vertex_2._x / 100.0f, it->_vertex_2._y / 100.0f, it->_vertex_2._z / 100.0f);
				glEnd(); //三角形
				glPopMatrix();
			}
			else if (getPoint.isEque(it->_vertex_2))
			{
				glColor3f(1.0, 0.0, 0.0);
				glVertex3f(it->_vertex_1._x/100.0f, it->_vertex_1._y /100.0f, it->_vertex_1._z/100.0f);
				glVertex3f(it->_vertex_3._x /100.0f, it->_vertex_3._y / 100.0f, it->_vertex_3._z /100.0f);
				glVertex3f(it->_vertex_2._x/100.0f , it->_vertex_2._y/100.0f , it->_vertex_2._z/100.0f );
				glEnd(); //三角形
				glPopMatrix();
			}
			else if (getPoint.isEque(it->_vertex_3))
			{
				glColor3f(1.0, 0.0, 0.0);
				glVertex3f(it->_vertex_3._x /100.0f, it->_vertex_3._y /100.0f, it->_vertex_3._z /100.0f);
				glVertex3f(it->_vertex_1._x/100.0f, it->_vertex_1._y/100.0f, it->_vertex_1._z/100.0f);
				glVertex3f(it->_vertex_2._x /100.0f, it->_vertex_2._y /100.0f, it->_vertex_2._z /100.0f);
				glEnd(); //三角形
				glPopMatrix();
			}
			else
			{
				glColor3f(0.5, 0.5, 0.5);
				glVertex3f(it->_vertex_1._x /100, it->_vertex_1._y /100, it->_vertex_1._z /100);
				glVertex3f(it->_vertex_3._x/100, it->_vertex_3._y/100, it->_vertex_3._z/100);
				glVertex3f(it->_vertex_2._x /100, it->_vertex_2._y /100, it->_vertex_2._z /100);
				glEnd(); //三角形
				glPopMatrix();
			}		
	}
	cout <<"getPointSize" <<getPoint._x << " " << getPoint._y << " " << getPoint._z << endl;
}
// 初始化设置
void Initial(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);//白色背景，前3个是RGB，最后是Alpha值，用来控制透明，1.0表示完全不透明
	glShadeModel(GL_FLAT);
	//glMatrixMode(GL_PROJECTION);//OpenGL按照三维方式来处理图像，所以需要一个投影变换将三维图形投影到显示器的二维空间中
	//gluOrtho2D(0.0, 200, 0.0, 150.0);//指定使用正投影将一个x坐标在0~200，y坐标0~150范围内的矩形坐标区域投影到显示器窗口
}
// 显示
void display_func()
{
	// 次数
	//static int z = 0;
	//cout << "显示：" << z++ << " " << endl;

	// 表示要清除颜色缓冲
	glClear(GL_COLOR_BUFFER_BIT);
	// 指定显示模式
	glMatrixMode(GL_MODELVIEW);
	// 恢复初始坐标
	glLoadIdentity();
	// 该函数定义一个视图矩阵，并与当前矩阵相乘
	gluLookAt(ex[view], ey[view], ez[view], fx[view], fy[view], fz[view], 0.0, 1.0, 0.0);

	// 画网格线
	glColor3f(0.82f, 0.82f, 0.82f);
	glBegin(GL_LINES);
	float i, j;
	for (i = -50; i <= 50; i = i + 0.2)
	{
		glVertex3f(i, -0.25f, -50.0f);
		glVertex3f(i, -0.25f, 50.0f);
	}
	for (j = -50; j <= 50; j += 0.2)
	{
		glVertex3f(-50.0f, -0.25f, j);
		glVertex3f(50.0f, -0.25f, j);
	}
	glEnd();
	glPushMatrix();
	drawBunny();
	glPopMatrix();

	// 交换两个缓冲区指针
	glutSwapBuffers();
}
// 查找n环领域
void GetFieldcirculation(int index, int nField)
{
	// 根据index找到点,注意重复问题，已经判断的点不能再次加入
	list<int>_listPointTemp;
	_listPointTemp.push_back(index);
	list<int>_listPointTemp1;
	_listPointTemp1.push_back(index);
	while (nField > 0)
	{
		_listPointTemp = _listPointTemp1;
		list<int>_listPointTemp1;
		for (auto it = _listPointTemp.begin(); it != _listPointTemp.end(); it++)
		{

			// 获取index，哪个点	
			index = *it;
			for (auto iter = vecPoint.begin(); iter != vecPoint.end(); iter++)
			{
				// 找到对应的点	

				if (iter->second.index == index)
				{
					Point	getPoint1(iter->second._x, iter->second._y, iter->second._z);
					getPoint = getPoint1;
					// 找到对应的面片信息
					for (auto it = vecFacet.begin(); it != vecFacet.end(); it++)
					{
						if (it->_vertex_1.index == index)
						{
							it->_vertex_1.flag = 1;
							_listPoint.insert(it->_vertex_1);

							if (it->_vertex_2.flag == 0)
							{
								_listPointTemp1.push_back(it->_vertex_2.index);
								_listPoint.insert(it->_vertex_2);
							}
							if (it->_vertex_3.flag == 0)
							{
								_listPointTemp1.push_back(it->_vertex_3.index);
								_listPoint.insert(it->_vertex_3);

							}
						}
						else if (it->_vertex_2.index == index)
						{
							it->_vertex_2.flag = 1;
							_listPoint.insert(it->_vertex_2);

							if (it->_vertex_1.flag == 0)
							{
								_listPointTemp1.push_back(it->_vertex_1.index);
								_listPoint.insert(it->_vertex_1);

							}
							if (it->_vertex_3.flag == 0)
							{
								_listPointTemp1.push_back(it->_vertex_3.index);
								_listPoint.insert(it->_vertex_3);

							}
						}
						else if (it->_vertex_3.index == index)
						{
							it->_vertex_3.flag = 1;
							_listPoint.insert(it->_vertex_3);
							if (it->_vertex_1.flag == 0)
							{
								_listPointTemp1.push_back(it->_vertex_1.index);
								_listPoint.insert(it->_vertex_1);
							}
							if (it->_vertex_2.flag == 0)
							{
								_listPointTemp1.push_back(it->_vertex_2.index);
								_listPoint.insert(it->_vertex_2);

							}
						}
					}
				}
			}
		}
		nField--;
	}
	_listPoint.erase(getPoint);
}
void ReadPly()
{
	ifstream fin("D:\\work\\任务\\Task7\\bunny(1).ply");
	string s;
	int countPoint = 0;
	if (fin.fail())

	{
		cout << "打开文件错误!" << endl;
		exit(0);
	}
	while (getline(fin, s))
	{
		if (strstr(s.c_str(), "end_header") != NULL)
		{
			while (getline(fin, s))
			{
				//cout << "s:" << s << endl;
				istringstream iss(s);
				float x1, y1, z1;
				iss >> x1 >> y1 >> z1;
				Point point(countPoint, x1, y1, z1);
				cout << point._x <<" "<< point._y << endl; 
				point.flag = 0;
				vecPoint.insert(pair<int, Point>(countPoint, point));
				countPoint++;
				if (strstr(s.c_str(), "3 0 1 2") != NULL)
				{
					break;
				}
			}
			break;
		}
	}
	ifstream fin1("D:\\work\\任务\\Task7\\bunny(1).ply");
	string s1;
	double Ssum = 0, TotalVolume = 0;
	while (getline(fin1, s1))
	{
		if (strstr(s1.c_str(), "3 0 1 2") != NULL)
		{
			//cout << "s1: " << s1 << endl;
			Facet facet;
			auto it = vecPoint.find(0);
			if (it != vecPoint.end())
			{
				facet._vertex_1 = it->second;
			}
			auto it1 = vecPoint.find(1);
			if (it1 != vecPoint.end())
			{
				facet._vertex_2 = it1->second;
			}
			auto it2 = vecPoint.find(2);
			if (it2 != vecPoint.end())
			{
				facet._vertex_3 = it2->second;
			}
			vecFacet.push_back(facet);
			while (getline(fin, s))
			{
				//cout << "s11: " << s << endl;
				istringstream isss(s);
				Facet facet;
				int a, b, c, d;
				isss >> a >> b >> c >> d;

				auto it = vecPoint.find(b);
				if (it != vecPoint.end())
				{
					facet._vertex_1 = it->second;
				}
				auto it1 = vecPoint.find(c);
				if (it1 != vecPoint.end())
				{
					facet._vertex_2 = it1->second;
				}
				auto it2 = vecPoint.find(d);
				if (it2 != vecPoint.end())
				{
					facet._vertex_3 = it2->second;
				}
				vecFacet.push_back(facet);
			}
			break;
		}
	}
}
