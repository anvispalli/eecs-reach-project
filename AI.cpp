/*
 * Copyright 2023 University of Michigan EECS183
 *
 * AI.cpp
 * Project UID 848fee0125dbb5eb53ed294f20dbef81
 *
 * <#Names#>
 * <#Uniqnames#>
 *
 * Final Project - Elevators
 */

#include "AI.h"
#include <cassert>

// This file is used only in the Reach, not the Core.
// You do not need to make any changes to this file for the Core

string getAIMoveString(const BuildingState& buildingState) {
    return "";
}

string getAIPickupList(const Move& move, const BuildingState& buildingState, 
                       const Floor& floorToPickup) {
    return "";
}

int floorPriority(const Floor& floor, int floorNum, int elevatorFloor) {
    int score = 0;
    int time = abs(floorNum - elevatorFloor);
    for (int i = 0; i < floor.getNumPeople(); i++) {    
        int angerLevel = floor.getPersonByIndex(i).person.getAngerLevel();
        if (time < ((MAX_ANGER - angerLevel) * TICKS_PER_ANGER_INCREASE)) {
            score += MAX_ANGER - (angerLevel+(time / TICKS_PER_ANGER_INCREASE));
        }
    }
    return score;
}

int getExplosionTime(const Person& p) {
    int angerLevel = p.getAngerLevel();
    int numFromMax = MAX_ANGER - angerLevel;
    int ticks = numFromMax * TICKS_PER_ANGER_INCREASE;
    
    return ticks;
}

string getDominantDirection(const Floor& floor) {
    string direction = "";
    int upAngerSum = 0;
    int downAngerSum = 0;
    int numPeople = floor.getNumPeople();
    for (int i = 0; i < numPeople; i++) {
        if (floor.getPersonByIndex(i).getTargetFloor()
            > floor.getPersonByIndex(i).getCurrentFloor()) {
            upAngerSum += floor.getPersonByIndex(i).getAngerLevel();
        }
        else {
            downAngerSum += floor.getPersonByIndex(i).getAngerLevel();
        }
    }
    if (upAngerSum >= downAngerSum) {
        direction = "up";
    }
    else {
        direction = "down";
    }
    
    return direction;
}
