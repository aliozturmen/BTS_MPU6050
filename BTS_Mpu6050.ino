#include "Wire.h"
#include<Math.h>;                                 //Trigonometrik işlemler için kütüphane
const int MPU_addr = 0x68;                        //MPU6050 I2C Slave adresi
unsigned long previousMillis = 0;                 //Önceki millis değeri için değişken
const long interval = 30;                         //Ölçümler arasındaki süre (ms)
 
float ang_x;  //X ekseni ile yapılan açı için değişken
float ang_y;  //Y ekseni ile yapılan açı için değişken
float ang_z;  //Z ekseni ile yapılan açı için değişken

float xson,yson,zson;
float xcalc,ycalc,zcalc;
 
int X_offset = 0;   //X ekseni için offset değeri
int Y_offset = 0;   //Y ekseni için offset değeri
int Z_offset = 1700;//Z ekseni için offset değeri
 
void setup() {
  Serial.begin(9600);                                    //Seri haberlşeme başlatılır
  Serial.println ("Başlamak için bir tuşa basınız");     //Devem edilmesi için girdi beklenir (Seri)
  while (Serial.available() == 0);
  Wire.begin();                                          //I2C Başlatılır
  Wire.beginTransmission(MPU_addr);                      //0x6B register'ındaki setup değerleri 0 yapılır
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
  Serial.begin(9600);
  pinMode(3,OUTPUT);
}
 
void loop() {
  unsigned long currentMillis = millis();                                     //Millis değeri alınır
 
  if (currentMillis - previousMillis >= interval) {                           //İnterval süresi geçtiğinde içeri girilir
    previousMillis = currentMillis;
 
    Wire.beginTransmission(MPU_addr);                                         //MPU6050 ile I2C haberleşme başlatılır
    Wire.write(0x3B);                                                         //İvme bilgisinin olduğu 0x3B-0x40 için request gönderilir
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 6, true);
    
    int16_t XAxisFull =  (Wire.read() << 8 | Wire.read())+X_offset;  //8-bitlik okunan iki değerden büyük ağırlıklı olanı 8 bit sola kaydırılıp küçük olanla veyalanır. Offset eklenir.
    int16_t YAxisFull =  (Wire.read() << 8 | Wire.read())+Y_offset;  //8-bitlik okunan iki değerden büyük ağırlıklı olanı 8 bit sola kaydırılıp küçük olanla veyalanır. Offset eklenir.
    int16_t ZAxisFull =  (Wire.read() << 8 | Wire.read())+Z_offset;  //8-bitlik okunan iki değerden büyük ağırlıklı olanı 8 bit sola kaydırılıp küçük olanla veyalanır. Offset eklenir.
    float XAxisFinal = (float) XAxisFull / 16384.0;                  //Datasheet'te yazan değerlere göre "g" cinsinden ivme bulunur. (X ekseni için)
    float YAxisFinal = (float) YAxisFull / 16384.0;                  //Datasheet'te yazan değerlere göre "g" cinsinden ivme bulunur. (Y ekseni için)
    float ZAxisFinal = (float) ZAxisFull / 16384.0;                  //Datasheet'te yazan değerlere göre "g" cinsinden ivme bulunur. (Z ekseni için)
 
    if(XAxisFinal>0.99) XAxisFinal=1;   //0.99 olan değerler 1'e tamamlanır.
    if(YAxisFinal>0.99) YAxisFinal=1;
    if(ZAxisFinal>0.99) ZAxisFinal=1;
 
    if(XAxisFinal<-0.99) XAxisFinal=-1; //-0.99 olan değerler 1'e tamamlanır.
    if(YAxisFinal<-0.99) YAxisFinal=-1;
    if(ZAxisFinal<-0.99) ZAxisFinal=-1;
 
    ang_x = atan(YAxisFull/(sqrt(pow(XAxisFull,2)+pow(ZAxisFull,2)))) * 57296 / 1000; //Euler Açı formülüne göre açı hesabı. (X-Ekseni)
    ang_y = atan(YAxisFull/(sqrt(pow(XAxisFull,2)+pow(ZAxisFull,2)))) * 57296 / 1000; //Euler Açı formülüne göre açı hesabı. (Y-Ekseni)
        if(XAxisFinal<0)
    {
      XAxisFinal=XAxisFinal*(-1);
    }
        if(YAxisFinal<0)
    {
      YAxisFinal=YAxisFinal*(-1);
    }
        if(ZAxisFinal<0)
    {
      ZAxisFinal=ZAxisFinal*(-1);
    }
 
    Serial.print("X Axis = ");  //Her eksen için g değerleri seri porttan basılır

    Serial.print(XAxisFinal);
    Serial.print("g");
    Serial.print("\t");
    Serial.print("Y Axis = ");
    Serial.print(YAxisFinal);
    Serial.print("g");
    Serial.print("\t");
    Serial.print("Z Axis = ");
    Serial.print(ZAxisFinal);
    Serial.println("g");
    delay(500);

    if(0.2 < XAxisFinal < 0.06)
    {
      if(0.2 < YAxisFinal < 0.06)
      {
      digitalWrite(3,HIGH);
      delay(100);
      digitalWrite(3,LOW);
      delay(100);
      }
    }
  }
}
