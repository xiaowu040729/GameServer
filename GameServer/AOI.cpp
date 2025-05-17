#include "AOI.h"



AOIWORLD::AOIWORLD(int xbegin, int xend, int ybegin, int yend, int xcount, int ycount) :w_xbegin(xbegin), w_xend(xend), w_ybegin(ybegin), w_yend(yend), x_count(xcount), y_count(ycount)
{
	
	x_width =(w_xend - w_xbegin) / x_count ;
	y_width = (w_yend - w_ybegin) / y_count;
	
	/*创建格子*/
	for (int i = 1; i <= x_count * y_count; i++)
	{
		Grid *grid;
		AIOWorld.push_back(grid);
	}
}

AOIWORLD::~AOIWORLD()
{
}

bool AOIWORLD::AddPlayer(Player* _player)
{
	/*计算玩家位置*/
	int grid = (_player->GetX() - w_xbegin) / x_width + (_player->GetY()) / y_width * x_count; 
	/*把玩家加入AIOWorld中*/
	AIOWorld[grid]->players.push_back(_player);
	return true;
}

void AOIWORLD::DeletePlayer(Player* _player)
{
	int grid = (_player->GetX() - w_xbegin) / x_width + (_player->GetY()) / y_width * x_count;
	/*把玩家加入AIOWorld中*/
	AIOWorld[grid]->players.remove(_player);
}

list<Player*> AOIWORLD::SurroundPlayers(Player * _player)
{
	/*计算玩家所在的格子*/
	int grid = (_player->GetX() - w_xbegin) / x_width + (_player->GetY()) / y_width * x_count;

	/*同时盘算周围的格子是否合法*/
	 
	/*确定周围玩家*/
	return ;
}
  