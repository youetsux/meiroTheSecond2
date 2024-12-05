# include <Siv3D.hpp> // Siv3D v0.6.14
#include <numeric>
#include <stack>
#include <queue>
#include <map>

const Size BLOCK_SIZE{ 32, 32 };
const Size MazeSize{ 25, 19 };//サイズは奇数でなければいけない
const Point Start{ 1,1 };
const Point Goal{ MazeSize.x - 2,MazeSize.y - 2 };


enum OBJS
{
	WALL, BAR, FLOOR, START, GOAL, MAX_OBJS
};

struct floorData
{
	Vec2 pos;
	OBJS type;
	int  weight;
};

//二次元配列　MazeData[MazeSize.y][MazeSize.x]{FLOORで初期化｝
std::vector<std::vector<floorData>> MazeData(MazeSize.y, std::vector<floorData>(MazeSize.x, {{0,0},FLOOR, 0}));
//std::vector<std::vector<int>> MazeDataForSearch(MazeSize.y, std::vector<int>(MazeSize.x, FLOOR));
//std::vector<std::vector<int>> MazeDataForDraw(MazeSize.y, std::vector<int>(MazeSize.x, FLOOR));
//std::vector<std::vector<int>> MazeDataForDigDraw(MazeSize.y, std::vector<int>(MazeSize.x, FLOOR));
std::stack<Point> prStack;





void MakeWall(int w, int h);
void MakeMaze(int w, int h);
void DrawMaze(int w, int h);

//void AllWall(int w, int h);
//void MakeMazeDigDug(int w, int h);
//void DigDug(int x, int y);

int tekazu = MAX_OBJS;
void DigDug(int x, int y)
{
	Point Dir[]{ {0,-1},{1, 0},{0, 1},{-1,0} };
	std::vector<int> dList;
	for (int i = 0; i < 4; i++) {
		//nextを0~3まで回してでたーを取得
		Point next = Point{ x, y } + Dir[i];
		Point nextNext = next + Dir[i];
		if (nextNext.x < 0 || nextNext.y < 0 || nextNext.x > MazeSize.x - 1 || nextNext.y > MazeSize.y - 1)
			continue;

		if (MazeData[nextNext.y][nextNext.x] == WALL && MazeData[next.y][next.x] == WALL)
		{
			dList.push_back(i);
		}
	}
	if (dList.empty())
	{
		return;
	}
	int nrand = rand() % dList.size();
	int tmp = dList[nrand];

	Point next = Point{ x, y } + Dir[tmp];
	Point nextNext = next + Dir[tmp];

	MazeData[next.y][next.x] = FLOOR;
	MazeData[nextNext.y][nextNext.x] = FLOOR;
	MazeDataForDigDraw[next.y][next.x] = tekazu;
	MazeDataForDigDraw[nextNext.y][nextNext.x] = tekazu;
	tekazu++;
	prStack.push(nextNext);
	DigDug(nextNext.x, nextNext.y);
}


void AllWall(int w, int h)
{
	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++) {
			if (i == 0 || j == 0 || i == w - 1 || j == h - 1)
				MazeData[j][i] = FLOOR;
			else
				MazeData[j][i] = WALL;
		}
	}
	MazeDataForDigDraw = MazeData;
}

void MakeMazeDigDug(int w, int h)
{
	AllWall(w, h);
	Point sp{ 1, 1 };
	prStack.push(sp);
	while (!prStack.empty())
	{
		sp = prStack.top();
		prStack.pop();
		DigDug(sp.x, sp.y);
	}
}

//w, h 迷路の幅と高さ
void MakeMaze(int w, int h)
{

	MakeWall(w, h);
	PushDownBar(w, h);
}




void MakeWall(int w, int h)
{
	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			if (i == 0 || j == 0 || i == w - 1 || j == h - 1)
				MazeData[j][i] = WALL;
			continue;
		}
	}
}

void SetSG(int w, int h)
{
	MazeDataForSearch = MazeData; //元データは取っておく
	MazeDataForDraw = MazeData;
	MazeData[Start.y][Start.x] = START;
	MazeData[Goal.y][Goal.x] = GOAL;
}

