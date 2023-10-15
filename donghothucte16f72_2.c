#INCLUDE <16F72.H>
#FUSES NOWDT, PUT, HS, NOPROTECT
#USE DELAY(CLOCK=20M)
#use i2c(master,fast,sda = pin_c4,scl = pin_c3) // su dung giao tiep i2c cho DS1307 
CONST UNSIGNED CHAR ma [10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
unsigned int8 rtc[3],i,phut,gio,step;
#BIT TMR1IF = 0x0C.0


void rtc_init() // khoi tao cho ds1307 
{  
   int8 a; // byte luu tam gia tri o nho 0 cua ds
   i2c_start();
   i2c_write(0xD0); // ghi vao ds1307 chieu ghi
   i2c_write(0); // ghi dia chi can doc
   i2c_start(); // start bit i2c 
   i2c_write(0xD1); // ghi vao ds1307 chieu doc
   a = i2c_read(0);
   i2c_stop();
   if ((a&0x80)!=0)                                  
   {
      a&=0x7f;
      i2c_start();         
      i2c_write(0xD0); // ghi vao ds1307 chieu ghi
      i2c_write(0); // ghi dia chi can doc
      i2c_write(a); // 
      i2c_stop();   
   }  
}

void ReadRTC() // doc du lieu tu 1307
{
   i2c_Start(); // Lenh bat dau i2c  
   i2c_Write(0xD0); // ghi ma ghi vào DS1307
   i2c_Write(0x00); // Ghi dia chi 0 vao DS
   i2c_Start(); // Lenh restart i2c
   i2c_Write(0xD1); // chi ma doc vao DS
   rtc[0]=i2c_Read(1); // Doc giay voi bit thua nhan (ACK) de dia chi tang tu dong
   rtc[1]=i2c_Read(1); // doc phut
   rtc[2]=i2c_Read(0); // doc gio khong bit thua nhan ket thuc viec doc                                      
   i2c_Stop(); // Lenh dung i2c
   phut=(rtc[1]>>4)*10+(rtc[1]&0x0f);
   gio=(rtc[2]>>4)*10+(rtc[2]&0x0f);
}

void WriteRTC()// ghi du lieu vao 1307
{
   rtc[1] = ((phut/10)<<4) | (phut%10); // chuyen 2 so thap phan cua phut thanh BCD luu vao mang thoi gian 
   rtc[2] = ((gio/10)<<4) | (gio%10); // chuyen 2 so thap phan cua gio thanh BCD luu vao mang thoi gian
   
   i2c_Start(); // lenh bat dau i2c
   i2c_Write(0xD0); // ghi ma ghi vao DS
   i2c_Write(0x00); // nap dia chi ban dau vao DS
   i2c_Write(0x00); // ghi giay
   i2c_Write(rtc[1]); // ghi phut
   i2c_Write(rtc[2]); // ghi gio
   i2c_Stop(); // lenh dung i2c
} 


void hienthi()
{
   output_B(ma[phut%10]);
   output_high(pin_A0);delay_ms(1);
   output_low(pin_A0);
   
   output_B(ma[phut/10]);
   output_high(pin_C7);delay_ms(1);
   output_low(pin_C7);
   
   output_B(ma[gio%10]+0x80);
   output_high(pin_C6);delay_ms(1);
   output_low(pin_C6);
   
   output_B(ma[gio/10]);
   output_high(pin_C5);delay_ms(1);
   output_low(pin_C5);
}

void main()
{
   set_tris_A(0xFE);
   set_tris_B(0x00);
   output_B(0xFF);
   rtc_init();//khoi tao RTC
   SETUP_TIMER_1(T1_INTERNAL | T1_DIV_BY_8);
   SET_TIMER1(3036);
   readRTC();
   
//khoi tao "LIEU"==============================================================   
   for(i=0;i<250;i++)
   {
      output_B(0xC1); //"U"
      output_high(pin_A0);delay_ms(1);
      output_low(pin_A0);
      
      output_B(0x86);//"E"
      output_high(pin_C7);delay_ms(1);
      output_low(pin_C7);
      
      output_B(0xCF);//"I"
      output_high(pin_C6);delay_ms(1);
      output_low(pin_C6);
      
      output_B(0xC7);//"L"
      output_high(pin_C5);delay_ms(1);
      output_low(pin_C5);
   }
   
  while(true)
  
  {
      if (TMR1IF==1)
      {
         step++;
         if (step==10)
         {
            step=0;
            readRTC();
         }
         TMR1IF=0;
      }
      hienthi();
      //chinh thoi gian========================================================
      if (input(pin_C0)==0)
      {
        delay_ms(20);
        if (input(pin_C0)==0)
        {
           while(input(pin_C0)==0);
           while(true)
            {
               if(input(pin_C1)==0)
               {
                  delay_ms(20);
                  if(input(pin_C1)==0)
                  {
                     while(input(pin_C1)==0);
                     if(gio<23)
                     {
                        gio++;
                     }
                     else
                     {
                        gio=0;
                     }
                  }
               }
               if(input(pin_C2)==0)
               {
                  delay_ms(20);
                  if(input(pin_C2)==0)
                  {
                     while(input(pin_C2)==0);
                     if(gio>0)
                     {
                        gio--;
                     }
                     else
                     {
                        gio=23;
                     }
                  }
               }
               output_B(ma[gio%10]);
               output_high(pin_C6);delay_ms(1);
               output_low(pin_C6);
               
               output_B(ma[gio/10]);
               output_high(pin_C5);delay_ms(1);
               output_low(pin_C5);
               if (input(pin_C0)==0)
               {
                  delay_ms(20);
                  if (input(pin_C0)==0)
                  {
                     while(input(pin_C0)==0);
                     while(true)
                     {
                        if(input(pin_C1)==0)
                        {
                           delay_ms(20);
                           if(input(pin_C1)==0)
                           {
                              while(input(pin_C1)==0);
                              if(phut<59)
                              {
                                 phut++;
                              }
                              else
                              {
                                 phut=0;
                              }
                           }
                        }
                        if(input(pin_C2)==0)
                        {
                           delay_ms(20);
                           if(input(pin_C2)==0)
                           {
                              while(input(pin_C2)==0);
                              if(phut>0)
                              {
                                 phut--;
                              }
                              else
                              {
                                 phut=59;
                              }
                           }
                        }
                        output_B(ma[phut%10]);
                        output_high(pin_A0);delay_ms(1);
                        output_low(pin_A0);
                        
                        output_B(ma[phut/10]);
                        output_high(pin_C7);delay_ms(1);
                        output_low(pin_C7);
                        if (input(pin_C0)==0)
                        {
                           delay_ms(20);
                           if (input(pin_C0)==0)
                           {
                              while(input(pin_C0)==0);
                              writeRTC();
                              break;   
                           }
                        }
                     }
                     break;
                  }
               }
            }
        }
      
      }
     
      
  }
  
}
