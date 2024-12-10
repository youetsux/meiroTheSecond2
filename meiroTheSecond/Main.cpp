# include <Siv3D.hpp> // Siv3D v0.6.14
#include <numeric>
#include <stack>
#include <queue>
#include <map>

using std::pair;

using dat = pair<int, Vec2>;


const Size BLOCK_SIZE{ 32, 32 };
const Size MazeSize{ 25, 19 };//サイズは奇数でなければいけない
const Point Start{ 1,1 };
const Point Goal{ MazeSize.x - 2,MazeSize.y - 2 };

const Vec2 dirs[4]{ {0,1},{0,-1},{1,0},{-1,0} };

enum OBJS
{
	WALL, BAR, FLOOR, START, GOAL, MAX_OBJS
};

struct floorData
{
	OBJS type;
	int  weight;
};



//二次元配列　MazeData[MazeSize.y][MazeSize.x]{FLOORで初期化｝
std::vector<std::vector<floorData>> MazeData(MazeSize.y, std::vector<floorData>(MazeSize.x, {FLOOR, 0}));
std::vector<std::vector<int>> dist(MazeSize.y, std::vector<int>(MazeSize.x, INT_MAX));
std::vector<std::vector<Vec2>> pre(MazeSize.y, std::vector<Vec2>(MazeSize.x, {0, 0}));
//std::stack<Point> prStack;



void MakeWall();
void MakeMaze();
void DrawMaze();

int tekazu = MAX_OBJS;

void MakeMaze()
{
	for (int j = 0; j < MazeSize.y; j++)
	{
		for (int i = 0; i < MazeSize.x; i++)
		{
			if (i == 0 || j == 0 || i == MazeSize.x - 1 || j == MazeSize.y - 1)
				continue;
			MazeData[j][i].weight = rand() % 10 + 1;
		}
	}
}


void MakeWall()
{
	for (int j = 0; j < MazeSize.y; j++)
	{
		for (int i = 0; i < MazeSize.x; i++)
		{
			if (i == 0 || j == 0 || i == MazeSize.x - 1 || j == MazeSize.y - 1)
				MazeData[j][i].type = WALL;
			continue;
		}
	}
	int bars[2]{ MazeSize.x * 1.0 / 3.0, MazeSize.x * 2 / 3 };

	for (int j = 0; j < MazeSize.y * (2.0 / 3.0); j++)
	{
		for (int i = 0; i < 2; i++)
		{
			int m = j;
			if (i == 1)
				m = MazeSize.y - j -1;
			MazeData[m][bars[i]].type = WALL;
		}
	}
}

void SetSG()
{
	//MazeDataForSearchMazeData; //元データは取っておく
	MazeData[Start.y][Start.x].type = START;
	MazeData[Goal.y][Goal.x].type = GOAL;
}


void DrawMaze()
{
	for (int j = 0; j < MazeSize.y; j++)
	{
		for (int i = 0; i < MazeSize.x; i++)
		{
			bool drawFrame = false;
			Color col = Color{ 0,0,0 };
			switch (MazeData[j][i].type)
			{
			case WALL:
				col = Palette::Firebrick;
				break;
			case FLOOR:
				col = Color{ 0, (unsigned char)(255.0 * (MazeData[j][i].weight / 10.0)), 0 };
				break;
			case START:
			case GOAL:
				col = Palette::Yellow;
				drawFrame = true;
				break;
			case BAR:
			default:
				col = Palette::Black;
			}
			Rect{ i * BLOCK_SIZE.x, j * BLOCK_SIZE.y, BLOCK_SIZE }.draw(col);
			if(drawFrame)
				Rect{ i * BLOCK_SIZE.x, j * BLOCK_SIZE.y, BLOCK_SIZE }.drawFrame(1, 1, Palette::Red);
			if(dist[j][i] < INT_MAX)
			FontAsset(U"NUM")(ToString(dist[j][i]))
				.drawAt(i * BLOCK_SIZE.x + BLOCK_SIZE.x / 2, j * BLOCK_SIZE.y + BLOCK_SIZE.y / 2, Palette::Black);
		}
	}
}
using vecInt = std::pair<int,int>;
using Mdat = std::pair<int, vecInt>;


void Dijkstra(pair<int, int> sp)
{
	dist[sp.second][sp.first] = 0;
	std::priority_queue<Mdat, std::vector<Mdat>, std::greater<Mdat>> pq;
	pq.push(Mdat(0, { sp.first, sp.second }));

	while (!pq.empty())
	{
		Mdat p = pq.top();
		pq.pop();

		Rect{ (int)p.second.first * BLOCK_SIZE.x, (int)p.second.second * BLOCK_SIZE.y, BLOCK_SIZE }.draw(Palette::Red);
		getchar();
		int c = p.first;
		vecInt v = p.second;

		for (int i = 0; i < 4; i++)
		{
			vecInt np = { v.first + (int)dirs[i].x, v.second + (int)dirs[i].y };
			if (np.first < 0 || np.second < 0 || np.first >= MazeSize.x || np.second >= MazeSize.y) continue;
			if (MazeData[np.second][np.first].type == WALL) continue;
			if (dist[np.second][np.first] <= MazeData[np.second][np.first].weight + c) continue;
			dist[np.second][np.first] = MazeData[np.second][np.first].weight + c;
			pq.push(Mdat(dist[np.second][np.first], np));
		}
	}

}



void Main()
{
	FontAsset::Register(U"NUM", 14, Typeface::Medium);

	srand((unsigned int)time(nullptr));
	// 背景の色を設定する | Set the background color
	Scene::SetBackground(Palette::Cornflowerblue);

	MakeMaze();
	MakeWall();
	SetSG();

	Dijkstra({ Start.x, Start.y });

	while (System::Update())
	{
		DrawMaze();

	}
}

