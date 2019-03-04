#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
using namespace std;
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

bool overlap(Actor * self, Actor * other) {     //check if two objects overlap
    double divX = self->getX() - other->getX();
    double divY = self->getY() - other->getY();
    if ((divX * divX + divY * divY) <= 100) {
        return true;
    }
    return false;
}

bool overlap(double selfX, double selfY, Actor * other) {    //overloading the original function
    double divX = selfX - other->getX();
    double divY = selfY - other->getY();
    if ((divX * divX + divY * divY) <= 100) {
        return true;
    }
    return false;
}

void explode(Actor * landmine) {        //when the landmine is triggered by flame or some Person
    landmine->getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
    for (int x = -1; x < 2; x ++) {
        for (int y = -1; y < 2; y ++) {      //check if any of the nine flames is blocked by other objects
            int dX = landmine->getX() + x * SPRITE_WIDTH;
            int dY = landmine->getY() + y * SPRITE_HEIGHT;
            int count = 0;
            Actor * object = landmine->getWorld()->getObejct(count);
            bool ovlap = false;
            while (object != nullptr) {
                if (object->blockFlame()) {
                    if (overlap(dX, dY, object)) {
                        ovlap = true;
                        break;
                    }
                }
                count ++;
                object = landmine->getWorld()->getObejct(count);
            }
            if (ovlap) {
                continue;
            }
            Flame * nFlame = new Flame (IID_FLAME, dX, dY, Actor::up, 0, landmine->getWorld());
            landmine->getWorld()->addObject(nFlame);
        }
    }
    Pit * nPit = new Pit(IID_PIT, landmine->getX(), landmine->getY(), 0, 0, landmine->getWorld());
    landmine->getWorld()->addObject(nPit);       //creating a pit
}

int randDir() {       //return a random direction
    int rand = randInt(0, 3);
    if (!rand) {
        return Actor::up;
    }
    if (rand == 1) {
        return Actor::down;
    }
    if (rand == 2) {
        return Actor::left;
    }
    return Actor::right;
}

//set the consequent x and y coordinates corresponding to a given direction and step length
void setPositionAccordingToDirection(double & x, double & y, const int dir, int length) {
    switch (dir) {
        case Actor::up:
            y += length;
            break;
        case Actor::down:
            y -= length;
            break;
        case Actor::left:
            x -= length;
            break;
        case Actor::right:
            x += length;
            break;
        default:
            break;
    }
}

Actor::Actor(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world) :
GraphObject(imageID, sx, sy, direct, depth) {
    isAlive = true;
    theWorld = world;
}

Actor::~Actor() {}


void Actor::setDead() {
    isAlive = false;
}

bool Actor::block() {
    return false;
}

bool Actor::infectable() {
    return false;
}

bool Actor::useable() {
    return false;
}

bool Actor::killableByFlame() {
    return true;
}

bool Actor::canTrigger() {
    return true;
}

bool Actor::blockFlame() {
    return false;
}

bool Actor::checkAlive() const {
    return isAlive;
}

bool Actor::explodeWhenDead() {
    return false;
}

void Actor::infectIfAppropriate() {}

StudentWorld * Actor::getWorld() {
    return theWorld;
}

void Actor::useExit() {}

Person::Person(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world) :
Actor(imageID, sx, sy, direct ,depth, world) {
    infected = false;
    infectCount = 0;
}

bool Person::infectable() {
    return true;
}

void Person::doSomething() {
    if (infected) {
        if (infectCount < 500) {
            infectCount++;
        }
    }
}

void Person::infectIfAppropriate() {
    if (!infected) {
        getWorld()->playSound(SOUND_CITIZEN_INFECTED);
        infected = true;
        infectCount = 0;
    }
}

int Person::getInfectCount() {
    return infectCount;
}

bool Person::block() {
    return true;
}

void Person::setIndfect() {
    infected = false;
    infectCount = 0;
}

Zombie::Zombie(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world) :
Actor(imageID, sx, sy, right, 0, world) {
    movePlan = 0;
    paralyzed = false;
}

bool Zombie::block() {
    return true;
}

