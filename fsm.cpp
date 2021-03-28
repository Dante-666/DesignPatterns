#include <iostream>
#include <map>

struct State;
struct Trigger;
struct Transition;

using namespace std;

struct State {
    State* onTrigger(Trigger* trig);
};

struct Trigger {
    map<State*, pair<Transition*, State*>> fromTo;
    State* transitionTo(State* from);
    void registerTrans(State* from, Transition* trans, State* to);
};

struct Transition {
    void (*callback)(void);
    Transition(void (*cb)(void))
	: callback(cb)
    {
    }
};

State* State::onTrigger(Trigger* trig)
{
    return trig->transitionTo(this);
}

State* Trigger::transitionTo(State* from)
{
    auto it = fromTo.find(from);
    if(it == fromTo.end()) {
	cout<<"Not happening"<<endl;
	return from; 
    } else {
	auto ts_pair = it->second;
	ts_pair.first->callback();
	return ts_pair.second;
    }
}

void Trigger::registerTrans(State* from, Transition* trans, State* to) {
    fromTo.insert({from, pair<Transition*, State*>{trans, to}});
}


int main() {

    State* locked = new State;
    State* unlocked = new State;

    Trigger* push = new Trigger;
    Trigger* coin = new Trigger;

    Transition* l2u = new Transition([]() {cout<<"This "<<endl;});
    Transition* u2u = new Transition([]() {cout<<"is "<<endl;});
    Transition* u2l = new Transition([]() {cout<<"the "<<endl;});
    Transition* l2l = new Transition([]() {cout<<"Pattern."<<endl;});

    push->registerTrans(locked, l2l, locked);
    push->registerTrans(unlocked, u2l, locked);
    coin->registerTrans(unlocked, u2u, unlocked);
    coin->registerTrans(locked, l2u, unlocked);

    locked->onTrigger(coin)->onTrigger(coin)->onTrigger(push)->onTrigger(push);

    return 0;
}
