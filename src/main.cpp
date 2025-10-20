#include "engine.h"

int main() {
  Engine engine{};
  engine.Init();
  engine.Run();
  engine.Destroy();
}
