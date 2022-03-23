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

#include "Data.h"

static float screen_width = 640.0f;
static float screen_height = 480.0f;
static std::vector< std::vector<unsigned int>> screen_color_buffer(screen_width + 1, std::vector<unsigned int>(screen_height + 1, 0XFFFFFF));
static std::vector< std::vector<int>> screen_depth_buffer(screen_width + 1, std::vector<int>(screen_height + 1, 2147483647));

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
 
static std::vector<Point> point_buffer_;
static std::vector<int> index_buffer_;

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

struct Vector4
{
	Vector4(float x, float y, float z, float w) :x(x), y(y), z(z), w(w) {}
	Vector4() :x(0.f), y(0.f), z(0.f), w(0.f) {}
	Vector4 operator+(const Vector4& v1) {
		Vector4 v;
		v.x = this->x + v1.x;
		v.y = this->y + v1.y;
		v.z = this->z + v1.z;
		v.w = this->w + v1.w;
		return v;
	}

	Vector4 operator-(const Vector4& v1) {
		Vector4 v;
		v.x = this->x - v1.x;
		v.y = this->y - v1.y;
		v.z = this->z - v1.z;
		v.w = this->w - v1.w;
		return v;
	}

	float Dot(const Vector4& v1) {
		return this->x * v1.x + this->y * v1.y + this->z * v1.z + this->w * v1.w;
	}

	float x, y, z, w;
};

//�е㻭�߷�
void line1(int x1, int y1, int x2, int y2) {

	int x, y, d0, d1, d2, a, b;
	y = y1;
	a = y1 - y2;          //ֱ�߷����е�a���㷨
	b = x2 - x1;          //ֱ�߷����е�b���㷨
	d0 = 2 * a + b;         //������ʼֵ
	d1 = 2 * a;           //��>=0ʱ������
	d2 = 2 * (a + b);       //��<0ʱ������
	for (x = x1; x <= x2; x++) {
		putpixel(x, y, 0x0000FF);   //����
		if (d0 < 0) {
			y++;
			d0 += d2;
		}
		else {

			d0 += d1;
		}

	}
}
//Bresenham�����㷨
void line2(int x1, int y1, int x2, int y2) {

	int x, y, dx, dy, d;
	y = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	d = 2 * dy - dx;        //����d�ĳ�ʼֵ
	for (x = x1; x <= x2; x++) {
		putpixel(x, y, GREEN);   //����
		if (d < 0) {
			d += 2 * dy;
		}
		else {
			y++;
			d += 2 * dy - 2 * dx;
		}



	}

}

//������������
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

//ͶӰ
Vector4 Projection(Vector4& p)
{
	//������д��һ����ƽ��1��Զƽ��1000��fov 60���ݺ��4/3��ƽ��ͷ��
	double cot_fov_2 = 1.732;
	double cot_fov_2_aspect = 1.299;
	double near_plane = 1.0f;
	double far_plane = 1000.0f;

	Vector4 new_p;
	new_p.x = p.x * cot_fov_2_aspect;
	new_p.y = p.y * cot_fov_2;
	new_p.z = (far_plane * p.z - far_plane * near_plane) / (far_plane - near_plane);
	new_p.w = p.z;
	return new_p;
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
			int depth = u * (float)pa.z + v * (float)pb.z + w * (float)pc.z;
			if (screen_depth_buffer[i][j] < depth) {
				continue;
			}
			int red = u * (float)pa.r + v * (float)pb.r + w * (float)pc.r;
			int green = u * (float)pa.g + v * (float)pb.g + w * (float)pc.g;
			int blue = u * (float)pa.b + v * (float)pb.b + w * (float)pc.b;
			screen_color_buffer[i][j] = red | (green << 8) | (blue << 16);
		}
	}
}

void DrawBuffer()
{
	int tri_num = index_buffer_.size() / 3;
	for (int i = 0; i < tri_num; i++)
	{
		DrawTriangle(point_buffer_[index_buffer_[i*3]], point_buffer_[index_buffer_[i * 3 + 1]], point_buffer_[index_buffer_[i * 3 + 2]]);
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
	initgraph(screen_width, screen_height);       //��EGE��ʼ��
	//for (int i = 0; i < 640; i++) {
	//	for (int j = 0; j < 480; j++) {
	//		color[i][j] = 0X00FF00;
	//	}
	//}
	point_buffer_.push_back(Point(100.0, 0.0, 700.0, 0x000000FF));
	point_buffer_.push_back(Point(100.0, 400.0, 700.0, 0x00FF0000));
	point_buffer_.push_back(Point(400.0, 0.0, 700.0, 0x0000FF00));
	point_buffer_.push_back(Point(400.0, 400.0, 700.0, 0x000000FF));
	for (int i = 0; i < point_buffer_.size(); i++)
	{
		Vector4 point = Vector4(point_buffer_[i].x, point_buffer_[i].y, point_buffer_[i].z, 1.0f);
		Vector4 point_after_projection = Projection(point);
		double screen_x = (point_after_projection.x * screen_width / 2.0 / point_after_projection.w) + screen_width / 2;
		double screen_y = (point_after_projection.y * screen_height / 2.0 / point_after_projection.w) + screen_height / 2;
		double screen_z = point_after_projection.z / point_after_projection.w;
		point_buffer_[i] = Point(screen_x, screen_y, screen_z, point_buffer_[i].color);
	}
	index_buffer_.push_back(0);
	index_buffer_.push_back(1);
	index_buffer_.push_back(2);
	index_buffer_.push_back(2);
	index_buffer_.push_back(1);
	index_buffer_.push_back(3);
	/*DrawTriangle(Point(-100.0, -300.0, 0.0, 0x000000FF), Point(320.0, 600.0, 0.0, 0x0000FF00), Point(640.0, 0.0, 0.0, 0x00FF0000));*/
	while (1)
	{
		DrawBuffer();
		Draw(screen_color_buffer);
	}
	//line1(200, 160, 400, 400);   //����
	//getch();                  //�ȴ��û�����
	closegraph();             //�ر�ͼ��
	return 0;
}