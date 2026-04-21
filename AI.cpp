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
#include <cmath>
// This file is used only in the Reach, not the Core.
// You do not need to make any changes to this file for the Core

string getAIMoveString(const BuildingState& buildingState) {
    return "";
}

string getAIPickupList(const Move& move, const BuildingState& buildingState, 
                       const Floor& floorToPickup) {
    return "";
}

int calculateFloorPriority(const Floor& floor, int floorNum, int elevatorFloor) {
    int priority = 0;
    int travelTime = abs(floorNum - elevatorFloor);
    for (int i = 0; i < floor.getNumPeople(); i++) {
        int angerLevel = floor.getPersonByIndex(i).getAngerLevel();
        //or should i call the other helper here?
        int ticksUntilExplosion = (MAX_ANGER - angerLevel) * TICKS_PER_ANGER_INCREASE;
        if (travelTime < ticksUntilExplosion) {
            if (ticksUntilExplosion - travelTime <= TICKS_PER_ANGER_INCREASE) {
                priority += angerLevel + abs(POINTS_LOST_PER_EXPLOSION);
            } else {
                priority += angerLevel;
            }
        }
    }
    return priority;
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