void Zombie::setDead() {   //set a zombie to die, and drop a vaccine if lucky enough
    if (checkAlive()) {
        getWorld()->playSound(SOUND_ZOMBIE_DIE);
        getWorld()->increaseScore(1000);
        if (randInt(0, 9) == 9) {
            double posX = getX();
            double posY = getY();
            int dir = randDir();
            if (dir == up) {
                posY += SPRITE_HEIGHT;
            }
            else if (dir == down) {
                posY -= SPRITE_HEIGHT;
            }
            else if (dir == left) {
                posX -= SPRITE_WIDTH;
            }
            else {
                posX += SPRITE_WIDTH;
            }
            if (getWorld()->checkCollision(posX, posY, this)) {
                //it is still possible that a dropped vaccine is destroyed by flame instantly.
                Vaccine_Goodie* nVac = new Vaccine_Goodie(IID_VACCINE_GOODIE, posX, posY, right, 0, getWorld());
                getWorld()->addObject(nVac);
            }
        }
        Actor::setDead();
    }
}

int Zombie::getMovePlan() const {
    return movePlan;
}

void Zombie::setMovePlan(int n) {
    movePlan = n;
}

bool Zombie::getPara() const {
    return paralyzed;
}

void Zombie::setPara(bool state) {
    paralyzed = state;
}

void Zombie::movement() {     //move a zombie accordingly
    double disX = getX() ;
    double disY = getY();
    setPositionAccordingToDirection(disX, disY, getDirection(), 1);
    if (getWorld()->checkCollision(disX, disY, this)) {
        moveTo(disX, disY);
        movePlan --;
    }
    else {
        movePlan = 0;
    }
}

void Zombie::spit() {       //according to Zombie's position, calculate the vomit location and spit the vomit
    double vX, vY;
    int dir;
    if (getDirection() == up) {
        vX = getX();
        vY = getY() + SPRITE_HEIGHT;
        dir = up;
    }
    else if (getDirection() == down) {
        vX = getX();
        vY = getY() - SPRITE_HEIGHT;
        dir = down;
    }
    else if (getDirection() == left) {
        vX = getX() - SPRITE_WIDTH;
        vY = getY();
        dir = left;
    }
    else {
        vX = getX() + SPRITE_WIDTH;
        vY = getY();
        dir = right;
    }
    int i = -1;
    Actor * object = getWorld()->getObejct(i);
    while (object != nullptr) {
        if (overlap(vX, vY, object) && object->infectable()) {
            if (randInt(0, 2) == 0) {
                getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
                Vomit * nVomit = new Vomit(IID_VOMIT, vX, vY, dir, 0, getWorld());
                getWorld()->addObject(nVomit);
                return;
            }
        }
        i ++;
        object = getWorld()->getObejct(i);
    }
}

//the dumb zombie's dosomething. First check status, then vomit, last move or changhe move plan
void Zombie::doSomething() {
    if (!checkAlive()) {
        return;
    }
    if (paralyzed) {
        paralyzed = false;
        return;
    }
    paralyzed = true;
    spit();
    dumbZombieMove();
}

void Zombie::dumbZombieMove() {     //check if move plan = 0, if not, move according to move plan
    if (movePlan == 0) {
        setMovePlan(randInt(3, 10));
        setDirection(randDir());
    }
    movement();
}

SmartZombie::SmartZombie(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world) :
Zombie(imageID, sx, sy, right, 0, world) {}

void SmartZombie::setDead() {
    if (checkAlive()) {
        Actor::setDead();
        getWorld()->playSound(SOUND_ZOMBIE_DIE);
        getWorld()->increaseScore(2000);
    }
}

/* A smart zombie is more complicated. Like a dumb zombie, it first checks its status, including if it's alive or
 paralysed. It then calculate the vomit part like a dumb zombie. Next it does its own thing, in method smartZ()*/

void SmartZombie::doSomething() {
    if (!checkAlive()) {
        return;
    }
    if (getPara()) {
        setPara(false);
        return;
    }
    setPara(true);
    spit();
    smartZ();
}

