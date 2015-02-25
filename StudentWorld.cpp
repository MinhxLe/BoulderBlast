#include "StudentWorld.h"
#include "Actor.h"
#include "Level.h"
#include <string>
#include <list>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cassert>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}
StudentWorld::StudentWorld(string assetDir)
:GameWorld(assetDir){
	Actor::setWorld(this);
}
StudentWorld::~StudentWorld(){
        list<Actor*>::iterator it = m_actors.begin();
        while (it != m_actors.end()){
            delete *it;
            it = m_actors.erase(it);
        }
        delete m_player;
}
int StudentWorld::init(){
 	ostringstream lvl;
    lvl << "level" << setw(2) << setfill ('0') << getLevel() << ".dat";
    Level lev(assetDirectory());
    Level::LoadResult result = lev.loadLevel(lvl.str());
    //if level file was corrupt or not found
    if (result == Level::load_fail_file_not_found ||
            result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    //LOADING LEVEL
    
    //loading game actors and prviate data
    m_bonus = 1000;
    m_jewelCount = 0;
    for (int x = 0; x < VIEW_WIDTH; x++)
	{
    for (int y = 0; y < VIEW_HEIGHT; y++) //CHECK IF RIGHT**WARNING PG. 14
        {
            switch(lev.getContentsOf(x,y))
            {
            case (Level::player):
                m_player = new Player(x,y);
                break;
            case (Level::wall):
            	m_actors.push_back(new Wall(x,y));
            	break;
            case (Level::boulder):
            	m_actors.push_back(new Boulder(x,y));
            	break;
            case (Level::hole):
            	m_actors.push_back(new Hole(x,y));
            	break;
            case (Level::jewel):
                m_actors.push_back(new Jewel(x,y));
                break;
            case (Level::extra_life):
                m_actors.push_back(new LifeGoodie(x,y));
                break;
            case (Level::restore_health):
                m_actors.push_back(new HealthGoodie(x,y));
                break;
            case (Level::ammo):
                m_actors.push_back(new AmmoGoodie(x,y));
                break;
            case (Level::exit):
                m_actors.push_back(new Exit(x,y));
                break;
                // case (Level::horiz_snarlbot):
                // m_actors.push_back(new SnarlBot(x,y,GraphObject::Direction::left));
                // break; 
                //   case (Level::vert_snarlbot):
                // m_actors.push_back(new SnarlBot(x,y,GraphObject::Direction::up));
                // break;             
            default:
            	break;
            };
        }
    }
    //formatting text
    updateText();
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //update text
    updateText();
    //update bonus
    if (m_bonus >0)
        m_bonus--;
    //actors doing shit
    list<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end()){    	
    	(*it)->doSomething();
    	it++;
    }
    m_player->doSomething();

    //deleting dead actors
    it = m_actors.begin();
    while (it != m_actors.end()){    	
    	if (!(*it)->isAlive()){
            if ((*it)->getID() == IID_EXIT)//player at exit
                return GWSTATUS_FINISHED_LEVEL;
    		delete (*it);
    		it = m_actors.erase(it);
    	}
    	else
    		it++;
    }

    if (m_player->isAlive()){
		m_player->doSomething();
		return GWSTATUS_CONTINUE_GAME;
	}
	else{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
//below doesn't kill actors the moment they are killed, but on
//next tick(same thing really)
// it = m_actors.begin();
// while (it != m_actors.end()){
 //       	(*it)->doSomething();
 //       	it++;
 //    }
	  // while (it != m_actors.end()){
   //  	if ((*it)->isAlive()){
   //     		(*it)->doSomething();
   //     		it++;
   //     	}
   //     	else {
   //     		delete (*it);
   //     		it = m_actors.erase(it);
   //     	}
       	
   //  }

}

void StudentWorld::cleanUp()
{
    list<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end()){
        delete *it;
        it = m_actors.erase(it);
    }
    delete m_player;
    assert(m_actors.empty());
}


//PRIVATE FUNCTIONS//
list<Actor*> StudentWorld::getActorAt(const int x,const int y) const{
	list <Actor*> at;
	list<Actor*>::const_iterator it = m_actors.begin();
	while (it != m_actors.end()){
		if ((*it)->getX() == x && (*it)->getY() == y)
			at.push_back(*it);
		it++;
	}
	return at;
}

void StudentWorld::destroyActor(Actor* a){
    if (a == m_player)
        delete m_player;
    else{
     list<Actor*>::iterator i = find(m_actors.begin(), m_actors.end(), a);
     if (a != nullptr)
        delete *i;
        m_actors.erase(i);
    }
}

void StudentWorld::updateText(){
 	ostringstream gametxt;
    gametxt << "Score: " << setw(7) << setfill('0') << getScore() << "  ";
    gametxt	<< "Level: " << setw(2) << setfill('0') << getLevel() << "  ";
    gametxt << "Lives: " << setw(2) << setfill(' ') << getLives() << "  ";
    gametxt << "Health: " << setw(3) << setfill(' ') << m_player->getHealth() << "  ";
    gametxt << "Ammo: " << setw(3) << setfill(' ') << m_player->getAmmo() << "  ";
    gametxt << "Bonus: " << setw(4) << setfill(' ') << m_bonus << "  ";
    setGameStatText(gametxt.str());
}

void StudentWorld::decJewelCount(){
    m_jewelCount--;
}
void StudentWorld::incJewelCount(){
    m_jewelCount++;
}
int StudentWorld::getJewelCount(){
    return m_jewelCount;
}
Player* StudentWorld::getPlayer() const{
    return m_player;   
}

void StudentWorld::addActor(Actor* a){
    m_actors.push_back(a);
}