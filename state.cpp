/* 
Udemy logo
Design Patterns in Modern C++
Leave a rating
Your progress
State Coding Exercise

A combination lock is a lock that opens after the right digits have been entered. A lock is preprogrammed with a combination 
(e.g., 12345 ) and the user is expected to enter this combination to unlock the lock.

The lock has a status  field that indicates the state of the lock. The rules are:

    If the lock has just been locked (or at startup), the status is LOCKED.
    If a digit has been entered, that digit is shown on the screen. As the user enters more digits, they are added to Status.
    If the user has entered the correct sequence of digits, the lock status changes to OPEN.
    If the user enters an incorrect sequence of digits, the lock status changes to ERROR.

Please implement the CombinationLock  class to enable this behavior. Be sure to test both correct and incorrect inputs.

Here is an example unit test for the lock:

    CombinationLock cl({1,2,3});
    ASSERT_EQ("LOCKED", cl.status);
    cl.enter_digit(1);
    ASSERT_EQ("1", cl.status);
    cl.enter_digit(2);
    ASSERT_EQ("12", cl.status);
    cl.enter_digit(3);
    ASSERT_EQ("OPEN", cl.status);

*/
#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class LockState {
    string _status;
    map<int, pair<function<string(int, string)>, LockState*>> _trans;

public:
    LockState* enter_digit(int digit);
    void setTransition(int digit, function<string(int, string)> cb, LockState* state);
    void setStatus(string status);
    string getStatus();
};

LockState* LockState::enter_digit(int digit)
{
    auto it = _trans.find(digit);
    if (it != _trans.end()) {
	auto _tpair = it->second;
	auto newStatus = _tpair.first(digit, _status);
	_tpair.second->setStatus(newStatus);
	return _tpair.second;
    } else {
	auto it = _trans.find(-1);
	auto _tpair = it->second;
	auto newStatus = _tpair.first(digit, _status);
	_tpair.second->setStatus(newStatus);
	return _tpair.second;
    }
}

void LockState::setTransition(int digit, function<string(int, string)> cb, LockState* state)
{
    _trans.insert({ digit, { cb, state } });
}

void LockState::setStatus(string status)
{
    _status = status;
}

string LockState::getStatus()
{
    return _status;
}

auto toOpen = [](int, string) -> string {
    return "OPEN";
};
auto toError = [](int, string) -> string {
    return "ERROR";
};
auto toLocked = [](int, string) -> string {
    return "LOCKED";
};
auto appendString = [](int x, string previous) -> string {
    stringstream buff;
    int num;
    try {
	num = stoi(previous);
	buff << num << x;
    } catch (exception& e) {
	buff << x;
    }
    return buff.str();
};

class CombinationLock {
    LockState* _currState;
    vector<int> combination;
    vector<LockState*> _correctStates;
    vector<LockState*> _badStates;
    LockState* _openState;

public:
    string status;
    CombinationLock(const vector<int>& combination)
	: combination(combination)
    {
	for (auto idx = 0; idx < combination.size(); idx++) {
	    _correctStates.push_back(new LockState());
	    _badStates.push_back(new LockState());
	}

	// This will be the open state, the last element in correct sequences
	_correctStates.push_back(new LockState());
	auto _openState = _correctStates.rbegin();

	// Set the current state
	_currState = *(_correctStates.begin());

	// Set the default transition on negative inputs for both the open state and the error state
	(*_openState)->setTransition(-1, toOpen, *_openState);
	(*_openState)->setTransition(666, toLocked, _currState);

	// Set the correct transitions on subsequent vector of states
	auto number = combination.begin();
	auto next = _correctStates.begin();
	++next;
	for (auto current = _correctStates.begin(); next != _correctStates.end(); current++, next++, number++) {
	    if (*next == *_openState) {
		(*current)->setTransition(*number, toOpen, *_openState);
	    } else {
		(*current)->setTransition(*number, appendString, *next);
	    }
	}

	// Set correct to bad transitions
	next = _badStates.begin();
	auto last = _correctStates.rbegin();
	++last;
	for (auto current = _correctStates.begin(); current != _correctStates.end(); current++, next++) {
	    if (*current != *last) {
		(*current)->setTransition(-1, appendString, *next);
	    } else {
		(*current)->setTransition(-1, toError, *next);
	    }
	}

	// Set default states for bad transitions
	next = _badStates.begin();
	++next;
	last = _badStates.rbegin();
	++last;
	for (auto current = _badStates.begin(); next != _badStates.end(); current++, next++) {
	    if (*current != *last) {
		(*current)->setTransition(-1, appendString, *next);
	    } else {
		(*current)->setTransition(-1, toError, *next);
	    }
	}

	auto _errorState = _badStates.rbegin();
	number = combination.begin();
	next = _correctStates.begin();
	++next;

	// Handle cases when the combination is just of length 1
	if (*next != *_openState) {
	    (*_errorState)->setTransition(*number, appendString, *next);
	} else {
	    (*_errorState)->setTransition(*number, toOpen, *next);
	}
	if (*_errorState != *_badStates.begin()) {
	    (*_errorState)->setTransition(-1, appendString, *_badStates.begin());
	} else {
	    (*_errorState)->setTransition(-1, toError, *_errorState);
	}

	// Set the starting status
	status = "LOCKED";
    }

    void enter_digit(int digit)
    {
	if (digit >= 0 && digit < 10) {
	    _currState = _currState->enter_digit(digit);
	    status = _currState->getStatus();
	} else if(digit == 666) {
	    _currState = _currState->enter_digit(digit);
	    status = _currState->getStatus();
	}
    }
};

int main()
{

    CombinationLock cl({ 1 });
    cout << cl.status << endl;
    cl.enter_digit(1);
    cout << cl.status << endl;
    cl.enter_digit(2);
    cout << cl.status << endl;
    cl.enter_digit(3);
    cout << cl.status << endl;
    cl.enter_digit(666);
    cout << cl.status << endl;
    cl.enter_digit(3);
    cout << cl.status << endl;
    cl.enter_digit(3);
    cout << cl.status << endl;
    cl.enter_digit(1);
    cout << cl.status << endl;
    return 0;
}
