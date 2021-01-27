/*
 * UNIVERSIDAD TECNICA DEL NORTE
 * FICA-CITEL
 * SISTEMAS EMBEBIDOS
 * LISSETH ALBAN
 * PROYECTO FINAL
*/

#include "datos.h"
#include "train.h"
#include <SoftwareSerial.h>
#define rxPin 2
#define txPin 3
SoftwareSerial  serial2 (rxPin, txPin);
int reset=12;


// reinicio
#include "LowPower.h"
unsigned long tiempo;


// Ultrasónico 
const float Echo=6;
const float Trig=7;
long duracion, distancia;   

// Filtro 
int N=25, n=(N-1)/2;
float xj=0, xi=0,zj=0;
float sen[4][25];                     //Vector para guardar los datos de los sensores
float senf[4];                        //vector de sensores filtrados

//knn
float dist[40];                       // vector que guarda la distancia de los sensores a todos los puntos de matriz de entrenamiento
float distor[40];                     // vector de distancias ordenado
int v=5;                              // el número de vecinos que quiero
int k[5];                             // vector para los vecinos 
int etiquetas[4];                     // El numero de etiquetas que evaluo 
int etiqueta=0;                       //muestra el valor final de la etiqueta

//comunicación
char dato;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{       
   
pinMode(rxPin,INPUT);
pinMode(txPin,OUTPUT);
serial2.begin(9600);  
serial2.setTimeout(2000);

  Serial.begin (9600);   
  Serial.setTimeout(1000);
    
  //ultrasónico
  pinMode(Echo, INPUT);           // define el pin 6 como entrada (echo, ultrasónico)
  pinMode(Trig, OUTPUT);          // define el pin 7 como salida  (triger, ultrasónico)
  pinMode(13,OUTPUT);             // Indicador 
  // imprime para sensores
  Serial.println("  S1        S2         S3         Dis  ");

//
//test();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{

// llena matriz de muestreos y luego les filtra
filtro();
//sinfiltro(); si se descomenta esta función, se debe comentar la de filtro(), solo debe funcionar una de las dos

// Imprime el valor filtrado de sensores  
imprimesen();


//knn
{knn();
 Serial.println(String("Etiqueta: ")+etiqueta);}

//enviar datos a processing
enviar();

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void enviar(){
 serial2.write(etiqueta);
}


void test(){
for (int i=0;i<188;i++)
{
  for (int j=0;j<4;j++)
  {
    senf[j]=matriz2[i][j];
  }
  knn();
  
}
}


void sinfiltro()
{
  senf[0]=analogRead(0);
  senf[1]=analogRead(1);
  senf[2]=analogRead(2);
  digitalWrite(Trig, LOW);        // Apaga indicador
  delayMicroseconds(2);           // Espero 2us
  digitalWrite(Trig, HIGH);       // Activa Trigger
  delayMicroseconds(10);          // Espera 10 us 
  digitalWrite(Trig, LOW);        // Apaga Trigger
  duracion = pulseIn(Echo, HIGH); // Mide el tiempo para transformar  
  distancia = duracion*0.001;   // transforma el tiempo en distancia en cm
  senf[3]=distancia;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void filtro()
{
// llena la matriz sen de 25 valores de cada sensor
 for (int f=0;f<4;f++)
{
  for (int c=0;c<N;c++)
  {
  if(f<3)
  {sen[f][c]=analogRead(f);}
  //Serial.print(sen[f][c]+String(" "));} 
  else
  {
  //lectura ultrasónico
  digitalWrite(Trig, LOW);        // Apaga indicador
  delayMicroseconds(2);           // Espero 2us
  digitalWrite(Trig, HIGH);       // Activa Trigger
  delayMicroseconds(10);          // Espera 10 us 
  digitalWrite(Trig, LOW);        // Apaga Trigger
  duracion = pulseIn(Echo, HIGH); // Mide el tiempo para transformar  
  distancia = duracion*0.001;   // transforma el tiempo en distancia en cm
  sen[f][c]=distancia;
  }
  }
}
// Aplica el filtro de Media a cada fila de 25 valores 
for (int c=0; c<4;c++)
{
for (int i = -n; i <= n; i++)
{
  xi = sen[c][n+i];
  xj=(i+xi+xj);
}

for (int i2 = 0; i2 <= N; i2++)
{
  zj=xj/(2*n+1);
  senf[c]=zj;
  delay(20);
}
  zj=0;
  xj=0;
  xi=0;
}
}// fin filtro

void imprimesen(){
for( int c=0;c<4;c++)
{
  Serial.print(senf[c]+String("  |  "));
}
  Serial.println("");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void knn(){
// creo un vector con las distancias dentre el punto de mis sensores y todos los puntos de la matriz de entrenamiento
for(int f=0;f<40;f++)
{
  dist[f]=0;
 for(int c=0;c<4;c++)
  {
    dist[f]=pow((matriz[f][c]-senf[c]),2)+dist[f];
  }
  dist[f]=sqrt(dist[f]);
  distor[f]=dist[f];
}
// ordeno el vector de distancias
BubbleSortAsc(distor, 40);

// guardo en k el valor de los vecinos más cercanos
for (int f=0;f<v;f++)
{
  k[f]=distor[f]; // primeras distancias ordenadas en el vector  K
}

// buscco las etiquetas correspondientes a los vecinos más vercanos en el vector de distancias sin ordenar
for (int c=0;c<4;c++) //busca un ciclo por cada etiqueta  
{
for (int f=0;f<40;f++)
{
  int i=0; //indice del vector k
  int a=k[i ]-dist[f]; // resto los dos valores y si es cero quiere decir que son iguales
  if (a==0)
  {
    if (matriz[f][4]==1)
    {etiquetas[0]++;}
    else if (matriz[f][4]==2)
    {etiquetas[1]++;}
    else if (matriz[f][4]==3)
    {etiquetas[2]++;}
    else if (matriz[f][4]==4)
    {etiquetas[3]++;}
    i++;
  }
}
}
// evalua el mayor valor de etiqueta
int mayor=0;
for (int i=0;i<4;i++)
{
 for (int j=0;j<4;j++)
{
 if  (( etiquetas[i]>= etiquetas[j])&&( etiquetas[i]>= mayor))
 {mayor= etiquetas[i];
 etiqueta=i+1;
 }
}
}
// imprime etiqueta

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BubbleSortAsc(float* values, int length)
{
 int i, j, flag = 1;
 int temp;
 for (i = 1; (i <= length) && flag; i++)
 {
  flag = 0;
  for (j = 0; j < (length - 1); j++)
  {
   if (values[j + 1] < values[j])
   {
   temp = values[j];
   values[j] = values[j + 1];
   values[j + 1] = temp;
   flag = 1;
   }
  }
 }
}
