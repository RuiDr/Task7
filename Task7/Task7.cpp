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
GLfloat default_matrix[16];
GLfloat modelview_z_dis;
const int win_w = 700, win_h = 700;
GLfloat ex[3] = { 0.0, 0.0, 0.0 }; //观察者位置
GLfloat ey[3] = { 1.0, 0.0, 0.0 };
GLfloat ez[3] = { 4.0, 0.0, 0.0 };
GLfloat fx[3] = { 0.0, 0.0, 0.0 }; //观察对象位置
GLfloat fy[3] = { 0.0, 0.0, 0.0 };
GLfloat fz[3] = { 0.0, 0.0, 0.0 };
GLfloat x_min = .0, x_max = 20.0, y_min = .0, y_max = 20.0, z_min = .0, z_max = 20.0;
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
	GetFieldcirculation(0, 5);
	cout << " " << getPoint._x << " " << getPoint._y << " " << getPoint._z << endl;
	cout << "总共多少个点: " << _listPoint.size() << endl;
	cout << "The run time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
	// 画图
	glutInit(&argc, argv);
	// 设置初始显示模式，指定RGB颜色模式以及指定双缓存窗口
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_DEPTH);
	glutInitWindowSize(win_w, win_h);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("bunny");
	// 改变窗口大小时保持图像比例
	glutReshapeFunc(reshape_func);
	// 显示图像
	glutDisplayFunc(display_func);
	// 设置鼠标事件
	//glutMouseFunc(onMouse);
	glutMouseFunc(mouse_click_func);
	glutMotionFunc(mouse_move_func);
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
/* 以下三个函数对物体进行平移、旋转、缩放，他们均在视觉(绝对)坐标下进行
 * 正常调用 glTranslate,glRotate,glScale 均是在局部坐标下进行(按正序看)
 * 为了达到在视觉坐标下操作的效果，需要将矩阵左乘到当前矩阵
 */
