#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <list>
#include <iostream>
using namespace std;
////////////ACTOR CLASS//////////
Actor::Actor(int imageID, int x, int y, Direction d)
:GraphObject(imageID, x, y, d){
	m_alive = true;
	setVisible(true);
}
Actor::~Actor(){}
bool Actor::isAlive(){
	return m_alive;
}
void Actor::die(){
	//getWorld()->destroyActor(this);
	m_alive = false;
}

void Actor::getAdjacentCoordinate(Direction d, int& x, int& y)const {
	switch(d){
		case(up):
			x = getX();
			y = getY()+1;
			break;
		case(down):
			x = getX();
			y = getY();
			break;
		case(left):
			y = getY();
			x = getX() - 1;
			break;
		case(right):
			y = getY();
			x = getX() + 1;
			break;
		default:
			break;
	}
}

void Actor::setWorld(StudentWorld* s){
	m_world = s;
}
StudentWorld* Actor::getWorld(){
	return m_world;
}

StudentWorld* Actor::m_world = nullptr;




//////////ENTITY CLASS//////////
Entity::Entity(int imageID, int x, int y, Direction d, int h)
:Actor(imageID, x, y, d){
	m_health = h;
}
Entity::~Entity(){}
void Entity::doSomething(){
	if (!isAlive())
		die();
	else if (!action())
		move();
}
void Entity::takeDamage(int d){
	setHealth(getHealth() - d);
}
bool Entity::isAlive(){
	return m_health > 0;
}
void Entity::die(){
	//play a sound
	setHealth(0);
	Actor::die();//unnecessary
}


bool Entity::canMoveTo(int x, int y) const{//empty
	//boundary outside
	if (x < 0 && x >= VIEW_WIDTH && y < 0 && y >= VIEW_HEIGHT)
		return false;
	//invalid objects to walk through
	list<Actor*> at = Actor::getWorld()->getActorAt(x,y);
	for (Actor* a : at){
		int id = a->getID();
		if (id != IID_AMMO && id != IID_JEWEL && id != IID_RESTORE_HEALTH
			&& id != IID_EXTRA_LIFE && id != IID_EXIT && id != IID_BULLET)
			return false; 
	}
	return true;
}



bool Entity::moveDirection(Direction d){//change method isMovableTo
    setDirection(d);
    int x, y;
    getAdjacentCoordinate(d, x,y);
    if (canMoveTo(x, y)){
        moveTo(x,y);
       	return true;
    }

    return false;
}

int Entity::getHealth() const{
	return m_health;
}
void Entity::setHealth(int h){
	m_health = h;
}

//////////VIOLENT CLASS//////////
violentEntity::violentEntity(int imageID, int x, int y, Direction d, int h)
:Entity::Entity(imageID, x, y, d, h){}
violentEntity::~violentEntity(){}
bool violentEntity::action(){
			switch(getDirection()){
        case up:
           	getWorld()->addActor(new Bullet(getX(), getY()+1,getDirection()));
            break;
        case down:
        	getWorld()->addActor(new Bullet(getX(), getY()-1,getDirection()));
        	break;
        case left:
            getWorld()->addActor(new Bullet(getX()-1, getY(),getDirection()));
            break;
        case right:
           getWorld()->addActor(new Bullet(getX()+1, getY(),getDirection()));
            break;
        default:
            break;
     };
	return true;
}
//////////PLAYER CLASS//////////
Player::Player(int x, int y)
:Entity(IID_PLAYER,x,y,right,20),violentEntity(IID_PLAYER, x, y, right, 20)
{
	m_ammo = 20;
}
Player::~Player(){}
void Player::doSomething(){
	            int ch;
            if (getWorld()->getKey(ch)){
                switch(ch){
                    case KEY_PRESS_LEFT:
                        moveDirection(left);
                        break;
                    case KEY_PRESS_RIGHT:
                        moveDirection(right);
                        break;
                    case KEY_PRESS_UP:
                        moveDirection(up);
                        break;
                    case KEY_PRESS_DOWN:
                        moveDirection(down);
                        break;
                    case KEY_PRESS_SPACE:
                        action();
                        break;
                    case KEY_PRESS_ESCAPE:
                        die();
                        break;
                    default:
                        break;
                };
	}
}	
void Player::setAmmo(int a){
	m_ammo = a;
}
int Player::getAmmo() const{
	return m_ammo;
}
void Player::takeDamage(int d){
	getWorld()->playSound((SOUND_PLAYER_IMPACT));
	Entity::takeDamage(d);
}
bool Player::action(){//shoot AND lose ammo//IMPLEMENT
	if (m_ammo > 0){
		setAmmo(m_ammo-1);
	violentEntity::action();	
	getWorld()->playSound(SOUND_PLAYER_FIRE);
	return true;
	}
	return false;
}
 bool Player::move(){return true;}//?

 bool Player::canMoveTo(int x, int y) const{
	//boundary
	if (x < 0 && x >= VIEW_WIDTH && y < 0 && y >= VIEW_HEIGHT)
		return false;
	//invalid objects to walk through
	list<Actor*> at = getWorld()->getActorAt(x,y);
	for (Actor* a : at){
		int id = a ->getID();
		if (id != IID_AMMO && id != IID_JEWEL && id != IID_RESTORE_HEALTH
			&& id != IID_EXTRA_LIFE && id != IID_EXIT && id != IID_BULLET
			&& id != IID_BOULDER)
			return false;
		if (id == IID_BOULDER){
			bool x = dynamic_cast<Boulder*>(a)->moveDirection(getDirection());//move boulder in direction
			a->setDirection(none);//resets direction
			return x;
		}
	}
	return true;
 }

