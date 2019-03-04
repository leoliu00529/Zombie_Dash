#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>
class Actor;
class Penelope;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void finishCurLevel();
    bool allCitizenCleared();
    bool checkCollision(double desX, double desY, Actor* object);
    void addObject(Actor * nObject);
    Actor * getObejct(int index) const;
    Penelope * getPlayer() const;
private:
    bool nextLevel;
    Penelope* player;
    std::vector<Actor*> listOfItems;
};


#endif // STUDENTWORLD_H_