void absolute_translate(GLfloat x, GLfloat y, GLfloat z)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, z);
	glMultMatrixf(modelview_matrix); // 使变换矩阵左乘到当前矩阵，这样才适合绝对坐标的考虑
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
	glPopMatrix();
}
void absolute_rotate(GLfloat dgree, GLfloat vecx, GLfloat vecy, GLfloat vecz)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(.0, .0, -modelview_z_dis);		// 平移回去，注意该句和后两句要倒序来看
	glRotatef(dgree, vecx, vecy, vecz);// 积累旋转量
	glTranslatef(.0, .0, modelview_z_dis);		// 先平移到原点
	glMultMatrixf(modelview_matrix); // 使变换矩阵左乘到当前矩阵，这样才适合绝对坐标的考虑
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
	glPopMatrix();
}
void absolute_scale(GLfloat factor)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(.0, .0, -modelview_z_dis);		// 平移回去，注意该句和后两句要倒序来看
	glScalef(factor, factor, factor);
	glTranslatef(.0, .0, modelview_z_dis);		// 先平移到原点
	glMultMatrixf(modelview_matrix); // 使变换矩阵左乘到当前矩阵，这样才适合绝对坐标的考虑
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
	glPopMatrix();
}
// 设置窗口
void reshape_func(int w, int h)
{
	/*if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.0, 100.0);*/
	glViewport(0, h - win_w, win_w, win_h);

}
// 画图函数
void drawBunny()
{
	// 旋转设置
	glRotatef(angle, 0, 1, 0);
	// 绘制一个灰色的球
	GLfloat color[] = { .4f,.4f,.4f,1.0f };
	// 反射
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);

	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30.0);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

	// 这个函数告诉接下来要做什么的声明，参数有三种模式：
	// GL_PROJECTION 投影, GL_MODELVIEW 模型视图, GL_TEXTURE 纹理
	glMatrixMode(GL_MODELVIEW);
	// 绘制透明物体
	for (auto it = vecFacet.begin();it != vecFacet.end();it++)
	{
			glPushMatrix();
			glBegin(GL_TRIANGLES);
		    glColor3f(0.0, 0.0, 1.0);
			glVertex3f(it->_vertex_1._x , it->_vertex_1._y , it->_vertex_1._z );
			glVertex3f(it->_vertex_3._x, it->_vertex_3._y, it->_vertex_3._z);
			glVertex3f(it->_vertex_2._x , it->_vertex_2._y , it->_vertex_2._z);
			glEnd(); //三角形
			glPopMatrix();
			
	}
	glPointSize(9.0);
	glBegin(GL_POINTS);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(getPoint._x, getPoint._y, getPoint._z);
	glEnd();
	for (auto it = _listPoint.begin();it != _listPoint.end();it++)
	{
		glBegin(GL_POINTS);
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(it->_x, it->_y, it->_z);
		glEnd();
	}
	cout <<"getPointSize" <<getPoint._x << " " << getPoint._y << " " << getPoint._z << endl;
}
// 初始化设置
void Initial(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);//白色背景，前3个是RGB，最后是Alpha值，用来控制透明，1.0表示完全不透明
	//glShadeModel(GL_FLAT);
	//glMatrixMode(GL_PROJECTION);//OpenGL按照三维方式来处理图像，所以需要一个投影变换将三维图形投影到显示器的二维空间中
	//gluOrtho2D(0.0, 200, 0.0, 150.0);//指定使用正投影将一个x坐标在0~200，y坐标0~150范围内的矩形坐标区域投影到显示器窗口
		// 投影矩阵初始化
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(20.0, 1.0, 10.0, 300.0);
	// 默认矩阵初始化
	glMatrixMode(GL_MODELVIEW);
	// 对当前矩阵进行初始化
	glLoadIdentity();
	/*函数原型
		void gluLookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz);
	该函数定义一个视图矩阵，并与当前矩阵相乘。
		第一组eyex, eyey, eyez 相机在世界坐标的位置
		第二组centerx, centery, centerz 相机镜头对准的物体在世界坐标的位置
		第三组upx, upy, upz 相机向上的方向在世界坐标中的方向
		你把相机想象成为你自己的脑袋：
		第一组数据就是脑袋的位置
		第二组数据就是眼睛看的物体的位置
		第三组就是头顶朝向的方向（因为你可以歪着头看同一个物体）*/
	gluLookAt(100.0 + (x_min + x_max)*.5, 100.0 + (y_min + y_max)*.5, 100.0 + (z_min + z_max)*.5,
		(x_min + x_max)*.5, (y_min + y_max)*.5, (z_min + z_max)*.5, .0, .0, 1.0);
	modelview_z_dis = 100.0f * sqrt(3.0f);
	//这个函数的作用是取出GL_MODELVIEW_MATRIX，然后存储在mat这个矩阵中，用于逆变换等
	glGetFloatv(GL_MODELVIEW_MATRIX, default_matrix);
	// 将default拷贝到modelview中，sizeof表示拷贝大小
	memcpy(modelview_matrix, default_matrix, sizeof(default_matrix));
	glLoadIdentity();

}
// 显示
void display_func()
{
	static int z = 0;
	std::cout << "display:" << z++ << std::endl;
	// 表示要清除颜色缓冲以及深度缓冲，可以使用以下标志位
	// GL_COLOR_BUFFER_BIT:    当前可写的颜色缓冲
	// GL_DEPTH_BUFFER_BIT:    深度缓冲
	// GL_ACCUM_BUFFER_BIT : 累积缓冲
	// GL_STENCIL_BUFFER_BIT : 模板缓冲
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	// 设置当前矩阵
	glLoadMatrixf(modelview_matrix);

	drawBunny();
	// 画线
	// 交换两个缓冲区的指针，当绘制完成，将结果显示在屏幕上，从而解决频繁刷新导致的画面闪烁问题
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

void absolute_default()
{
	memcpy(modelview_matrix, default_matrix, sizeof(default_matrix));
}
// 鼠标状态变量，用来在鼠标点击事件和拖动事件之间通信
static bool l_button_down = false, r_button_down = false, mid_button_down = false;
static int last_x = -1, last_y = -1;
#define  GLUT_WHEEL_UP		3 // 滚轮操作  
#define  GLUT_WHEEL_DOWN	4
void mouse_click_func(int button, int state, int x, int y)
{
	y = win_h - y;
	switch (button) {
		// 左按钮
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			l_button_down = true;
			last_x = x; last_y = y;
			glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		}
		else {
			l_button_down = false;
			last_x = -1; last_y = -1;
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
		break;
		// 中间
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_DOWN) {
			mid_button_down = true;
			last_x = x; last_y = y;
			glutSetCursor(GLUT_CURSOR_CYCLE);

		}
		else {
			mid_button_down = false;
			last_x = -1; last_y = -1;
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
		break;
		// 右边
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN) {
			r_button_down = true;
			absolute_default();
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
			glutPostRedisplay();
		}
		else {
			r_button_down = false;
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
		break;
		// 转动
	case GLUT_WHEEL_UP:
		if (state == GLUT_UP) {
			absolute_scale(.9f);
			glutPostRedisplay();
		}
		break;
		// 向下转动
	case GLUT_WHEEL_DOWN:
		if (state == GLUT_UP) {
			absolute_scale(1.1f);
			glutPostRedisplay();
		}
		break;
	}

}
// 鼠标移动
void mouse_move_func(int x, int y)
{
	y = win_h - y;
	if (last_x >= 0 && last_y >= 0 && (last_x != x || last_y != y)) {
		GLfloat deltax = GLfloat(x - last_x), deltay = GLfloat(y - last_y);
		if (mid_button_down) {
			absolute_translate(deltax * .1f, deltay * .1f, .0f);
			glutPostRedisplay();
		}
		else if (l_button_down) {
			GLfloat dis = sqrt(deltax*deltax + deltay * deltay);
			absolute_rotate(dis, -deltay / dis, deltax / dis, .0);
			glutPostRedisplay();
		}
	}
	last_x = x; last_y = y;
}