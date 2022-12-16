#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;
void normal(double o[], double a[], double b[]) {
	double a1[] = { a[0] - o[0], a[1] - o[1],a[2] - o[2] };
	double b1[] = { b[0] - o[0], b[1] - o[1], b[2] - o[2] };
	glNormal3d(a1[1] * b1[2] - b1[1] * a1[2], -a1[0] * b1[2] + b1[0] * a1[2], a1[0] * b1[1] - b1[0] * a1[1]);
}
void nenormal(double o[], double a[], double b[]) {
	double a1[] = { a[0] - o[0], a[1] - o[1],a[2] - o[2] };
	double b1[] = { b[0] - o[0], b[1] - o[1], b[2] - o[2] };
	glNormal3d(-(a1[1] * b1[2] - b1[1] * a1[2]),-( -a1[0] * b1[2] + b1[0] * a1[2]),-( a1[0] * b1[1] - b1[0] * a1[1]));
}

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


void figure(void) {
	double A[] = { 1, 1, 0 };
	double B[] = { -1, 1, 0 };
	double C[] = { -1,-1, 0 };
	double D[] = { 1,-1, 0 };
	glBegin(GL_QUADS);
	glColor3d(1, 0.7, 0.9);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(D);
	glEnd();

	double A1[] = { 1, 1, 0 };
	double B1[] = { -1, 1, 0 };
	double C1[] = { -1,1, 1 };
	double D1[] = { 1,1, 1 };
	glBegin(GL_QUADS);
	glColor3d(1, 0.4, 0.5);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glVertex3dv(D1);
	glEnd();

	double A2[] = { -1, 1, 1 };
	double B2[] = { -1, 1, 0 };
	double C2[] = { -1,-1, 0 };
	double D2[] = { -1,-1, 1 };
	glBegin(GL_QUADS);
	glColor3d(0, 0, 1);
	glVertex3dv(A2);
	glVertex3dv(B2);
	glVertex3dv(C2);
	glVertex3dv(D2);
	glEnd();

	double A3[] = { 1, -1, 1 };
	double B3[] = { -1, -1, 1 };
	double C3[] = { -1,-1, 0 };
	double D3[] = { 1,-1, 0 };
	glBegin(GL_QUADS);
	glColor3d(0, 1, 0);
	glVertex3dv(A3);
	glVertex3dv(B3);
	glVertex3dv(C3);
	glVertex3dv(D3);
	glEnd();

	double A4[] = { 1, 1, 0 };
	double B4[] = { 1, 1, 1 };
	double C4[] = { 1,-1, 1 };
	double D4[] = { 1,-1, 0 };
	glBegin(GL_QUADS);
	glColor3d(1, 0, 0);
	glVertex3dv(A4);
	glVertex3dv(B4);
	glVertex3dv(C4);
	glVertex3dv(D4);
	glEnd();

	double A5[] = { 1,1,1 };
	double B5[] = { 1, -1, 1 };
	double C5[] = { 0, 0, 5 };
	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.8, 0.1);
	glVertex3dv(A5);
	glVertex3dv(B5);
	glVertex3dv(C5);
	glEnd();

	double A6[] = { 1,-1,1 };
	double B6[] = { -1, -1, 1 };
	double C6[] = { 0, 0, 5 };
	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.6, 0.3);
	glVertex3dv(A6);
	glVertex3dv(B6);
	glVertex3dv(C6);
	glEnd();

	double A7[] = { -1,1,1 };
	double B7[] = { -1, -1, 1 };
	double C7[] = { 0, 0, 5 };
	glBegin(GL_TRIANGLES);
	glColor3d(0.2, 0.4, 0.1);
	glVertex3dv(A7);
	glVertex3dv(B7);
	glVertex3dv(C7);
	glEnd();

	double A8[] = { -1,1,1 };
	double B8[] = { 1, 1, 1 };
	double C8[] = { 0, 0, 5 };
	glBegin(GL_TRIANGLES);
	glColor3d(0.5, 0.8, 0.1);
	glVertex3dv(A8);
	glVertex3dv(B8);
	glVertex3dv(C8);
	glEnd();
}

