/*
 * Copyright 2023 University of Michigan EECS183
 *
 * Move.cpp
 * Project UID 848fee0125dbb5eb53ed294f20dbef81
 *
 * Max Landman, Anthony Oddo, Anvi Palli, Athrv Siriki
 * maxgland, ajoddo, aspalli, asiriki
 *
 * Final Project - Elevators
 */
 
#include <cmath>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include "Move.h"
#include "SatisfactionIndex.h"

using namespace std;

Move::Move(string commandString) : Move() {
    // initialize everything first
    elevatorId = -1;
    targetFloor = -1;
    numPeopleToPickup = 0;
    totalSatisfaction = 0;

    isPass = false;
    isSave = false;
    isQuit = false;
    isPickup = false;
    
    // handle commands
    if (commandString == "") {
        isPass = true;
        return;
    }
    if (commandString == "S") {
        isSave = true;
        return;
    }
    if (commandString == "Q") {
        isQuit = true;
        return;
    }

    // parse elevator id/convert char to int of same number
    elevatorId = commandString[1] - '0';

    // pickup move
    if (commandString[2] == 'p') {
        isPickup = true;
    }
    /* servicing move: if f is in [2] of command, look to next
     index for floor number */
    else if (commandString[2] == 'f') {
        targetFloor = commandString[3] - '0';
    }
}

bool Move::isValidMove(Elevator elevators[NUM_ELEVATORS]) const {
    if (isPass || isSave || isQuit) {
        return true;
    }
    if (elevatorId < 0 || elevatorId >= NUM_ELEVATORS) {
        return false;
    }
    else if (elevators[elevatorId].isServicing()) {
        return false;
    }
    
    
    if (isPickup) {
        return true;
    }
    else {
        if (targetFloor < 0 && targetFloor >= NUM_FLOORS){
            return false;
        }
        else if (targetFloor == elevators[elevatorId].getCurrentFloor()) {
            return false;
        }
    }
        
    return true;
}

void Move::setPeopleToPickup(const string& pickupList, const int currentFloor,
                             const Floor& pickupFloor) {
    numPeopleToPickup = 0;
    totalSatisfaction = 0;
    int extremeFloor = 0;
    int maxDistance = -1;
    int i = 0;
    while (i < pickupList.size()) {
        int converted_index = pickupList[i] - '0';
        peopleToPickup[numPeopleToPickup] = converted_index;
        numPeopleToPickup++;
        
        // pickup person and add satisfaction
        Person pers = pickupFloor.getPersonByIndex(converted_index);
        
        int anger = pers.getAngerLevel();
        totalSatisfaction += (MAX_ANGER - anger);
        
        // track farthest target
        int target = pers.getTargetFloor();
        int distance = abs(target - currentFloor);

        if (distance > maxDistance) {
            maxDistance = distance;
            extremeFloor = target;
        }

        i++;
    }

    targetFloor = extremeFloor;
}
//////////////////////////////////////////////////////
////// DO NOT MODIFY ANY CODE BENEATH THIS LINE //////
//////////////////////////////////////////////////////

Move::Move() {
    elevatorId = -1;
    targetFloor = -1;
    numPeopleToPickup = 0;
    totalSatisfaction = 0;
    isPass = false;
    isPickup = false;
    isSave = false;
    isQuit = false;
}

bool Move::isPickupMove() const {
    return isPickup;
}

bool Move::isPassMove() const {
    return isPass;
}

bool Move::isSaveMove() const {
    return isSave;
}

bool Move::isQuitMove() const {
    return isQuit;
}

int Move::getElevatorId() const {
    return elevatorId;
}

int Move::getTargetFloor() const {
    return targetFloor;
}

int Move::getNumPeopleToPickup() const {
    return numPeopleToPickup;
}

int Move::getTotalSatisfaction() const {
    return totalSatisfaction;
}

void Move::setTargetFloor(int inTargetFloor) {
    targetFloor = inTargetFloor;
}

void Move::copyListOfPeopleToPickup(int newList[MAX_PEOPLE_PER_FLOOR]) const {
    for (int i = 0; i < numPeopleToPickup; ++i) {
        newList[i] = peopleToPickup[i];
    }
}
