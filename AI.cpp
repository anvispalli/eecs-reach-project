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
#include "BuildingState.h"
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

int getTicksToFloor(int currentFloor, int targetFloor) {
    return abs(targetFloor - currentFloor);
}

bool canSavePerson(const Person& p, int elevatorFloor) {
    int ticksToReach = getTicksToFloor(elevatorFloor, p.getCurrentFloor());
    int ticksUntilExplosion = getExplosionTime(p);
    return ticksToReach < ticksUntilExplosion;
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

int getBestFloor(const BuildingState& buildingState, int elevatorFloor) {
    
    int bestEmergencyFloor = -1; // to be changed by later code
    double bestEmergencyScore = -1; // to be changed
    
    // PASS 1: checking for people about to explode
    for (int f = 0; f < NUM_FLOORS; f++) {
        for (int p = 0; p < buildingState.floors[f].numPeople - 1 ; p++) {
            
            int ticksToFloor = getTicksToFloor(); // should be impemented elsewhere
            
            _Person person = buildingState.floors[f].people[p];
            int ticksUntilExplosion = getExplosionTime(person); // should be implemented elsewhere
            
            if (ticksToFloor <= ticksUntilExplosion) {
                double urgency = 1.0 / (ticksUntilExplosion + 1); // + 1 incase it's 0
                
                if (urgency > bestEmergencyScore) { // should always be true first execution
                    bestEmergencyFloor = f;
                    bestEmergencyScore = urgency;
                }
            }
        }
    }
    if (bestEmergencyFloor != -1) {
        return bestEmergencyFloor;
    }
    // PASS 2: normal urgency scoring
    int bestFloor = -1;
    double bestScore = -1;
    
    int totalAnger = 0;
    for (int i = 0; i < NUM_FLOORS; i++) {
        if (buildingState.floors[i].numPeople > 0) {
            
            for (int j = 0; j < buildingState.floors[i].numPeople; j++) {
                totalAnger += buildingState.floors[i].people[j].angerLevel;
            }
            int distance = abs(elevatorFloor - i);
            
            double score = static_cast<double>(totalAnger) / (1 + distance);
            
            if (score > bestScore) {
                bestScore = score;
                bestFloor = i;
            }
        }
    }
    return bestFloor;
}