void figuretreug(double a[], double b[], double c[]) {
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.7, 0.9);
	glVertex3dv(a);
	glTexCoord2d(0,0);
	glVertex3dv(b);
	glTexCoord2d(4, -2);
	glVertex3dv(c);
	glTexCoord2d(4, -8);
	glEnd();
}

void figurequadr(double a[], double b[], double c[], double d[]) {
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3d(1, 0, 0);
	glTexCoord2d(1, 0);
	glVertex3dv(a);
	glTexCoord2d(0, 0);
	glVertex3dv(b);
	glTexCoord2d(0, 1);
	glVertex3dv(c);
	glTexCoord2d(1, 1);
	glVertex3dv(d);
	glEnd();
}

void circle(double a[], double b[]) {
	double c[] = { 6, 3, 0 };
	double c1[] = { 6, 3, 3 };
	double degree = 3.14159 / 180.0;
	double x[] = { b[0], b[1],b[2] };
	double y[] = { a[0], a[1] ,0 };
	double radius = sqrt(2 * 2 + 3 * 3);
	for (double i = 0.0; i < 360.0; i = i + 0.1)
	{
		y[0] = c[0] + (radius * cos(i * degree));
		y[1] = c[1] + (sin(i * degree) * radius);
		if ((y[0] >= a[0]) && (y[1] >= b[1])) {
			double x1488[] = { x[0],x[1],x[2] + 3 };
			double y1488[] = { y[0],y[1],y[2] + 3 };

			glNormal3d(0, 0, -1);
			figuretreug(c, x, y);
			glNormal3d(0, 0, 1);
			figuretreug(c1, x1488, y1488);
			nenormal(y, x, y1488);//glNormal3d(-(x[1] * y[2] - y[1] * x[2]),-( -x[0] * y[2] + y[0] * x[2]),-( x[0] * y[1] - y[0] * x[1]));
			figurequadr(x, y, y1488, x1488);
			x[0] = y[0];
			x[1] = y[1];

		}

	}
	double x1488[] = { x[0],x[1],x[2] + 3 };
	double y1488[] = { y[0],y[1],y[2] + 3 };

	glNormal3d(0, 0, -1);
	figuretreug(c, x, y);
	glNormal3d(0, 0, 1);
	figuretreug(c1, x1488, y1488);
	nenormal(y, x, y1488);//glNormal3d(-(x[1] * y[2] - y[1] * x[2]),-( -x[0] * y[2] + y[0] * x[2]),-( x[0] * y[1] - y[0] * x[1]));
	figurequadr(x, y, y1488, x1488);


}

void incercle() {
	double A[] = { 0,0,0 };
	double B[] = { 4, 6, 0 };
	double C[] = { 8, 0, 0 };
	double D[] = { 4,-2,0 };
	double E[] = { 4, -8, 0 };
	double F[] = { -6, -8, 0 };
	double G[] = { -8, -4, 0 };
	double H[] = { -4, 2, 0 };
	double A1[] = { 0,0,3 };


	double c[] = { -1,-17,0 };
	double radius = sqrt((c[0] - E[0]) * (c[0] - E[0]) + (c[1] - E[1]) * (c[1] - E[1]));
	double point0[] = { E[0],E[1],E[2] };
	double point1[] = { E[0],E[1],E[2] };
	double degree = 3.14159 / 180.0;

	for (double i = 0.0; i < 360; i = i + 0.1) {
		point1[0] = c[0] + (radius * cos((i)*degree));
		point1[1] = c[1] + (radius * sin((i)*degree));
		glColor3d(1, 0, 0);

		if (point1[0] >= F[0] && point1[0] <= E[0] && point1[1] >= F[1]) {
			glNormal3d(0, 0, -1);
			figuretreug(A, point0, point1);
			double point01[] = { point0[0],point0[1],point0[2] + 3 };
			double point11[] = { point1[0],point1[1],point1[2] + 3 };
			glNormal3d(0, 0, 1);
			figuretreug(A1, point01, point11);
			normal(point1, point0, point11);//glNormal3d(point0[1] * point1[2] - point1[1] * point0[2], -point0[0] * point1[2] + point1[0] * point0[2], point0[0] * point1[1] - point1[0] * point0[1]);
			figurequadr(point0, point1, point11, point01);

			point0[0] = point1[0];
			point0[1] = point1[1];
		}
	}
}


