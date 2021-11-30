#ifndef SLAVE_COMMANDS_H
#define SLAVE_COMMANDS_H

#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum DIRECTION {
  FORWARD,
  BACKWARD,
  STOP
};

class Commands {
  public:
    Commands();
    ~Commands();

    void rotate(float degrees);
    void move();
  private:

};

#endif
