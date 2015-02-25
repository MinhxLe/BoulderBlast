#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
/*Every Actor is an objection on the screen
*It belongs to a world (and knows so)
*it can either be alive or not(in other words active)
*/
class Actor : public GraphObject{
public:
	Actor(int imageID, int x, int y, Direction d = none);//active automatically true
	virtual ~Actor();
	virtual void doSomething() = 0;

	virtual bool isAlive();
	virtual void die();

	void getAdjacentCoordinate(Direction d, int& x, int& y) const;

	static void setWorld(StudentWorld* s);
	static StudentWorld* getWorld();


private:
	static StudentWorld* m_world;
	bool m_alive;//ie not destroyed
};

class Entity : public Actor{
public:
	Entity(int imageID, int x, int y, Direction d = none, int h = 0);
	virtual ~Entity();
	virtual void doSomething();//try to die first, try to do action(), then move()
	
	bool moveDirection(Direction d);
	//two below are same for all entities, need to play sound unique to class
	virtual void takeDamage(int d);
	virtual bool isAlive();
	virtual void die();// like destroySelf // may not be necessary

	int getHealth() const;
	void setHealth(int h);

protected:
	virtual bool canMoveTo(int x, int y) const;
	virtual bool action() = 0;//the action an Entity does that makes it unique
	virtual bool move() = 0;//the move algorithm of Entity
private:	
	int m_health;
};

class violentEntity : public virtual Entity{
public:
	violentEntity(int imageID, int x, int y, 
		Direction d = none, int h = 0);
	virtual ~violentEntity();
protected:
	virtual bool action();//shoot //assumes no ammo
	virtual bool move() = 0;
};

class Player : public violentEntity{
public:
	Player(int x, int y);
	virtual ~Player();
	virtual void doSomething();
	
	void setAmmo(int a);
	int getAmmo() const;
	virtual void takeDamage(int d);
protected:
	virtual bool action();//shoot AND lose ammo
	virtual bool move();//move by keystroke
	virtual bool canMoveTo(int x, int y) const;//players can push boulder
private:
	int m_ammo;
};

class Wall : public Actor{
public:
	Wall(int x, int y);
	virtual ~Wall();
	virtual void doSomething();
private:
};


class Boulder : public Entity{
public:
	Boulder(int x, int y);
	virtual ~Boulder();
//	bool getPushed();//seperate method because being pushed is NOT a do somethign
protected:
	virtual bool canMoveTo( int x,  int y) const;
	virtual bool action();//fill a hole->kill a hole
	virtual bool move();//if pushed by player, try to move, if can't move then don't
};

class Hole : public Actor{
public:
	Hole(int x, int y);
	~Hole();
	virtual void doSomething();
private:
};

class Goodie : public Actor{
public:
	Goodie(int imageID, int x, int y);
	virtual ~Goodie();
	virtual void doSomething();//either gets picked up or stolen
	virtual void affectGame() = 0;
	void setActive(bool a);//active - can be collected, //inactive - cannot be collected
	bool getActive() const;
private:
	bool m_active;

};

class Jewel : public Goodie{//CANNOT BE STOLEN!
public:
	Jewel(int x, int y);
	virtual ~Jewel();
private:
	virtual void affectGame();
};
class LifeGoodie : public Goodie{//CANNOT BE STOLEN!
public:
	LifeGoodie(int x, int y);
	virtual ~LifeGoodie();
private:
	virtual void affectGame();

};

class HealthGoodie : public Goodie{//CANNOT BE STOLEN!
public:
	HealthGoodie(int x, int y);
	virtual ~HealthGoodie();
	virtual void affectGame();
private:
};

class AmmoGoodie : public Goodie{//CANNOT BE STOLEN!
public:
	AmmoGoodie(int x, int y);
	virtual ~AmmoGoodie();
	virtual void affectGame();
private:
};

class Exit : public Actor{
public:
	Exit(int x, int y);
	virtual ~Exit();
	virtual void doSomething();
private:
};

class Bullet
 :public  Entity{
public:
	Bullet(int x, int y, Direction d);
	virtual ~Bullet();
	int getDamage() const;
protected:
	virtual bool canMoveTo(int x, int y) const;
	virtual bool action();//keep flying till hit or out of bounds/wall
	virtual bool move();
private:
	int m_damage;
};

class SnarlBot : public violentEntity{
public:
	SnarlBot(int x, int y, Direction d);
	virtual ~SnarlBot();
		bool playerInLineOfSight() const;
	virtual bool action();//unlimited ammo?
	virtual bool move();
private:
	virtual void reverseDirection();
};

// class KleptoBot : virtual public Entity{
// public:
// 	KelptoBot(int x, int y);
// 	virtual ~KleptoBot();
// 	virtual void doSomething();
// 	virtual bool action();//steal, set goodie inactive
// 	virtual bool move();
// 	virtual void die();
// 	virtual void takeDamage();
// 	void drop();//drop item upon death, nive goodie to current location
// 	void setLoot(Goodie* g);
// 	Goodie* getLoot() const;
// private:
// 	Goodie* m_loot;
// };

// class AngryKleptoBot : public KleptoBot, public SnarlBot{
// public:
// 	AngryKleptoBot(int x, int y);
// 	virtual ~AngryKleptoBot();
// 	virtual void doSomething();
// 	virtual bool action();//attack, if not then steal, else do nothing
// 	virtual void die();
// 	virtual void takeDamage();
// private:
// };










// class Factory : public Actor{
// public:
// 	Factory(int x, int y);
// 	virtual ~Factory();
// 	virtual void doSomething();
// 	virtual void produceBot();
// private:
// };

// class AngryFactory : public Factory{
// public:
// 	AngryFactory(int x, int y);
// 	virtual ~AngryFactory();
// 	virtual void produceBot();
// private:
// };





#endif // ACTOR_H_
