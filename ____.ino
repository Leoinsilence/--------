#include <Adafruit_NeoPixel.h>
#include <Wire.h>//调用收发数据所使用的库函数
#include "I2Cdev.h"
#include <SHT2x.h> 
#include <avr/sleep.h>
#include <avr/power.h>
#include <IRremote.h>
#include <U8glib.h>
//-------字体设置，大、中、小
#define setFont_L u8g.setFont(u8g_font_7x13)
#define setFont_M u8g.setFont(u8g_font_fixed_v0r)
#define setFont_S u8g.setFont(u8g_font_fixed_v0r)
#define setFont_SS u8g.setFont(u8g_font_fub25n)
#define PIXEL_PIN    A0//定义LED的接口
#define PIXEL_COUNT  1//定义LED的个数
#define INTERVAL_LCD 20//定义OLED刷新时间间隔
#define temp1    20//#define定义常量
#define temp2    26
#define val_max 255
#define val_min 0   
#define TEM 0x1FEF807    //增加+
#define HUM 0x1FE708F   //减少-
#define CHE 0x1FE48B7  //检查
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
int RECV_PIN = 10;//定义红外接收传感器的接口
IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long lcd_time = millis();                 //OLED刷新时间计时器
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);     //设置OLED型号
long previousMillis = 0;        // 存储LED最后一次的更新
long interval = 1000*60*60*2;           // 闪烁的时间间隔（毫秒）
unsigned long currentMillis=0;
float senser_tem;
float senser_hum;
void Interrupt(void) {
    /* 中断唤醒 */
 
    /*当中断引脚为低电平时关闭中断*/
    Serial.println("hey guys!");
    detachInterrupt(0);
}
void enterSleep(void) {

    attachInterrupt(0, Interrupt, LOW);
    delay(100);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_mode();
    sleep_disable();
}
void read()
{
  senser_tem = SHT2x.GetTemperature() ;//把获得的温度值赋给变量sensor_tem
  senser_hum = SHT2x.GetHumidity() ;//把获得的湿度值赋给变量sensor_light
  Serial.println(senser_tem);//将数据从Arduino传递到PC且单独占据一行，此数据可在串口监视器中看到
  Serial.print("--");   
  Serial.println(SHT2x.GetHumidity());
  delay(100);
}
void rainbowCycle( int r, int g, int b, uint8_t wait) {
  for (int val = 0; val < 255; val++) 
  //val由0自增到254不断循环
  {
colorSet(strip.Color(map(val, val_min, val_max, 0, r), map(val, val_min, val_max, 0, g), map(val, val_min, val_max, 0, b)));
//红绿蓝LED灯依次从暗到亮
/*“map(val,x,y,m,n)”函数为映射函数，可将某个区间的值（x-y）变幻成（m-n），val则是你需要用来映射的数据*/
    delay(wait); //延时
  }
  for (int val = 255; val >= 0; val--)  //val从255自减到0不断循环
  {
colorSet(strip.Color(map(val, val_min, val_max, 0, r), map(val, val_min, val_max, 0, g), map(val, val_min, val_max, 0, b)));
//红绿蓝LED灯依次由亮到暗
    delay(wait); //延时
  }
}
void colorSet(uint32_t c) {
  //for 函数循环语句
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);//i:表示第几个灯开始，从0开始算第一个灯；c 表示灯的颜色
    strip.show();//表示LED显示
    //delay(wait);
  }
}
void setup() {
    Serial.begin(9600);
    irrecv.enableIRIn(); // 启动红外解码
    Serial.println("Initialisation complete.");
    pinMode(RECV_PIN, INPUT);
    Wire.begin(); 
    strip.begin();
    strip.show(); // 关灯    
}