void SmartZombie::smartZ() {
    //first calculate the distance between itself and the player
    if (getMovePlan() == 0) {
        setMovePlan(randInt(3, 10));
        int i = -1;
        Actor * object = getWorld()->getObejct(i);
        double shortestDis = 256*256;
        Actor * nearestPerson = getWorld()->getObejct(i);
        while (object != nullptr) {
            if (object->infectable()) {
                double dis = (object->getX() - getX()) * (object->getX() - getX()) + (object->getY() - getY()) * (object->getY() - getY());
                if (dis < shortestDis) {
                    nearestPerson = object;
                    shortestDis = dis;
                }
            }
            i ++;
            object = getWorld()->getObejct(i);
        }
        if (shortestDis > 6400) {
            setDirection(randDir());
        }
        
        //if the distance is less or equal to 80, than the smart zombie tries to move closer to the player
        else {
            if (nearestPerson->getX() == getX()) {     //first check if they are on the same x or y axis
                if (nearestPerson->getY() > getY()) {
                    setDirection(up);
                }
                else {
                    setDirection(down);
                }
            }
            else if (nearestPerson->getY() == getY()) {
                if (nearestPerson->getX() > getX()) {
                    setDirection(right);
                }
                else {
                    setDirection(left);
                }
            }
            else {                                     //if not, randomly choose a direction in either x or y axis
                int d1, d2;
                if (nearestPerson->getY() > getY()) {
                    d1 = up;
                }
                else {
                    d1 = down;
                }
                if (nearestPerson->getX() < getX()) {
                    d2 = left;
                }
                else {
                    d2 = right;
                }
                if (randInt(0, 1)) {
                    setDirection(d1);
                }
                else {
                    setDirection(d2);
                }
            }
        }
    }
    movement();        //with the direction set, the smart zombie attemps to carry out its move plan.
}

Item::Item(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world) :
Actor(imageID, sx, sy, direct, depth, world) {}

bool Item::useable() {
    return true;
}

bool Item::canTrigger() {
    return false;
}


Projectile::Projectile(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world) :
Item(imageID, sx, sy, direct, 0, world) {     //all projectiles have a life span of 2
    lifeSpan = 2;
}

bool Projectile::useable() {
    return false;
}

bool Projectile::killableByFlame() {
    return false;
}

void Projectile::doSomething() {      //all projectiles need to check their lifespan in doSomething()
    if (lifeSpan <= 0) {
        setDead();
        return;
    }
    lifeSpan --;
}

Goodie::Goodie(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world) :
Item(imageID, sx, sy, right, 1, world) {}

void Goodie::doSomething() {
    if (!checkAlive()) {
        return;
    }
    Actor * player = getWorld()->getPlayer();         //All goodies check if it overlaps with player
    if (overlap(this, player)) {
        setDead();
        getWorld()->increaseScore(50);
        grantItem();
        getWorld()->playSound(SOUND_GOT_GOODIE);
    }
}

Penelope::Penelope(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world) :
Person(imageID, sx, sy, right, 0, world) {
    landmine = 0;
    flamethrower = 0;
    vaccine = 0;
}

void Penelope::setDead() {
    getWorld()->playSound(SOUND_PLAYER_DIE);
    Actor::setDead();
}

//All the getter functions down below are to be used by studentWorld to update the game status

void Penelope::pickLandmine() {
    landmine +=2;
}

int Penelope::getLandmine() const {
    return landmine;
}

void Penelope::pickFlamethrower() {
    flamethrower += 5;
}

int Penelope::getFlamethrower() const {
    return flamethrower;
}

void Penelope::pickVaccine() {
    vaccine ++;
}

int Penelope::getVaccine() const{
    return vaccine;
}

