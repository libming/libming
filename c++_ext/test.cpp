
#include <mingpp.h>

int main()
{
  Ming_init();

  SWFMovie *movie = new SWFMovie();
  SWFShape *shape = new SWFShape();

  shape->setLine(20, 0, 0, 0);
  shape->movePenTo(100,100);
  shape->drawLine(100,0);
  shape->drawLine(0,100);
  shape->drawLine(-100,0);
  shape->drawLine(0,-100);

  movie->add(shape);

  movie->output();
}
