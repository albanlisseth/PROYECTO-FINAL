import processing.serial.*;
import ddf.minim.*;
PImage i1, silla,e2,e1,e3,e4;
Serial port;
int etiqueta=0;
void setup()
{
port= new Serial(this, "COM10",9600);
delay(500);
size(600,400);
background(0);
inicio();
}  

void draw() {

  sensores();

}


void serialEvent(Serial port)
{
 etiqueta = port.read();
 println(etiqueta);
   
}


void inicio()
{
background(0);
i1=loadImage("1.png");
imageMode(CORNER);
tint(80,50);
image(i1, 0, 0, 600, 400); 
textSize(30);
fill(255);
stroke(0);
text("Machine Learning\nProyecto final sistemas embebidos\n2021\n\n  ",60,180);
}

void sensores(){
  delay(5000);
background(0);
tint(80,50);
image(i1, 0, 0, 600, 400); 
tint(255);
silla=loadImage("Silla.png");
image(silla, 60, 60, 250, 250);
text("Etiqueta: "+etiqueta,350,80);
if (etiqueta==2)
{
  imageMode(CORNER);
e2=loadImage("e2.png");
image(e2, 350, 140, 120, 150);
}
if (etiqueta==1)
{
  imageMode(CORNER);
e1=loadImage("e1.png");
image(e1, 350, 140, 120, 150);
}
if (etiqueta==3)
{
  imageMode(CORNER);
e3=loadImage("e3.png");
image(e3, 350, 140, 120, 150);
}
if (etiqueta==4)
{
  imageMode(CORNER);
e4=loadImage("e4.png");
image(e4, 350, 140, 120, 150);
}
}