void Penelope::doSomething() {
    if (!checkAlive()) {
        return;
    }
    Person::doSomething();
    if(getInfectCount() == 500) {
        setDead();
        getWorld()->playSound(SOUND_PLAYER_DIE);
        return;
    }
    int ch;
    if (getWorld()->getKey(ch)) {       //monitor the key press
        switch(ch) {
            case KEY_PRESS_UP:
                setDirection(up);
                if (getWorld()->checkCollision(getX(), getY() + 4, this)) {
                    moveTo(getX(), getY() + 4);
                }
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                if (getWorld()->checkCollision(getX(), getY() - 4, this)) {
                    moveTo(getX(), getY() - 4);
                }
                break;
            case KEY_PRESS_LEFT:
                setDirection(left);
                if (getWorld()->checkCollision(getX() - 4, getY(), this)) {
                    moveTo(getX() - 4, getY());
                }
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if (getWorld()->checkCollision(getX() + 4, getY(),  this)) {
                    moveTo(getX() + 4, getY());
                }
                break;
            case KEY_PRESS_TAB:
            {
                if (landmine > 0) {
                    Landmine * nLandmine = new Landmine(IID_LANDMINE, getX(), getY(), right, 1, getWorld());
                    getWorld()->addObject(nLandmine);
                    landmine --;
                 }
                
            }
                break;
            case KEY_PRESS_SPACE:
            {
                if(flamethrower > 0) {
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    int di = this->getDirection();
                    for (int i = 1; i < 4; i ++) {
                        double dX, dY;
                        if (di == up) {
                            dX = getX();
                            dY = getY() + i * SPRITE_HEIGHT;
                        }
                        else if (di == down) {
                            dX = getX();
                            dY = getY() - i * SPRITE_HEIGHT;
                        }
                        else if (di == left) {
                            dX = getX() - i * SPRITE_WIDTH;
                            dY = getY();
                        }
                        else{
                            dX = getX() + i * SPRITE_WIDTH;
                            dY = getY();
                        }
                        int count = 0;
                        Actor * object = getWorld()->getObejct(count);
                        bool ol = false;
                        while (object != nullptr) {
                            if (object->blockFlame()) {
                                if (overlap(dX, dY, object)) {
                                    ol = true;
                                    break;
                                }
                            }
                            count ++;
                            object = getWorld()->getObejct(count);
                        }
                        if (ol) {
                            break;
                        }
                        Flame * nFlame = new Flame(IID_FLAME, dX, dY, di, 1, getWorld());
                        getWorld()->addObject(nFlame);
                    }
                    flamethrower --;
                }
                break;
            }
            case KEY_PRESS_ENTER:
            {
                if (vaccine > 0) {
                    vaccine --;
                    Person::setIndfect();
                }
            }
            default:
                break;
        }
    }
}


/*bool Penelope::attacked() {
    return false;
}*/

Citizen::Citizen(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world) :
Person(imageID, sx, sy, right, 0, world) {
    paralyzed = false;
}

void Citizen::setDead() {
    if (checkAlive()) {
        getWorld()->playSound(SOUND_CITIZEN_DIE);
        Actor::setDead();
        getWorld()->increaseScore(-1000);
    }
}

void Citizen::useExit() {       //use setDead method from base class to prevent the -1000 points penalty
    Actor::setDead();
    getWorld()->increaseScore(500);
}

bool Citizen::turnedZombie() {
    if (getInfectCount() == 500) {
        getWorld()->playSound(SOUND_ZOMBIE_BORN);
        setDead();
        getWorld()->increaseScore(-1000);
        int ran = randInt(0, 9);
        if (ran <= 6) {
            Zombie * nZom = new Zombie(IID_ZOMBIE, getX(), getY(), right, 0, getWorld());
            getWorld()->addObject(nZom);
        }
        else {
            SmartZombie * nZom = new SmartZombie(IID_ZOMBIE, getX(), getY(), right, 0, getWorld());
            getWorld()->addObject(nZom);
        }
        return true;
    }
    return false;
}