void loop() {
  read();//调用自定义函数
  currentMillis = millis();
    Serial.print("Awake for ");
    Serial.print(currentMillis - previousMillis, DEC);
    Serial.println(" second");
    delay(1000);

    if (irrecv.decode(&results)) {
      Serial.println(results.value, HEX);
  
      u8g.firstPage();
      do {
          switch (results.value)
          {
          case CHE:
          setFont_L;
          for(int times=1;times<=5;times++){
            if (times==1){    
          u8g.setPrintPos(4, 20);
          u8g.print("  self checking");
          
            }
            else if (times==2){    
          u8g.setPrintPos(4, 20);
          u8g.print("  self checking");
          u8g.setPrintPos(30,35);
          u8g.print("wait...");
          colorSet(strip.Color(0, 0, 255));
          delay(500);  
            } 
            else if(times==3){
              
          u8g.setPrintPos(4, 20);
          u8g.print("  self checking");
          u8g.setPrintPos(30,35);
          u8g.print("wait...");
          colorSet(strip.Color(0, 255, 0));
          delay(500);         
            }
            else if(times==4){
          u8g.setPrintPos(4, 20);
          u8g.print("  self checking");
          u8g.setPrintPos(30,35);
          u8g.print("wait...");
          colorSet(strip.Color(255, 0, 0));
          delay(500);           
            }
            else if(times==5){
          u8g.setPrintPos(4, 20);
          u8g.print("  self checking");
          u8g.setPrintPos(30,35);
          u8g.print("wait...");         
          u8g.setPrintPos(45, 50);
          u8g.print("Normal");
          u8g.setPrintPos(5, 63);
          u8g.print("  B   U   P   T"); 
            }
          }
          break;  
          case TEM:
          setFont_L;
          u8g.setPrintPos(40, 20);
          u8g.print("instant ");
          u8g.setPrintPos(30,35);
          u8g.print("temperature");
          u8g.setPrintPos(45, 50);
          u8g.print(senser_tem);
          u8g.setPrintPos(5, 63);
          u8g.print("  B   U   P   T");
          break;
          case HUM:
          setFont_L;
          u8g.setPrintPos(40, 20);
          u8g.print("instant ");
          u8g.setPrintPos(35,35);
          u8g.print("humidity");
          u8g.setPrintPos(45, 50);
          u8g.print(senser_hum);
          u8g.setPrintPos(5, 63);
          u8g.print("  B   U   P   T");
          break;
          default:
          setFont_L;
          u8g.setPrintPos(0, 20);
          u8g.print("Vol+ indicates tem");
          u8g.setPrintPos(0,35);
          u8g.print("Vol- indicates hum");
          u8g.setPrintPos(0,50);
          u8g.print("   ************");
          u8g.setPrintPos(5, 63);
          u8g.print("  B   U   P   T");
          colorSet(strip.Color(0, 0, 0));
          break;  
          }
          
      }while( u8g.nextPage() );
      
      irrecv.resume(); // 接收下一个值
      previousMillis = currentMillis;
    }

    if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis; 
      Serial.println("Entering sleep");
      enterSleep();
    }
  if(currentMillis<previousMillis)
    previousMillis=currentMillis=0;
  switch (results.value)
  {
  case TEM:
  if (senser_tem <= temp1)//if判断语句
    rainbowCycle( 0, 0, 255, 10); //蓝色呼吸
    //colorSet(int color)设置颜色的函数
    //strip.Color(R,G,B)修改RGB的值可以改变所显现的颜色，R：红色；G：绿色；B：蓝色；
  else if (senser_tem > temp1 && senser_tem <= temp2)//三个逻辑运算符，>表示大于，&&表示逻辑关系“与”，<=表示大于等于
    rainbowCycle( 0, 255, 0, 10); //绿色呼吸//绿色，温度适宜
 
  else if (senser_tem > temp2 )
    rainbowCycle( 255, 0, 0, 10); //红色呼吸//红色，偏热
  break;
  case HUM:
  if (senser_hum <= 40)//if判断语句
    rainbowCycle( 255, 0, 0, 10); //红色呼吸
    //colorSet(int color)设置颜色的函数
    //strip.Color(R,G,B)修改RGB的值可以改变所显现的颜色，R：红色；G：绿色；B：蓝色；
 
  else if (senser_hum > 40 && senser_hum <= 60)//三个逻辑运算符，>表示大于，&&表示逻辑关系“与”，<=表示大于等于
    rainbowCycle( 0, 255, 0, 10); //绿色呼吸//绿色，湿度适宜
 
  else if (senser_hum > 60 )
    rainbowCycle( 0, 0, 255, 10); //蓝色呼吸//蓝色，潮湿
    break;
    default:
    break;

  }
  }


