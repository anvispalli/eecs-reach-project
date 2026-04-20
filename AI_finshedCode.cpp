/*
 * Copyright 2023 University of Michigan EECS183
 *
 * AI.cpp
 * Project UID 848fee0125dbb5eb53ed294f20dbef81
 *
 * Max Landman, Anthony Oddo, Anvi Palli, Athrv Siriki
 * maxgland, ajoddo, aspalli, asiriki
 *
 * Final Project - Elevators
 */

#include "AI.h"
#include <cassert>

// This file is used only in the Reach, not the Core.
// You do not need to make any changes to this file for the Core

// Returns true if another elevator is already servicing the given floor
bool floorAlreadyTargeted(const BuildingState& buildingState, int floorNum) {
    for (int i = 0; i < NUM_ELEVATORS; i++) {
        if (buildingState.elevators[i].isServicing &&
            buildingState.elevators[i].targetFloor == floorNum) {
            return true;
        }
    }
    return false;
}

// Computes the actual sumIndex points gained by sending an elevator
// at elevFloor to floorNum. Only counts saveable people (those who
// won't explode before the elevator arrives). Returns 0 if no one
// is saveable, meaning there is no point in going there.
int scoreFloor(const BuildingState& buildingState, int floorNum, int elevFloor) {
    int travelTime = floorNum - elevFloor;
    if (travelTime < 0) {
        travelTime = -travelTime;
    }

    int points = 0;

    for (int i = 0; i < buildingState.floors[floorNum].numPeople; i++) {
        int angerLevel = buildingState.floors[floorNum].people[i].angerLevel;
        int ticksUntilExplosion = (MAX_ANGER - angerLevel) * TICKS_PER_ANGER_INCREASE;

        // skip doomed people — they explode before we arrive regardless
        if (travelTime >= ticksUntilExplosion) {
            continue;
        }
        // calculates total anger by the time of arrival of elevator
        int angerOnArrival = angerLevel + (travelTime / TICKS_PER_ANGER_INCREASE);
        points += MAX_ANGER - angerOnArrival;
    }

    return points;
}

string getAIMoveString(const BuildingState& buildingState) {
    for (int elevId = 0; elevId < NUM_ELEVATORS; elevId++) {
        const _Elevator& elev = buildingState.elevators[elevId];

        if (elev.isServicing) {
            continue;
        }

        // if already on a floor with people, pick them up immediately
        if (buildingState.floors[elev.currentFloor].numPeople > 0) {
            return "e" + to_string(elevId) + "p";
        }

        int bestFloor = -1;
        int bestScore = 0;  // only move if there is positive value

        for (int floorNum = 0; floorNum < NUM_FLOORS; floorNum++) {
            if (floorNum == elev.currentFloor) {
                continue;
            }
            if (buildingState.floors[floorNum].numPeople == 0) {
                continue;
            }
            if (floorAlreadyTargeted(buildingState, floorNum)) {
                continue;
            }

            int score = scoreFloor(buildingState, floorNum, elev.currentFloor);
            if (score > bestScore) {
                bestScore = score;
                bestFloor = floorNum;
            }
        }

        if (bestFloor != -1) {
            return "e" + to_string(elevId) + "f" + to_string(bestFloor);
        }
    }

    return "";
}

string getAIPickupList(const Move& move, const BuildingState& buildingState,
                       const Floor& floorToPickup) {
    int elevFloor = buildingState.elevators[move.getElevatorId()].currentFloor;
    int numPeople = floorToPickup.getNumPeople();

    // score each direction by total saveable points going that way
    int upScore = 0;
    int downScore = 0;

    for (int i = 0; i < numPeople; i++) {
        Person p = floorToPickup.getPersonByIndex(i);
        int angerLevel = p.getAngerLevel();
        int points = MAX_ANGER - angerLevel;

        if (p.getTargetFloor() > elevFloor) {
            upScore += points;
        } else {
            downScore += points;
        }
    }

    // pick the direction with higher value
    bool goingUp = (upScore >= downScore);

    string pickupList = "";
    for (int i = 0; i < numPeople; i++) {
        Person p = floorToPickup.getPersonByIndex(i);
        bool personGoingUp = (p.getTargetFloor() > elevFloor);
        if (personGoingUp == goingUp) {
            pickupList += to_string(i);
        }
    }

    return pickupList;
}
