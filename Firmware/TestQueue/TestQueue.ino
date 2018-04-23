
#include "SPQueue.h"

void setup()
{
  Serial.begin(9600);

  SPQueue queue = SPQueue(10);
  
  queue.push(3,5);
  queue.push(3000,0);
  queue.push(250,200);
  queue.push(0,0);
  queue.push(1000,-500);
  
  while(queue.itemCount() > 0)
  {
    int* temp = queue.pop();
    Serial.print(temp[0]);
    Serial.print("  ");
    Serial.println(temp[1]);
  }
}



void loop()
{

}