void Citizen::doSomething() {
    if (!checkAlive()) {
        return;
    }
    Person::doSomething();
    if (turnedZombie()) {
        return;
    }
    if (paralyzed) {
        paralyzed = false;
        return;
    }
    paralyzed = true;
    
    
    // Now calculate the distance to player and to the nearest zombie
    Actor * player = getWorld()->getPlayer();
    double dist_p = (player->getX() - getX()) * (player->getX() - getX()) + (player->getY() - getY()) * (player->getY() - getY());
    double dist_z = 256*256;
    int index = 0;
    Actor * object = getWorld()->getObejct(index);
    while (object != nullptr) {
        if (!object->infectable() && object->block() && object->killableByFlame()) {
            double dis = (object->getX() - getX()) * (object->getX() - getX()) + (object->getY() - getY()) * (object->getY() - getY());
            if (dis < dist_z) {
                dist_z = dis;
            }
        }
        index ++;
        object = getWorld()->getObejct(index);
    }
    
    //if the distance to player is less then distance to the nearest zombie and distance to player < 80
    if (dist_p < dist_z && dist_p <= 6400) {
        double dX = getX();
        double dY = getY();
        int d1, d2;
        if (player->getX() == getX() || player->getY() == getY()) {
            if (player->getX() == getX()) {
                if (player->getY() < getY()) {
                    dY -= 2;
                    setDirection(down);
                }
                else {
                    dY += 2;
                    setDirection(up);
                }
            }
            else if (player->getY() == getY()) {
                if (player->getX() < getX()) {
                    dX -= 2;
                    setDirection(left);
                }
                else {
                    dX += 2;
                    setDirection(right);
                }
            }
            if (getWorld()->checkCollision(dX, dY, this)) {
                moveTo(dX, dY);
                return;
            }
        }
        else {
            if (player->getX() > getX()) {
                d1 = right;
            }
            else {
                d1 = left;
            }
            if (player->getY() > getY()) {
                d2 = up;
            }
            else {
                d2 = down;
            }
            int r1, r2;
            if (randInt(0, 1)) {
                r1 = d1;
                r2 = d2;
            }
            else {
                r1 = d2;
                r2 = d1;
            }
            setPositionAccordingToDirection(dX, dY, r1, 2);
            if (getWorld()->checkCollision(dX, dY, this)) {
                setDirection(r1);
                moveTo(dX, dY);
                return;
            }
            else {
                dX = getX();
                dY = getY();
                setPositionAccordingToDirection(dX, dY, r2, 2);
                if (getWorld()->checkCollision(dX, dY, this)) {
                    setDirection(r2);
                    moveTo(dX, dY);
                    return;
                }
            }
        }
    }
    //if the distance to the cloest zombie is less or equal to 80,
    if (dist_z <= 6400) {
        int di, dir;
        di = right;
        dir = right;
        double disX = getX();
        double disY = getY();
        double finalX = disX;
        double finalY = disY;
        double d1 = dist_z;
        for (int i = 0; i < 3; i ++) {          //check every direction
            switch (i) {
                case 0:
                    di = up;
                    disY += 2;
                    break;
                case 1:
                    di = down;
                    disY -= 2;
                    break;
                case 2:
                    di = left;
                    disX -= 2;
                    break;
                case 3:
                    di = right;
                    disX += 2;
                    break;
                default:
                    break;
            }
            double curDis =  256 * 256;
            if (getWorld()->checkCollision(disX, disY, this)) {
                //calculate the distance to the nearest zombie in all directions
                Actor * object = getWorld()->getObejct(0);
                int index = 0;
                while (object != nullptr) {
                    if (!object->infectable() && object->block() && object->killableByFlame()) {
                        double dis = (object->getX() - disX) * (object->getX() - disX) + (object->getY() - disY) * (object->getY() - disY);
                        if (dis < curDis) {
                            curDis = dis;
                        }
                    }
                    index ++;
                    object = getWorld()->getObejct(index);
                }
                if (curDis > d1) {
                    d1 = curDis;
                    dir = di;
                    finalX = disX;
                    finalY = disY;
                }
            }
        }
        if (d1 <= dist_z) {      //if the distance to nearest zombie is not greater to the original distance
            return;              //then the citizen would remain in the same position
        }
        setDirection(dir);       //else, citizen moves
        moveTo(finalX, finalY);
    }
}



Wall::Wall(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world) :
Item(imageID, sx, sy, right, 0, world) {}

void Wall::doSomething() {
}

bool Wall::block() {
    return true;
}

bool Wall::killableByFlame() {
    return false;
}

bool Wall::blockFlame() {
    return true;
}

Exit::Exit(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world) :
Item(imageID, sx, sy, right, 1, world) {}

