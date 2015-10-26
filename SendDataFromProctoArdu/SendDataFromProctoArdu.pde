
/*
Processing sketch to run with this example:
*/
// This example code is in the public domain.
import processing.serial.*;

int MJMACPort =11;
Serial myPort;                       // The serial port

void setup() {
  
  size(200,200);
  println(Serial.list());
  String portName = Serial.list()[MJMACPort];
  myPort = new Serial(this, portName, 9600);
}

void draw() {
  if (mousePressed == true) 
  {                           //if we clicked in the window
   myPort.write('1');         //send a 1
   println("1");   
  } else 
  {                           //otherwise
  myPort.write('0');          //send a 0
  }   
}