void PushDownBar(int w, int h)
{
	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			if (!(i == 0 || j == 0 || i == w - 1 || j == h - 1))
			{
				if (i % 2 == 0 && j % 2 == 0)
				{
					MazeData[j][i] = BAR;
					//棒を立てたらすかさず倒す
					int rnd;
					if (j == 2)
						rnd = rand() % 4;//0:↑　1:→　2:↓　3:←
					else
						rnd = rand() % 3 + 1;
					Vec2 Dir[]{ {0,-1},{1, 0},{0, 1},{-1,0} };
					if (MazeData[j + (int)Dir[rnd].y][i + (int)Dir[rnd].x] != BAR)
						MazeData[j + (int)Dir[rnd].y][i + (int)Dir[rnd].x] = BAR;
					else
					{
						int rnd1 = (rnd + 1) % 3 + 1;
						int rnd2 = (rnd + 2) % 3 + 1;
						if (MazeData[j + (int)Dir[rnd1].y][i + (int)Dir[rnd1].x] != BAR)
							MazeData[j + (int)Dir[rnd1].y][i + (int)Dir[rnd1].x] = BAR;
						else
							MazeData[j + (int)Dir[rnd2].y][i + (int)Dir[rnd2].x] = BAR;
					}

				}
			}
		}
	}
}

void DrawMaze(int w, int h)
{
	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			Color col[MAX_OBJS]{ Palette::Firebrick,  Palette::Darkorange, Palette::Black,Palette::Yellow,Palette::Yellow };
			//if (MazeData[j][i] != FLOOR)
			Rect{ i * BLOCK_SIZE.x, j * BLOCK_SIZE.y, BLOCK_SIZE }.draw(col[MazeData[j][i]]);
			if (MazeData[j][i] == START) {
				Rect{ i * BLOCK_SIZE.x, j * BLOCK_SIZE.y, BLOCK_SIZE }.drawFrame(1, 1, col[MazeData[j][i]]);
			}

			if (MazeData[j][i] == GOAL) {
				Rect{ i * BLOCK_SIZE.x, j * BLOCK_SIZE.y, BLOCK_SIZE }.drawFrame(1, 1, col[MazeData[j][i]]);
			}

		}
	}
}

void DrawRoute(int w, int h)
{
	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			Color col[MAX_OBJS]{ Palette::Firebrick,  Palette::Darkorange, Palette::Black,Palette::Black,Palette::Black };
			//if (MazeData[j][i] != FLOOR)

			if (MazeDataForSearch[j][i] >= MAX_OBJS) {
				Rect rec = Rect{ i * BLOCK_SIZE.x, j * BLOCK_SIZE.y, BLOCK_SIZE };
				rec.drawFrame(1, 1, Palette::Red);
				FontAsset(U"NUM")(ToString(MazeDataForSearch[j][i] - MAX_OBJS))
					.drawAt(i * BLOCK_SIZE.x + BLOCK_SIZE.x / 2, j * BLOCK_SIZE.y + BLOCK_SIZE.y / 2, Palette::Black);
			}
			else
			{
				Rect{ i * BLOCK_SIZE.x, j * BLOCK_SIZE.y, BLOCK_SIZE }.draw(col[MazeDataForSearch[j][i]]);
			}

		}
	}
}

std::list<std::pair<Point, int>> animList;
std::queue<std::pair<Point, int>> myQueue;
std::stack<std::pair<Point, int>> myStack;
void BFS()
{
	Point np[4]{ {0,-1},{1, 0},{0, 1},{-1,0} };
	int step = MAX_OBJS;
	myQueue.push({ Start, step });
	MazeDataForSearch[Start.y][Start.x] = step;
	//animList.push_back({ Start, step });
	while (!myQueue.empty())
	{
		std::pair<Point, int> Crr = myQueue.front();
		myQueue.pop();
		animList.push_back({ Crr.first, Crr.second + 1 });
		for (int i = 0; i < 4; i++)
		{
			Point tmp = Crr.first + np[i];
			if (tmp.x < 0 || tmp.y <0 || tmp.x > MazeSize.x - 1 || tmp.y > MazeSize.y - 1)
				continue;
			if (MazeDataForSearch[tmp.y][tmp.x] == FLOOR) {
				MazeDataForSearch[tmp.y][tmp.x] = Crr.second + 1;
				myQueue.push({ tmp, Crr.second + 1 });
			}
		}
	}
}

