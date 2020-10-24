#include <chrono>
#include <iostream>
#include <Windows.h>
#include <random>


void gotoXY(int x, int y);
void setColor(WORD color);
int Lerp(int a, int b,double t);

class Vec2
{
public:
	int x;
	int y;
	Vec2(int x = 0, int y = 0) : x(x),y(y){}

	double length()
	{
		return sqrt(x * x + y * y);
	}
	double dot(Vec2 right)
	{
		return right.x * this->x + right.y * this->y;
	}
	Vec2 operator-(const Vec2& right)
	{
		return Vec2(this->x - right.x, this->y - right.y);
	}
	Vec2 operator+(const Vec2& right)
	{
		return Vec2(this->x+right.x,this->y+right.y);
	}
	template<class T>
	Vec2 operator*(const T& right)
	{
		if (typeid(T) != typeid(Vec2))
		{
			Vec2 result;
			result.x *= right;
			result.y*= right;
			return result;
		}
	}
};

class Pong 
{
private:
	Vec2 pos;
	Vec2 MovingDir;
	Vec2* tableDimen;
public: 
	Pong(Vec2* dimen,int x = 0, int y = 0) : pos(x, y) 
	{
		tableDimen = dimen;
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine rand (seed);
		std::uniform_int_distribution<int> dis(0, 4);
		int k = dis(rand) - 2;
		while (k == 0)
		{
			k = dis(rand) - 2;
		}
		MovingDir.y = k;

		MovingDir.x = k % 2 ? -1 : 1;
	}
	void update()
	{
		Vec2 pre = pos;
		pos = pos + MovingDir;
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine rand(seed);
		std::uniform_int_distribution<int> dis(1, 2);
		if (pos.x <0  || pos.x >= tableDimen->x)
		{
			MovingDir.x = -MovingDir.x;
			pos = pre;
			pos = pos + MovingDir;
		}
		if (pos.y <= 0 || pos.y >= tableDimen->y)
		{
			if (pos.y < 1) MovingDir.y = dis(rand);
			else MovingDir.y = -dis(rand);
			pos = pre;
			pos = pos + MovingDir;
		}
	}
	Vec2 getPos()
	{
		return pos;
	}
};
class Player
{
private:
	Vec2 size;
	Vec2 pos;
	Vec2* tableDimen;
	int key[2];
public:
	Player(Vec2* t,int up,int down,int posx,int y) : pos(posx, 0),size(1,y),tableDimen(t){
		key[0] = up;
		key[1] = down;
		pos.y = tableDimen->y / 2 - size.y / 2;
	}
	void update(){
		if (GetAsyncKeyState(key[0]) & 1)
		{
			pos = pos + Vec2(0,-1);
			if (pos.y < 1)
			{
				pos.y = 1;
			}
		}
		if (GetAsyncKeyState(key[1]) & 1)
		{
			pos = pos + Vec2(0, 1);
			if (pos.y + size.y> tableDimen->y)
			{
				pos.y = tableDimen->y - size.y+1;
			}
		}
	}
	Vec2 getPos()
	{
		return pos;
	}
	Vec2 getSize()
	{
		return size;
	}
	void draw()
	{
		for (int i = 0; i < size.y; ++i)
		{
			Vec2 pos1(size.x + pos.x - 1, pos.y + Lerp(0, size.y, i /size.length()));
			gotoXY(pos1.x, pos1.y);
			setColor(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);

		}
	}
};

void drawBoder(const Vec2& table);

int main()
{
	Vec2 table(30, 20);
	Player p1(&table,0x57,0x53,0,5);
	Player p2(&table, VK_UP,VK_DOWN, table.x-1, 5);
	Pong k(&table,table.x/2,table.y/2);
	int p1s = 0;
	int p2s = 0;
	while (true)
	{
		
		p1.update();
		k.update();
		p2.update();		
		
	
		gotoXY(0, table.y + 5);
		std::cout << "Player 1 Score: " << p1s;
		gotoXY(0, table.y + 6);
		std::cout << "Player 2 Score: " << p2s << " ";
		gotoXY(k.getPos().x, k.getPos().y);
		setColor(BACKGROUND_BLUE);
		p1.draw();
		p2.draw();
		drawBoder(table);
		if (k.getPos().x < 1)
		{
			if (!(k.getPos().y >= p1.getPos().y && k.getPos().y <= p1.getPos().y + p1.getSize().y))
			{
				gotoXY(0, table.y + 2);
				std::cout << "Player 2 Win with score " << p2s;
				break;
			}
			++p1s;
		}
		else if (k.getPos().x + 1 >= table.x)
		{
			if (!(k.getPos().y >= p2.getPos().y && k.getPos().y <= p2.getPos().y + p2.getSize().y))
			{
				gotoXY(0, table.y + 2);

				std::cout << "Player 1 Win with score " << p1s;
				break;
			}
			++p2s;
		}
		Sleep(100);
		system("cls");
	}
	return 0;
}

void gotoXY(int x, int y)
{
	static HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole != INVALID_HANDLE_VALUE)
	{
		COORD pos = {(short)x,(short)y};
		SetConsoleCursorPosition(hConsole,pos);
	}
}

void setColor(WORD color)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &info);
	SetConsoleTextAttribute(hConsole,color);
	std::cout << ' ';
	SetConsoleTextAttribute(hConsole, info.wAttributes);
}

int Lerp(int a, int b, double t)
{
	return a + round(t * ((double)b - (double)a));
}

void drawBoder(const Vec2& table)
{
	std::string border(table.x, '=');
	gotoXY(0, 0);
	std::cout << border;
	gotoXY(0, table.y+1);
	std::cout << border;
}
