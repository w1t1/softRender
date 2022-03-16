#include <iostream>
#include <graphics.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <algorithm>
#include <windows.h>
#include <tchar.h>

#include <vector>

static float screen_width = 640.0f;
static float screen_height = 480.0f;
static std::vector< std::vector<unsigned int>> screen_color_buffer(screen_width + 1, std::vector<unsigned int>(screen_height + 1, 0XFFFFFF));

struct Point
{
	Point(float x, float y, float z) :x(x), y(y), z(z) {}
	Point(float x, float y, float z, unsigned int color) :x(x), y(y), z(z), color(color) {
		r = color & 0x000000FF;
		g = (color & 0x0000FF00) >> 8;
		b = (color & 0x00FF0000) >> 16;
	}
	Point() :x(0.f), y(0.f), z(0.f), color(0xFFFFFF) {}
	float x, y, z;
	unsigned color = 0xFFFFFF;
	int r,g,b;
};

struct Vector3
{
	Vector3(float x, float y, float z) :x(x), y(y), z(z) {}
	Vector3() :x(0.f), y(0.f), z(0.f) {}
	Vector3 operator+(const Vector3& v1) {
		Vector3 v;
		v.x = this->x + v1.x;
		v.y = this->y + v1.y;
		v.z = this->z + v1.z;
		return v;
	}

	Vector3 operator-(const Vector3& v1) {
		Vector3 v;
		v.x = this->x - v1.x;
		v.y = this->y - v1.y;
		v.z = this->z - v1.z;
		return v;
	}

	float Dot(const Vector3& v1) {
		return this->x * v1.x + this->y * v1.y + this->z * v1.z;
	}

	float x, y, z;
};
//中点画线法
void line1(int x1, int y1, int x2, int y2) {

	int x, y, d0, d1, d2, a, b;
	y = y1;
	a = y1 - y2;          //直线方程中的a的算法
	b = x2 - x1;          //直线方程中的b的算法
	d0 = 2 * a + b;         //增量初始值
	d1 = 2 * a;           //当>=0时的增量
	d2 = 2 * (a + b);       //当<0时的增量
	for (x = x1; x <= x2; x++) {
		putpixel(x, y, 0x0000FF);   //打亮
		if (d0 < 0) {
			y++;
			d0 += d2;
		}
		else {

			d0 += d1;
		}

	}
}
//Bresenham画线算法
void line2(int x1, int y1, int x2, int y2) {

	int x, y, dx, dy, d;
	y = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	d = 2 * dy - dx;        //增量d的初始值
	for (x = x1; x <= x2; x++) {
		putpixel(x, y, GREEN);   //打亮
		if (d < 0) {
			d += 2 * dy;
		}
		else {
			y++;
			d += 2 * dy - 2 * dx;
		}



	}

}

//求点的重心坐标
void BarycentricInTriangle(Point p, Point a, Point b, Point c, float& u, float& v,float& w)
{
	//Vector3 v0 = Vector3(c.x - a.x, c.y - a.y, 0);
	//Vector3 v1 = Vector3(b.x - a.x, b.y - a.y, 0);
	//Vector3 v2 = Vector3(p.x - a.x, p.y - a.y, 0);
	//float dot00 = v0.Dot(v0);
	//float dot01 = v0.Dot(v1);
	//float dot02 = v0.Dot(v2);
	//float dot11 = v1.Dot(v1);
	//float dot12 = v1.Dot(v2);

	//u = (dot02 * dot11 - dot12 * dot01) / (dot00 * dot11 - dot01 * dot01);
	//v = (dot02 * dot01 - dot12 * dot00) / (dot01 * dot01 - dot00 * dot11);

	v = ((a.y - c.y) * p.x + (c.x- a.x) * p.y + (a.x * c.y - c.x* a.y)) / ((a.y - c.y) * b.x + (c.x - a.x) * b.y + (a.x * c.y - c.x * a.y));
	w = ((a.y - b.y) * p.x + (b.x- a.x) * p.y + (a.x * b.y - b.x* a.y)) / ((a.y - b.y) * c.x + (b.x - a.x) * c.y + (a.x * b.y - b.x * a.y));
	//u = ((-(p.x - b.x) * (c.y - b.y)) + ((p.y - b.y) * (c.x - b.x))) / ((-(a.x - b.x) * (c.y - b.y)) + ((b.y - c.y) * (a.x - c.x)));
	//v = ((-(p.x - c.x) * (a.y - c.y)) + ((p.y - c.y) * (a.x - c.x))) / ((-(b.x - c.x) * (a.y - c.y)) + ((b.y - c.y) * (a.x - c.x)));
	u = 1 - w - v;
}

void DrawTriangle(Point pa, Point pb, Point pc) {
	float aabb_x_min = std::max(0.f, std::min(std::min(pa.x, pb.x), pc.x));
	float aabb_x_max = std::min(screen_width, std::max(std::max(pa.x, pb.x), pc.x));
	float aabb_y_min = std::max(0.f, std::min(std::min(pa.y, pb.y), pc.y));
	float aabb_y_max = std::min(screen_height, std::max(std::max(pa.y, pb.y), pc.y));

	for (int i = aabb_x_min; i <= aabb_x_max; i++)
	{
		for (int j = aabb_y_min; j <= aabb_y_max; j++)
		{
			float u, v, w;
			BarycentricInTriangle(Point((float)i, (float)j, 0), pa, pb, pc, u, v, w);
			if (u < 0 || v < 0 || w < 0)
			{
				continue;
			}
			int red = u * (float)pa.r + v * (float)pb.r + w * (float)pc.r;
			int green = u * (float)pa.g + v * (float)pb.g + w * (float)pc.g;
			int blue = u * (float)pa.b + v * (float)pb.b + w * (float)pc.b;
			screen_color_buffer[i][j] = red | (green << 8) | (blue << 16);
		}
	}
}

void Draw(std::vector< std::vector<unsigned int>>& color) {
	for (int i = 0; i < color.size(); i++){
		for (int j = 0; j < color[i].size(); j++) {
			putpixel(i, j, color[i][j]);
		}
	}
	//putpixel(color.size()-1, color[color.size() - 1][color[color.size() - 1].size() -1 ], 0xFF0000);
}


int main()
{
	initgraph(screen_width, screen_height);       //打开EGE初始化
	//for (int i = 0; i < 640; i++) {
	//	for (int j = 0; j < 480; j++) {
	//		color[i][j] = 0X00FF00;
	//	}
	//}
	DrawTriangle(Point(-100.0, -300.0, 0.0, 0x000000FF), Point(320.0, 600.0, 0.0, 0x0000FF00), Point(640.0, 0.0, 0.0, 0x00FF0000));
	Draw(screen_color_buffer);
	//line1(200, 160, 400, 400);   //画线
	//getch();                  //等待用户操作
	Sleep(100000);
	closegraph();             //关闭图形
	return 0;
}