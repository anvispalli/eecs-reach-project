/*
 * Copyright 2023 University of Michigan EECS183
 *
 * AI.cpp
 * Project UID 848fee0125dbb5eb53ed294f20dbef81
 *
 * <#Anvi Palli, Anthony Oddo, Max Landman, Athrv Siriki#>
 * <#aspalli, ajoddo, maxgland, asiriki#>
 *
 * Final Project - Elevators
 */

#include "AI.h"
#include "BuildingState.h"
#include "Building.h"
#include "Person.h"
#include <cassert>
#include <string>

// This file is used only in the Reach, not the Core.
// You do not need to make any changes to this file for the Core

int calculateFloorPriority(const _Floor& floor, int floorNum, int elevatorFloor) {
    int priority = 0;
    int travelTime = abs(floorNum - elevatorFloor);
    
    for (int i = 0; i < floor.numPeople; i++) {
        int angerLevel = floor.people[i].angerLevel;
        //or should i call the other helper here?
        int ticksUntilExplosion = (MAX_ANGER - angerLevel) * TICKS_PER_ANGER_INCREASE;
        
        // only count ppl you can reach in time
        if (travelTime < ticksUntilExplosion) {
            
            if (ticksUntilExplosion - travelTime <= TICKS_PER_ANGER_INCREASE) {
                priority += angerLevel + abs(POINTS_LOST_PER_EXPLOSION);
            }
            else {
                priority += angerLevel;
            }
        }
        else {
            priority += angerLevel;
        }
    }
    return priority;
}

int getExplosionTime(const _Person& p) {
    int numFromMax = MAX_ANGER - p.angerLevel;
    int ticks = (numFromMax * TICKS_PER_ANGER_INCREASE);
    
    return ticks;
}

int getTicksToFloor(int currentFloor, int targetFloor) {
    return abs(targetFloor - currentFloor);
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
        for (int p = 0; p < buildingState.floors[f].numPeople; p++) {
            
            int ticksToFloor = getTicksToFloor(elevatorFloor, f); // should be impemented elsewhere
            
            _Person person = buildingState.floors[f].people[p];
            int ticksUntilExplosion = getExplosionTime(person); // should be implemented elsewhere
            
            if (ticksToFloor < ticksUntilExplosion) {
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
    
    
    for (int i = 0; i < NUM_FLOORS; i++) {
        if (buildingState.floors[i].numPeople > 0) {

            int priority = calculateFloorPriority(buildingState.floors[i], i, elevatorFloor);
            
            int distance = abs(elevatorFloor - i);
            
            double score = static_cast<double>(priority) / (1 + distance);
            
            if (score > bestScore) {
                bestScore = score;
                bestFloor = i;
            }
        }
    }
    return bestFloor;
}

string getAIMoveString(const BuildingState& buildingState) {
    int bestElevator = 0;
    int bestDistance = 100000;  // large number
    int bestTargetFloor = 0;

    // Find best elevator
    for (int e = 0; e < NUM_ELEVATORS; e++) {
        // Skip elevators in motion
        if (!buildingState.elevators[e].isServicing) {
            int currentFloor = buildingState.elevators[e].currentFloor;
            int targetFloor = getBestFloor(buildingState, currentFloor);
            int distance = getTicksToFloor(currentFloor, targetFloor);
            if (distance < bestDistance) {
                bestDistance = distance;
                bestElevator = e;
                bestTargetFloor = targetFloor;
            }
        }
    }

    int currentFloor = buildingState.elevators[bestElevator].currentFloor;

    // Pickup if possible
    if (buildingState.floors[currentFloor].numPeople > 0) {
        return "e" + to_string(bestElevator) + "p";
    }

    // Move toward target
    if (bestTargetFloor != currentFloor) {
        return "e" + to_string(bestElevator) + "f" + to_string(bestTargetFloor);
    }

    // fallback
    return "";
}

string getAIPickupList(const Move& move, const BuildingState& buildingState,
                       const Floor& floorToPickup) {
    string pickupList = "";
    string dominantDir = getDominantDirection(floorToPickup);

    int count = 0;
    
    for (int i = 0; i < floorToPickup.getNumPeople() && count < ELEVATOR_CAPACITY; i++) {
        Person p = floorToPickup.getPersonByIndex(i);
        bool goingUp = p.getTargetFloor() > p.getCurrentFloor();

        if (dominantDir == "up" && goingUp) {
            pickupList += to_string(i);
            count++;
        }
        else if (dominantDir == "down" && !goingUp) {
            pickupList += to_string(i);
            count++;
        }
    }
    return pickupList;
}
