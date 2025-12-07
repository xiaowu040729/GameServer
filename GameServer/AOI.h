#pragma once
#include<iostream>
#include<list>
#include<vector>
using namespace std;
/*玩家*/
class Player {
public:
	/*返回坐标位置*/
	virtual int GetX() = 0;
	virtual int GetY() = 0;
	
private:
	int x;
	int y;
};



class Grid {
public:
	int x;
	int y;
	/*一个格子里可能有多个玩家*/
	list<Player*> players;
private:
	
};

/*Area Of Intrest算法*/
/*可以看做一块地图*/
class AOIWORLD
{
private:
	/*地图的相关细节*/
	int w_xbegin;
	int w_xend;
	int w_ybegin;
	int w_yend;
	int x_count;
	int y_count;
	int x_width;
	int y_width;
public:
	AOIWORLD(int xbegin,int xend,int ybegin,int yend,int xcount,int ycount);
	~AOIWORLD();
	/*一块地图里有多个格子*/
	vector<Grid*> AOIWorld;
	
	/*添加玩家*/
	bool AddPlayer(Player * _player);

	/*删除玩家*/
	void DeletePlayer(Player * _player);

	/*获取周围玩家*/
	list<Player *> SurroundPlayers(Player* _player);

	/**/
};

