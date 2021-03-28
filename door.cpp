#include <iostream>

class DoorState {
  public:
    virtual DoorState *open() = 0;
    virtual DoorState *close() = 0;
    virtual DoorState *ulock() = 0;
};

template <typename T> class DoorCRTP {
  public:
    static T *getInstance() {
        static T* __inst = new T();
        return __inst;
    }
};

class OpenState : public DoorState, public DoorCRTP<OpenState> {
    OpenState() = default;

  public:
    virtual DoorState *open() override;
    virtual DoorState *close() override;
    virtual DoorState *ulock() override;
    friend class DoorCRTP<OpenState>;
};

class CloseState : public DoorState, public DoorCRTP<CloseState> {
    CloseState() = default;

  public:
    virtual DoorState *open() override;
    virtual DoorState *close() override;
    virtual DoorState *ulock() override;
    friend class DoorCRTP<CloseState>;
};

class LockedState : public DoorState, public DoorCRTP<LockedState> {
    LockedState() = default;

  public:
    virtual DoorState *open() override;
    virtual DoorState *close() override;
    virtual DoorState *ulock() override;
    friend class DoorCRTP<LockedState>;
};

using namespace std;

DoorState *OpenState::open() {
    cout << "Door is already open" << endl;
    return OpenState::getInstance();
}
DoorState *OpenState::close() {
    cout << "Closing door..." << endl;
    return CloseState::getInstance();
}
DoorState *OpenState::ulock() {
    cout << "Door cannot be locked" << endl;
    return OpenState::getInstance();
}

DoorState *CloseState::open() {
    cout << "Opening door..." << endl;
    return OpenState::getInstance();
}
DoorState *CloseState::close() {
    cout << "Door is already closed" << endl;
    return CloseState::getInstance();
}
DoorState *CloseState::ulock() {
    cout << "Locking door..." << endl;
    return LockedState::getInstance();
}

DoorState *LockedState::open() {
    cout << "Door cannot be opened" << endl;
    return LockedState::getInstance();
}
DoorState *LockedState::close() {
    cout << "Door cannot be closed" << endl;
    return LockedState::getInstance();
}
DoorState *LockedState::ulock() {
    cout << "Unlocking door" << endl;
    return CloseState::getInstance();
}

class Door {
    DoorState *_currState;

  public:
    Door() { _currState = OpenState::getInstance(); }

    void open() { _currState = _currState->open(); }
    void close() { _currState = _currState->close(); }
    void ulock() { _currState = _currState->ulock(); }
};

int main() {
    Door door;

    door.open();
    door.close();
    door.ulock();

    return 0;
}
