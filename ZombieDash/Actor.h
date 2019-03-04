#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>

class  GameWorld;
class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
public:
    Actor(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world);
    virtual ~Actor();
    bool checkAlive() const;
    virtual void setDead();
    virtual void doSomething() = 0;
    virtual bool infectable();
    virtual bool killableByFlame();
    virtual bool useable();
    virtual bool canTrigger();
    virtual bool block();
    virtual bool blockFlame();
    virtual bool explodeWhenDead();
    virtual void infectIfAppropriate();
    virtual void useExit();
    StudentWorld * getWorld();
private:
    bool isAlive;
    StudentWorld * theWorld;
};

class Person : public Actor {
public:
    Person(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world);
    virtual bool infectable();
    virtual bool block();
    virtual void doSomething();
    virtual void infectIfAppropriate();
    void setIndfect();
    int getInfectCount();
private:
    bool infected;
    int infectCount;
};

class Zombie : public Actor {
public:
    Zombie(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world);
    virtual void doSomething();
    virtual bool block();
    virtual void setDead();
    int getMovePlan() const;
    bool getPara() const;
    void setPara(bool state);
    void dumbZombieMove();
    void setMovePlan(int n);
    void movement();
    void spit();
private:
    int movePlan;
    bool paralyzed;
};

class SmartZombie : public Zombie {
public:
    SmartZombie(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world);
    virtual void doSomething();
    virtual void setDead();
    void smartZ();
};
class Item : public Actor {
public:
    Item(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world);
    virtual bool canTrigger();
    virtual bool useable();
};

class Projectile : public Item {
public:
    Projectile(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world);
    virtual void doSomething();
    virtual bool useable();
    virtual bool killableByFlame();
private:
    int lifeSpan;
};

class Goodie : public Item {
public:
    Goodie(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world);
    virtual void doSomething();
    virtual void grantItem() = 0;
};

class Penelope : public Person {
    
public:
    Penelope(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world);
    virtual void doSomething();
    virtual void setDead();
    void pickLandmine();
    void pickFlamethrower();
    void pickVaccine();
    int getLandmine() const;
    int getFlamethrower() const;
    int getVaccine() const;
private:
    int landmine;
    int flamethrower;
    int vaccine;
};

class Citizen : public Person {
public:
    Citizen(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world);
    virtual void doSomething();
    virtual void setDead();
    virtual void useExit();
    bool turnedZombie();
private:
    bool paralyzed;
};

class Wall : public Item {
    
public:
    Wall(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world);
    virtual void doSomething();
    virtual bool block();
    virtual bool blockFlame();
    virtual bool killableByFlame();
};

class Exit : public Item {
public:
    Exit(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world);
    virtual void doSomething();
    virtual bool useable();
    virtual bool blockFlame();
    virtual bool killableByFlame();
};

class Pit : public Item {
public:
    Pit(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world);
    virtual void doSomething();
    virtual bool useable();
    virtual bool killableByFlame();
};

class Flame : public Projectile{
public:
    Flame(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world);
    virtual void doSomething();
};

class Vomit : public Projectile {
public:
    Vomit(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world);
    virtual void doSomething();
};

class Landmine_Goodie :public Goodie {
public:
    Landmine_Goodie(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world);
    virtual void grantItem();
};

class Landmine : public Item {
public:
    Landmine(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world);
    virtual void doSomething();
    virtual bool explodeWhenDead();
private:
    int safty;
    bool active;
};

class Gas_Can_Goodie : public Goodie {
public:
    Gas_Can_Goodie(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world);
    virtual void grantItem();
};

class Vaccine_Goodie : public Goodie {
public:
    Vaccine_Goodie(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world);
    virtual void grantItem();

};
#endif // ACTOR_H_
