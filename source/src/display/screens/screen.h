#ifndef UVENT_SCREEN_H
#define UVENT_SCREEN_H

class Screen {
public:
    Screen();
    virtual void init();
    virtual void setup();
    virtual void cleanup();
protected:
private:
};

class MainScreen : public Screen {
public:
    MainScreen();
    void init();
    void setup();
    void cleanup();
};

#endif //UVENT_SCREEN_H