bool Exit::useable() {
    return false;
}

bool Exit::killableByFlame() {
    return false;
}

bool Exit::blockFlame() {
    return true;
}

void Exit::doSomething() {       //exit checks if a person overlaps with itself.
    int i = 0;
    Actor * curObject = getWorld()->getObejct(i);
    while (curObject != nullptr) {
        if (curObject->infectable() && overlap(this, curObject)) {
            getWorld()->playSound(SOUND_CITIZEN_SAVED);
            curObject->useExit();
        }
        i ++;
        curObject = getWorld()->getObejct(i);
    }
    if (overlap(this, getWorld()->getPlayer()) && getWorld()->allCitizenCleared()) {
        getWorld()->finishCurLevel();
    }
}


Pit::Pit(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world) :
Item(imageID, sx, sy, right, 0, world) {}

void Pit::doSomething() {
    int i = -1;
    Actor * curObject = getWorld()->getObejct(i);
    while (curObject != nullptr) {
        if (curObject->killableByFlame() || curObject->infectable()) {
            if (overlap(this, curObject)) {
                curObject->setDead();
            }
        }
        i ++;
        curObject = getWorld()->getObejct(i);
    }
}

bool Pit::useable() {
    return false;
}

bool Pit::killableByFlame() {
    return false;
}

Flame::Flame(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world) :
Projectile(imageID, sx, sy, direct, 0, world) {}

void Flame::doSomething() {
    Projectile::doSomething();
    int i = -1;
    Actor* object = getWorld()->getObejct(i);
    while (object != nullptr) {
        if (object->killableByFlame()) {       //flame only kills person, zombie, goodies and landmine
            if (overlap(this, object) && object->checkAlive()) {
                if (object->explodeWhenDead()) {
                    explode(object);
                }
                object->setDead();
            }
        }
        i ++;
        object = getWorld()->getObejct(i);
    }
}


Vomit::Vomit(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world) :
Projectile(imageID, sx, sy, direct, 0, world) {}

void Vomit::doSomething() {
    Projectile::doSomething();
    int i = -1;
    Actor* object = getWorld()->getObejct(i);
    while(object != nullptr) {
        if (object->infectable()) {             //vomit only affects Person
            if (overlap(this, object)) {
                object->infectIfAppropriate();
            }
        }
            i ++;
            object = getWorld()->getObejct(i);
    }
}


Landmine_Goodie::Landmine_Goodie(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world) :
Goodie(imageID, sx, sy, direct, 1, world) {}


//all goodie classes do not have a doSomething() method. They inherit from their base class.
void Landmine_Goodie::grantItem() {
    getWorld()->getPlayer()->pickLandmine();
}

Landmine::Landmine(int imageID, double sx, double sy, int direct, int depth, StudentWorld * world) :
Item(imageID, sx, sy, right, 1, world) {
    safty = 30;
    active = false;
}

bool Landmine::explodeWhenDead() {    //only landmine would explode when destroyed by flame
    return true;
}

void Landmine::doSomething() {      //landmine checks for its safty first
    if (!this->checkAlive()) {
        return;
    }
    if (!active) {
        safty --;
        if (safty == 0) {
            active = true;
        }
        return;
    }
    int i = -1;
    Actor* object = getWorld()->getObejct(i);      //then checks for overlap
    while (object != nullptr) {
        if (object->canTrigger()) {                      //landmine will only be trigger by person and zombie
            if (overlap(this, object)) {
                this->setDead();
                explode(this);
            }
        }
        i ++;
        object = getWorld()->getObejct(i);
    }
}

Gas_Can_Goodie::Gas_Can_Goodie(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world) :
Goodie(imageID, sx, sy, right, 1, world) {}

void Gas_Can_Goodie::grantItem() {
    getWorld()->getPlayer()->pickFlamethrower();
}
    
Vaccine_Goodie::Vaccine_Goodie(int imageID, double sx, double sy, int direct, int depth, StudentWorld* world) :
Goodie(imageID, sx, sy, right, 1, world) {}

void Vaccine_Goodie::grantItem() {
    getWorld()->getPlayer()->pickVaccine();
}

    