void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  


	//Начало рисования квадратика станкина
	/*double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	//конец рисования квадратика станкина

	double A[] = { 0,0,0 };
	double B[] = { 4, 6, 0 };
	double C[] = { 8, 0, 0 };
	double D[] = { 4,-2,0 };
	double E[] = { 4, -8, 0 };
	double F[] = { -6, -8, 0 };
	double G[] = { -8, -4, 0 };
	double H[] = { -4, 2, 0 };
	double A1[] = { 0,0,3 };
	double B1[] = { 4, 6,3 };
	double C1[] = { 8, 0,3 };
	double D1[] = { 4,-2,3 };
	double E1[] = { 4, -8,3 };
	double F1[] = { -6, -8,3 };
	double G1[] = { -8, -4, 3 };
	double H1[] = { -4, 2, 3 };

	glNormal3d(0, 0, -1);

	figuretreug(A, B, C);
	figuretreug(A, C, D);
	figuretreug(A, D, E);
	//figuretreug(A, E, F);
	figuretreug(A, F, G);
	figuretreug(A, G, H);

	glNormal3d(0, 0, 1);

	figuretreug(A1, B1, C1);
	figuretreug(A1, C1, D1);
	figuretreug(A1, D1, E1);
	//figuretreug(A1, E1, F1);
	figuretreug(A1, F1, G1);
	figuretreug(A1, G1, H1);

	

	//glNormal3d((A[1]*B[2]-B[1]*A[2], -A[0]*B[2]+B[0]*A[2], A[0]*B[1]-B[0]*A[1]);
	glBindTexture(GL_TEXTURE_2D, texId);
	normal(B, A, B1);
	figurequadr(A, B, B1, A1);
	
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(B1);
	glTexCoord2d(0, 1);
	glVertex2dv(A1);
	normal(C, B, C1);//glNormal3d(B[1] * C[2] - C[1] * B[2], -B[0] * C[2] + C[0] * B[2], B[0] * C[1] - C[0] * B[1]);
	figurequadr(B, C, C1, B1);
	normal(D, C, D1); //glNormal3d(C[1] * D[2] - D[1] * C[2], -C[0] * D[2] + D[0] * C[2], C[0] * D[1] - D[0] * C[1]);
	figurequadr(C, D, D1, C1);
	normal(E, D, E1); //glNormal3d(D[1] * E[2] - E[1] * D[2], -D[0] * E[2] + E[0] * D[2], D[0] * E[1] - E[0] * D[1]);
	figurequadr(D, E, E1, D1);
	//figurequadr(E, F, F1, E1);
	normal(G, F, G1); //glNormal3d(F[1] * G[2] - G[1] * F[2], -F[0] * G[2] + G[0] * F[2], F[0] * G[1] - G[0] * F[1]);
	figurequadr(F, G, G1, F1);
	normal(H, G, H1); //glNormal3d(G[1] * H[2] - H[1] * G[2], -G[0] * H[2] + H[0] * G[2], G[0] * H[1] - H[0] * G[1]);
	figurequadr(G, H, H1, G1);
	normal(A, H, A1); //glNormal3d(-(H[1] * A[2] - A[1] * H[2]),-( -H[0] * A[2] + A[0] * H[2]),-( H[0] * A[1] - A[0] * H[1]));
	figurequadr(H, A, A1, H1);


	
	circle(B, C);
	
	incercle();


   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}