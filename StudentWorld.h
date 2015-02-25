#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <list>


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Player;
class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	~StudentWorld();

	virtual int init();
	virtual int move();
	virtual void cleanUp();

	void destroyActor(Actor* a);
	Player* getPlayer() const;

	std::list<Actor*> getActorAt(const int x, const int y) const;

	void addActor(Actor* a);
	void decJewelCount();
	void incJewelCount();
	int getJewelCount();
private:
	std::list<Actor*> m_actors;




	Player* m_player;
	int m_bonus;
	int m_jewelCount;

	void updateText();
};

#endif // STUDENTWORLD_H_