void DFS()
{
	Point np[4]{ {0,-1},{1, 0},{0, 1},{-1,0} };
	int step = MAX_OBJS;
	myStack.push({ Start, step });
	MazeDataForSearch[Start.y][Start.x] = step;
	//animList.push_back({ Start, step });
	while (!myStack.empty())
	{
		std::pair<Point, int> Crr = myStack.top();
		myStack.pop();
		animList.push_back({ Crr.first, Crr.second + 1 });
		for (int i = 0; i < 4; i++)
		{
			Point tmp = Crr.first + np[i];
			if (tmp.x < 0 || tmp.y <0 || tmp.x > MazeSize.x - 1 || tmp.y > MazeSize.y - 1)
				continue;
			if (MazeDataForSearch[tmp.y][tmp.x] == FLOOR) {
				MazeDataForSearch[tmp.y][tmp.x] = Crr.second + 1;
				myStack.push({ tmp, Crr.second + 1 });

			}
		}
	}
}

bool DrawSearchAnimFrame(bool& next)
{
	if (next && !animList.empty()) {
		auto dat = animList.front();
		animList.pop_front();
		MazeDataForDraw[dat.first.y][dat.first.x] = dat.second;
		next = false;
	}
	for (int j = 0; j < MazeSize.y; j++)
	{
		for (int i = 0; i < MazeSize.x; i++)
		{
			//WALL, BAR, FLOOR, START, GOAL, MAX_OBJS
			Color col[MAX_OBJS]{ Palette::Firebrick,  Palette::Darkorange, Palette::Black, Palette::Yellow, Palette::Yellow };

			if (MazeDataForDraw[j][i] >= MAX_OBJS) {
				Rect rec = Rect{ i * BLOCK_SIZE.x, j * BLOCK_SIZE.y, BLOCK_SIZE };
				rec.drawFrame(1, 1, Palette::Red);
				//FontAsset(U"NUM")(ToString(MazeDataForSearch[j][i] - MAX_OBJS))
				//	.drawAt(i * BLOCK_SIZE.x + BLOCK_SIZE.x / 2, j * BLOCK_SIZE.y + BLOCK_SIZE.y / 2, Palette::Black);
				Rect{ i * BLOCK_SIZE.x, j * BLOCK_SIZE.y, BLOCK_SIZE }.draw(Palette::Yellow);
			}
			else
			{
				Rect{ i * BLOCK_SIZE.x, j * BLOCK_SIZE.y, BLOCK_SIZE }.draw(col[MazeDataForDraw[j][i]]);
			}

		}
	}
	if (animList.empty())
		return true;
	else
		return false;
}





void Main()
{
	FontAsset::Register(U"NUM", 14, Typeface::Medium);

	srand((unsigned int)time(nullptr));
	// 背景の色を設定する | Set the background color
	Scene::SetBackground(Palette::Cornflowerblue);


	//MakeMaze(MazeSize.x, MazeSize.y);
	MakeMazeDigDug(MazeSize.x, MazeSize.y);
	MakeWall(MazeSize.x, MazeSize.y);
	SetSG(MazeSize.x, MazeSize.y);

	//BFS();
	DFS();
	bool routeSwitch = false;
	bool routeORstep = false;
	while (System::Update())
	{
		if (routeORstep) {
			DrawRoute(MazeSize.x, MazeSize.y);
		}
		else {
			if (KeySpace.down())
				routeSwitch = true;
			routeORstep = DrawSearchAnimFrame(routeSwitch);
		}
		//if (KeySpace.pressed())
		//	DrawRoute(MazeSize.x, MazeSize.y);
		//else
		//	DrawMaze(MazeSize.x, MazeSize.y);

	}
}