//////////WALL CLASS//////////
Wall::Wall(int x, int y)
	:Actor(IID_WALL, x, y, none){}
Wall::~Wall(){}
void Wall::doSomething(){}

//////////BOULDER CLASS//////////
Boulder::Boulder(int x, int y)
:Entity(IID_BOULDER, x, y, none, 10){}
Boulder::~Boulder(){}


bool Boulder::action(){
// fill a hole 
	list<Actor*> at = Actor::getWorld()->getActorAt(getX(), getY());
	for (Actor* a : at){
		if (a->getID() == IID_HOLE){
			a->die();
			setHealth(0);//HACKS
			//this->die();
			return true;
		}
	}
	//if current positition is hole, destroy hole and self
	//return true
	return false;
}
bool Boulder::move(){return false;}//boulders can't move on their own(duh)
bool Boulder::canMoveTo( int x,  int y) const{
	if (x < 0 && x >= VIEW_WIDTH && y < 0 && y >= VIEW_HEIGHT)
		return false;
	//invalid objects to walk through
	list<Actor*> at = getWorld()->getActorAt(x,y);
	for (Actor* a : at){
		int id = a ->getID();
		if (id != IID_AMMO && id != IID_JEWEL && id != IID_RESTORE_HEALTH
			&& id != IID_EXTRA_LIFE && id != IID_EXIT && id != IID_BULLET
			&& id != IID_HOLE)
			return false;
		
	}
	return true;
}

//////////HOLE CLASS//////////
Hole::Hole(int x, int y)
	:Actor(IID_HOLE, x, y, none){}
Hole::~Hole(){}
void Hole::doSomething(){}

//////////GOODIE CLASS//////////
Goodie::Goodie(int imageID, int x, int y)
:Actor(imageID,x, y, none){
	m_active = true;
}
Goodie::~Goodie(){}
void Goodie::doSomething(){
	if (!getActive())
		setVisible(false);
	else{
		if (getX() == getWorld()->getPlayer()->getX() &&
			getY() == getWorld()->getPlayer()->getY()){//player on it
			getWorld()->playSound(SOUND_GOT_GOODIE);
			affectGame();
			die();
		}
	}
}
bool Goodie::getActive() const{
	return m_active;
}
void Goodie::setActive(bool a){
	m_active = a;
}//active - can be collected, //inactive - cannot be collected

//////////JEWEL CLASS//////////
Jewel::Jewel(int x, int y)
:Goodie(IID_JEWEL, x, y){
	getWorld()->incJewelCount();
}
Jewel::~Jewel(){}
void Jewel::affectGame(){
	getWorld()->increaseScore(50);
	getWorld()->decJewelCount();
}
//////////LifeGoodie CLASS//////////
LifeGoodie::LifeGoodie(int x, int y)
:Goodie(IID_EXTRA_LIFE, x, y){}
LifeGoodie::~LifeGoodie(){}
void LifeGoodie::affectGame(){
	getWorld()->incLives();	
	getWorld()->increaseScore(1000);
}
//////////HealthGoodie CLASS//////////
HealthGoodie::HealthGoodie(int x, int y)
:Goodie(IID_RESTORE_HEALTH, x, y){}
HealthGoodie::~HealthGoodie(){}
void HealthGoodie::affectGame(){
	getWorld()->getPlayer()->setHealth(20);//max health
	getWorld()->increaseScore(20);
}
//////////AMMOGOODIE CLASS//////////
AmmoGoodie::AmmoGoodie(int x, int y)
:Goodie(IID_AMMO, x, y){}
AmmoGoodie::~AmmoGoodie(){}
void AmmoGoodie::affectGame(){
	Player* m_player = getWorld()->getPlayer();
	m_player->setAmmo(m_player->getAmmo()+20);//MOAR AMMOLOL
	getWorld()->increaseScore(20);
}
//////////EXIT CLASS//////////

