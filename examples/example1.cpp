#include "../graphicsapi.h"
#include <iostream>

int main() {
  Xspace newImage(32); // resolution supports 1:1 only for now

  newImage.setCharacterDensityList(); // initialize ASCII characters;

  newImage.clear();                // clear first ;
  newImage.line({5, 5}, {21, 23}); // draws a line on these coordinates;

  newImage.render(); // finally render

  // simplest possible code

  // for more advanced stuff;

  newImage.clear();
}
