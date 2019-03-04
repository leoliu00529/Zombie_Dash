#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include "Actor.h"
#include "Level.h"
using namespace std;


//method that put score into proper format. Mostly dealing with trailing zeros and '-' sign
void formatScore(string& score) {
    bool scoreNegative = false;
    if (score[0] == '-') {
        score = score.substr(1,score.length()-1);
        scoreNegative = true;
    }
    switch (score.length()) {
        case 1:
            score = "00000" + score;
            break;
        case 2:
            score = "0000" + score;
            break;
        case 3:
            score = "000" + score;
            break;
        case 4:
            score = "00" + score;
            break;
        case 5:
            score = "0" + score;
            break;
        default:
            break;
    }
    if (scoreNegative) {
        score = "-" + score;
    }
    
}


GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    nextLevel = false;
}


//the destructor calls cleanup method for simplicity
StudentWorld::~StudentWorld() {
    cleanUp();
}

void StudentWorld::finishCurLevel() {
    nextLevel = true;
}

bool StudentWorld::allCitizenCleared() {
    for (int i = 0; i < listOfItems.size(); i ++) {
        if (listOfItems[i]->infectable()) {
            return false;
        }
    }
    return true;
}

//this method checks if an object other than the caller of this function exists at a given set of coordinates
bool StudentWorld::checkCollision(double desX, double desY, Actor * object) {
    for (int i = 0; i < listOfItems.size(); i ++) {
        if (listOfItems[i]->block() && listOfItems[i] != object) {
            double wX = listOfItems[i]->getX();
            double wY = listOfItems[i]->getY();
            if ((desX - wX >= -15 && desX - wX <= 15) && (desY - wY >= -15 && desY - wY <= 15)) {
                return false;
            }
        }
    }
    if (object != player) {
        if ((desX - player->getX() >= -15 && desX - player->getX() <= 15) && (desY - player->getY() >= -15 && desY - player->getY() <= 15)) {
            return false;
        }
    }
    return true;
}

void StudentWorld::addObject(Actor* nObject) {
    listOfItems.push_back(nObject);
}

int StudentWorld::init()
{
    nextLevel = false;
    Level lev(assetPath());
    ostringstream oss;
    oss << "level0" << getLevel() <<".txt";
    string levFile = oss.str();
    Level::LoadResult result = lev.loadLevel(levFile);
    if (result == Level::load_fail_file_not_found) {         //as spec requires, return player_won if no level data file
        cerr << "Cannot find level01.txt data file" << endl;
        return GWSTATUS_PLAYER_WON;
    }
    if (getLevel() == 100) {    //or if player has completed level 99
        return GWSTATUS_PLAYER_WON;
    }
    else if (result == Level::load_fail_bad_format) {
        cerr << "Your level was improperly formatted" << endl;
    }
    else {
        for (int x = 0; x < LEVEL_WIDTH; x ++) {       //loop through the level data, render every Actors
            for (int y = 0; y < LEVEL_HEIGHT; y ++) {
                Level::MazeEntry loc = lev.getContentsOf(x, y);
                switch (loc) {
                    case Level::empty:
                        break;
                    case Level::smart_zombie:
                    {
                        SmartZombie * nSmart = new SmartZombie(IID_ZOMBIE, x * SPRITE_WIDTH, y*SPRITE_HEIGHT, 0, 0, this);
                        listOfItems.push_back(nSmart);
                        break;
                    }
                    case Level::dumb_zombie:
                    {
                        Zombie * nZombie = new Zombie(IID_ZOMBIE, x * SPRITE_WIDTH, y*SPRITE_HEIGHT, 0, 0, this);
                        listOfItems.push_back(nZombie);
                        break;
                        
                    }
                    case Level::player:
                    {
                        player = new Penelope(IID_PLAYER, x * SPRITE_WIDTH, y*SPRITE_HEIGHT, 0, 0, this);
                    }
                        break;
                    case Level::wall:
                    {
                        Wall * nWall = new Wall(IID_WALL, x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 0, 0, this);
                        listOfItems.push_back(nWall);
                    }
                        break;
                    case Level::pit:
                    {
                        Pit * nPit = new Pit(IID_PIT, x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 0, 0, this);
                        listOfItems.push_back(nPit);
                        break;
                    }
                    case Level::landmine_goodie:
                    {
                        Landmine_Goodie * nLandmineG = new Landmine_Goodie(IID_LANDMINE_GOODIE, x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 0, 0, this);
                        listOfItems.push_back(nLandmineG);
                        break;
                    }
                    case Level::gas_can_goodie:
                    {
                        Gas_Can_Goodie * nGas = new Gas_Can_Goodie(IID_GAS_CAN_GOODIE, x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 0, 0, this);
                        listOfItems.push_back(nGas);
                        break;
                    }
                    case Level::exit:
                    {
                        Exit * nExit = new Exit(IID_EXIT, x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 0, 0, this);
                        listOfItems.push_back(nExit);
                        break;
                    }
                    case Level::vaccine_goodie:
                    {
                        Vaccine_Goodie * nVaccine = new Vaccine_Goodie(IID_VACCINE_GOODIE, x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 0, 0, this);
                        listOfItems.push_back(nVaccine);
                        break;
                    }
                    case Level::citizen:
                    {
                        Citizen * nCiti = new Citizen(IID_CITIZEN, x * SPRITE_WIDTH, y * SPRITE_HEIGHT, 0, 0, this);
                        listOfItems.push_back(nCiti);
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if (nextLevel) {       //if the current level is finished
        playSound(SOUND_LEVEL_FINISHED);    //play sound, move to next level
        return GWSTATUS_FINISHED_LEVEL;
    }
    player->doSomething();
    if (!player->checkAlive()) {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    for (int i = 0; i < listOfItems.size(); i ++) {
        //ask every Actor to do something, and check if player is still alive after each iteration
        listOfItems[i]->doSomething();
        if (!player->checkAlive()) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
    }
    size_t length= listOfItems.size();          // remove every dead Actor
    int i = 0;
    while (i < length) {
        if (!listOfItems[i]->checkAlive()) {
            delete listOfItems[i];
            listOfItems.erase(listOfItems.begin() + i);
            length --;
            continue;
        }
        i ++;
    }
    
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    
    string life = to_string(getLives());     //update the game status
    string level = to_string(getLevel());
    string score = to_string(getScore());
    formatScore(score);
    string landmine = to_string(player->getLandmine());
    string flamethrower = to_string(player->getFlamethrower());
    string vaccine = to_string(player->getVaccine());
    string infectCount = to_string(player->getInfectCount());
    string status = "Scores: " + score + "  Level: " + level + "  Lives: " + life + "  Vacc: " + vaccine + "  Flames: " + flamethrower + "  Mines: " + landmine + "  Infected: " + infectCount;
    
    setGameStatText(status);
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    //before deleting anything, first make sure there is something to delete
    if (listOfItems.size() > 0) {
        if (player != nullptr) {
            delete player;
            
        }
        for (int i = 0; i < listOfItems.size(); i ++) {
            delete listOfItems[i];
            listOfItems.erase(listOfItems.begin() + i);
            i --;
        }
    }
}

Actor * StudentWorld::getObejct(int index) const {
    //return an object at given index. To allow easiern implementaion of loops, an given index of -1 returns the player
    if (index == -1) {
        return player;
    }
    if (index < listOfItems.size()) {
        return (listOfItems[index]);
    }
    return nullptr;
}

Penelope * StudentWorld::getPlayer() const {
    return player;
}


