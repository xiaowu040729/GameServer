#include "AOI.h"


/*初始化*/
AOIWORLD::AOIWORLD(int xbegin, int xend, int ybegin, int yend, int xcount, int ycount) :w_xbegin(xbegin), w_xend(xend), w_ybegin(ybegin), w_yend(yend), x_count(xcount), y_count(ycount)
{
	
	x_width =(w_xend - w_xbegin) / x_count ;
	y_width = (w_yend - w_ybegin) / y_count;
	
	/*创建格子*/
	for (int i = 1; i <= x_count * y_count; i++)
	{
		Grid *grid =  new Grid();
		AOIWorld.push_back(grid);
	}
}

AOIWORLD::~AOIWORLD()
{
	for (auto *single : AOIWorld)
	{
		delete single;
	}
}

/*添加玩家*/
bool AOIWORLD::AddPlayer(Player* _player)
{
	/*计算玩家位置*/
	int grid = (_player->GetX() - w_xbegin) / x_width + ((_player->GetY() - w_ybegin) / y_width) * x_count;
	/*把玩家加入AOIWorld中*/
	AOIWorld[grid]->players.push_back(_player);
	return true;
}

/*删除玩家*/
void AOIWORLD::DeletePlayer(Player* _player)
{
	int grid = (_player->GetX() - w_xbegin) / x_width + ((_player->GetY() - w_ybegin) / y_width) * x_count;
	/*把玩家加入AOIWorld中*/
	AOIWorld[grid]->players.remove(_player);
}

/*显示周围玩家*/
list<Player*> AOIWORLD::SurroundPlayers(Player * _player)
{
	list<Player*> surround_players;
	/*计算玩家所在的格子*/
	int grid = (_player->GetX() - w_xbegin) / x_width + ((_player->GetY() - w_ybegin) / y_width) * x_count;
	
	/*玩家所在格子的x的格子的数量和y格子的数量*/
	int x_index = grid % x_count;
	int y_index = grid / x_count;

	/*判断格子是否是在特殊位置如：左上角，左下角，右上角，右下角*/
	/*该格子的左上角是合法的*/
	if (x_index > 0 && y_index > 0)		
	{
		/*获取周围玩家 并且把玩家放入一个list中*/
		list<Player*> &tmp = AOIWorld[grid - x_count - 1]->players;
		surround_players.insert(surround_players.begin(),tmp.begin(),tmp.end() );
	}

	/*头顶上的格子是合法的*/
	if (y_index > 0)		
	{
		list<Player*>& tmp = AOIWorld[grid - x_count]->players;
		surround_players.insert(surround_players.begin(), tmp.begin(), tmp.end());
	}

	/*右上角格子是合法的*/
	if (x_index < x_count - 1 && y_index > 0)
	{
		list<Player*>& tmp = AOIWorld[grid - x_count + 1]->players;
		surround_players.insert(surround_players.begin(), tmp.begin(), tmp.end());
	}

	/*左边的格子是合法的*/
	if (x_index > 0)	
	{
		list<Player*>& tmp = AOIWorld[grid - 1]->players;
		surround_players.insert(surround_players.begin(), tmp.begin(), tmp.end());
	}

	/*玩家自身的格子是合法的*/
	list<Player*>& tmp = AOIWorld[grid]->players;
	surround_players.insert(surround_players.begin(), tmp.begin(), tmp.end());

	/*玩家右边的格子是合法的*/
	if (x_index < x_count - 1)		
	{
		list<Player*>& tmp = AOIWorld[grid + 1]->players;
		surround_players.insert(surround_players.begin(), tmp.begin(), tmp.end());
	}

	/*玩家左下角的格子是合法的*/
	if (x_index > 0 && y_index < y_count - 1)		
	{
		list<Player*>& tmp = AOIWorld[grid + x_count - 1]->players;
		surround_players.insert(surround_players.begin(), tmp.begin(), tmp.end());
	}

	/*玩家下方的格子是合法的*/
	if (y_index < y_count - 1)		
	{
		list<Player*>& tmp = AOIWorld[grid + x_count]->players;
		surround_players.insert(surround_players.begin(), tmp.begin(), tmp.end());
	}

	/*玩家右下角的格子是合法的*/
	if (x_index < x_count - 1 && y_index < y_count - 1)			
	{
		list<Player*>& tmp = AOIWorld[grid + x_count + 1]->players;
		surround_players.insert(surround_players.begin(), tmp.begin(), tmp.end());
	}
	return surround_players;
}