Exit::Exit(int x, int y)
:Actor(IID_EXIT, x, y, none){
	setVisible(false);
}
Exit::~Exit(){}

void Exit::doSomething(){
	if (getWorld()->getJewelCount() <= 0){
		getWorld()->playSound(SOUND_REVEAL_EXIT);
		setVisible(true);
		
	}
	if (isVisible()){
	Player* m_player = getWorld()->getPlayer();
	if (getX() == m_player->getX() && getY() == m_player->getY()){
		getWorld()->playSound(SOUND_FINISHED_LEVEL);
		getWorld()->increaseScore(2000);
		die(); 
	}
}
}

Bullet::Bullet(int x, int y, Direction d)
:Entity(IID_BULLET, x, y, d, 1){
	if (x < 0 && x >= VIEW_WIDTH && y < 0 && y >= VIEW_HEIGHT)
		die();
	m_damage = 2;
}
//////BULLET CLASS//////////
Bullet::~Bullet(){}
int Bullet::getDamage() const{
	return m_damage;
}
bool Bullet::action(){
	std::list<Actor*> act = getWorld()->getActorAt(getX(), getY());
	for (Actor* a : act){
		if (a->getID() == IID_BOULDER || 
			a->getID() == IID_PLAYER ||
			a->getID() == IID_SNARLBOT ||
			a->getID() == IID_KLEPTOBOT){
			static_cast<Entity*>(a)->takeDamage(getDamage());
			setHealth(0);
			return true;
		}
		if(a->getID() == IID_WALL || a->getID() == IID_ROBOT_FACTORY){
			setHealth(0);
			return true;
		}
	}
	return false;
}

bool Bullet::move(){
	if (moveDirection(getDirection()))
		return true;
	else
		die();
		return false;
}
bool Bullet::canMoveTo(int x, int y) const{
	if (x < 0 && x >= VIEW_WIDTH && y < 0 && y >= VIEW_HEIGHT)
		return false;

	list<Actor*> at = getWorld()->getActorAt(x,y);
	for (Actor* a : at){
		int id = a ->getID();
		if (id == IID_WALL)
			return false;
		
	}
	return true;
}

SnarlBot::SnarlBot(int x, int y, Direction d)
:Entity(IID_SNARLBOT, x, y,d),violentEntity(IID_SNARLBOT,x,y, d,10)
{
	setVisible(true);
}

SnarlBot::~SnarlBot(){}

bool SnarlBot::action(){
	if (playerInLineOfSight())
		return violentEntity::action();
	else
		return false;
}

bool SnarlBot::move(){
	if (moveDirection(getDirection()))
		return true;
	else
		reverseDirection();
	return true;
}

void SnarlBot::reverseDirection(){
	switch(getDirection()){
			case(up):
				setDirection(down);
				break;
			case (down):
				setDirection(up);
				break;
			case(left):
				setDirection(right);
				break;
			case(right):
				setDirection(left);
				break;
			default:
				break;
		};
}

bool SnarlBot::playerInLineOfSight() const{
	int playerX = getWorld()->getPlayer()->getX()
	, playerY = getWorld()->getPlayer()->getY();

	int currX, currY;

	do{
		getAdjacentCoordinate(getDirection(),currX, currY);
		if (currX == playerX && currY == playerY)
			return true;
		else{
			list<Actor*> act = getWorld()->getActorAt(currX, currY);
			for (Actor* a : act){
				if (a->getID() == IID_BOULDER || a->getID() == IID_WALL
					|| a->getID() == IID_KLEPTOBOT || a->getID() == IID_SNARLBOT)
					return false;
			}

		}
	}
	while (currX >0 && currX < VIEW_WIDTH && currY > 0 && currY < VIEW_HEIGHT);
}
