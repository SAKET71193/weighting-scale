
#include "main.h"
#include <HX711.h>
#include <avr/wdt.h>

HX711 scale(16, 17); 
/*!
    @function
    @abstract   Return available RAM memory
    @discussion This routine returns the ammount of RAM memory available after
                initialising the C runtime.
    @param      
    @result     Free RAM available.
*/

static int freeMemory() 
{
  int free_memory;

  if((int)__brkval == 0)
     free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);

  return free_memory;
}


/*!
    @function
    @abstract   Braws a bargraph onto the display representing the value passed.
    @discussion Draws a bargraph on the specified row using barLength characters. 
    @param      value[in] Value to represent in the bargraph
    @param      row[in] Row of the LCD where to display the bargraph. Range (0, 1)
                for this display.
    @param      barlength[in] Length of the bar, expressed in display characters.
    @param      start[in]     Start bar character
    @param      end [in]      End bar character

    @result     None
*/
static void drawBars ( int value, uint8_t row, uint8_t barLength, char start, 
                       char end )
{
   int numBars;

   // Set initial titles on the display
   lcd.setCursor (row, 0);
   lcd.print (start);

   // Calculate the size of the bar
   value = map ( value, -20, 50, 0, ( barLength - 1) * CHAR_WIDTH );
   numBars = value / CHAR_WIDTH;
   
   // Limit the size of the bargraph to barLength
   if ( numBars > barLength )
   {
     numBars = barLength;
   }

   lcd.setCursor ( row,2 );
   
   // Draw the bars
   while ( numBars-- )
   {
      lcd.print ( char( (sizeof(charBitmap ) / sizeof(charBitmap[0])) - 1 ) );
   }
   
   // Draw the fractions
   numBars = value % CHAR_WIDTH;
   lcd.print ( char(numBars) );
   lcd.setCursor (row, barLength + 1);
   lcd.print ( " " );
   lcd.print (end);
}


void switch_tare_isr()
{
    scale.tare(5);
}

void switch_add_isr()
{
  M_add_flag = 1;
  total_weight += result_weight;   
}

void switch_total_isr()
{
  M_total_flag  = 1;
}

void switch_clr_isr()
{
    M_add_flag = 0;
  total_weight = 0;
}

void switch_send_isr()
{
  char buffer_send[300];
  char  lcd_data_buff[10];
  char longitude[10],latitude[10];
   
  if (M_add_flag == 1)
  {
   String thisString = String(total_weight,3);  
   thisString.toCharArray(lcd_data_buff,10);   

   thisString = String(flat,6);
   thisString.toCharArray(longitude,10);
   thisString = String(flon,6);
   thisString.toCharArray(latitude,10);
  if(total_weight < 0)
   sprintf(buffer_send,"{\"device\":{\"make\":\"%s\",\"model\":\"%s\",\"version\":\"%d\",\"deviceid\":\"%s\"},\"reading\":{\"datetime\":\"%02d-%02d-%02d %02d:%02d:%02d\",\"weight\":\"%s\",\"unit\":\"kg\",\"lat\":\"%s\",\"lng\":\"%s\"}}",
make,"SWS-500",1,device_id,day(), month(),year(),hour(),minute(),second(),&lcd_data_buff[1],latitude,longitude);
  else
   sprintf(buffer_send,"{\"device\":{\"make\":\"%s\",\"model\":\"%s\",\"version\":\"%d\",\"deviceid\":\"%s\"},\"reading\":{\"datetime\":\"%02d-%02d-%02d %02d:%02d:%02d\",\"weight\":\"%s\",\"unit\":\"kg\",\"lat\":\"%s\",\"lng\":\"%s\"}}",
make,"SWS-500",1,device_id,day(), month(),year(),hour(),minute(),second(),lcd_data_buff,latitude,longitude);
  
  
  }else{ 
  
  String thisString = String(result_weight,3);
  thisString.toCharArray(lcd_data_buff,10);

   thisString = String(flat,6);
   thisString.toCharArray(longitude,10);   
   thisString = String(flon,6);
   thisString.toCharArray(latitude,10);
  
  if(result_weight < 0)
   sprintf(buffer_send,"{\"device\":{\"make\":\"%s\",\"model\":\"%s\",\"version\":\"%d\",\"deviceid\":\"%s\"},\"reading\":{\"datetime\":\"%02d-%02d-%02d %02d:%02d:%02d\",\"weight\":\"%s\",\"unit\":\"kg\",\"lat\":\"%s\",\"lng\":\"%s\"}}",
make,"SWS-500",1,device_id,day(), month(),year(),hour(),minute(),second(),&lcd_data_buff[1],latitude,longitude);
   else
   sprintf(buffer_send,"{\"device\":{\"make\":\"%s\",\"model\":\"%s\",\"version\":\"%d\",\"deviceid\":\"%s\"},\"reading\":{\"datetime\":\"%02d-%02d-%02d %02d:%02d:%02d\",\"weight\":\"%s\",\"unit\":\"kg\",\"lat\":\"%s\",\"lng\":\"%s\"}}",
make,"SWS-500",1,device_id,day(), month(),year(),hour(),minute(),second(),lcd_data_buff,latitude,longitude);
   }
    HC05.print(buffer_send);  

}

void setup() {
    int i;
    int charBitmapSize = (sizeof(charBitmap ) / sizeof (charBitmap[0]));
    Serial.begin(9600); 

      
   lcd.init ();
   HC05_init();
   init_gps();
   hx711_init();

   
   lcd.setContrast(70);

   lcd.load_custom_character(smiley_symb,smiley);
   lcd.load_custom_character(lowbat_symb,low_bat);
   lcd.load_custom_character(medbat_symb,med_bat);
   lcd.load_custom_character(highbat_symb,high_bat);
   lcd.load_custom_character(gps_symb,gps_symbol);
   lcd.load_custom_character(bluetooth_symb,bluetooth_symbol);
   lcd.load_custom_character(blank_symb,blank_symbol);


    
   for(int i = 10 ; i>0 ;i--)
    {
       lcd.clear ();
       lcd.setCursor ( 0, i );
       lcd.print ("Genius");
       lcd.setCursor ( 1, i );
       lcd.print ("Electronics");
       delay(150);
   
   } 
   delay(50);
   lcd.clear();
   lcd.setCursor ( 0, 4 );
   lcd.print (device_id);
   delay(2000);
   lcd.clear(); 

    
  while(!read_gps())
   {   lcd.setCursor(1,7);
       lcd.print("NO SIGNAL");
       delay(300);
       lcd.clear();
   }
   
       
  pinMode(Tare_interruptPin, INPUT);     //set the pin to input
  digitalWrite(Tare_interruptPin, LOW); //use the internal pullup resistor
  PCintPort::attachInterrupt(Tare_interruptPin, switch_tare_isr,FALLING); // attach a PinChange Interrupt to our pin on the rising edge

  pinMode(M_add_interruptPin, INPUT);     //set the pin to input
  digitalWrite(M_add_interruptPin, HIGH); //use the internal pullup resistor
  PCintPort::attachInterrupt(M_add_interruptPin, switch_add_isr,RISING); // attach a PinChange Interrupt to our pin on the rising edge

  pinMode(total_interruptPin, INPUT);     //set the pin to input
  digitalWrite(total_interruptPin, HIGH); //use the internal pullup resistor
  PCintPort::attachInterrupt(total_interruptPin, switch_total_isr,RISING); // attach a PinChange Interrupt to our pin on the rising edge

  pinMode(send_interruptPin, INPUT);     //set the pin to input
  digitalWrite(send_interruptPin, HIGH); //use the internal pullup resistor
  PCintPort::attachInterrupt(send_interruptPin, switch_send_isr,RISING); // attach a PinChange Interrupt to our pin on the rising edge

  pinMode(M_clr_interruptPin, INPUT);     //set the pin to input
  digitalWrite(M_clr_interruptPin, HIGH); //use the internal pullup resistor
  PCintPort::attachInterrupt(M_clr_interruptPin, switch_clr_isr,RISING); // attach a PinChange Interrupt to our pin on the rising edge

}

void loop() {
         char charbuff[5];

       Batt_Value = analogRead(adc_Pin);  
            
       lcd.setCursor(1,19);
       if(Batt_Value > 833)
       lcd.write(highbat_symb);
       else if(Batt_Value > 791 )
       lcd.write(medbat_symb);
       else
       {
       lcd.write(lowbat_symb);
       if(Batt_Value < 720)
       { lcd.clear();
         while(Batt_Value < 720)
         {
            Batt_Value = analogRead(adc_Pin);
            lcd.setCursor(0,5);
            lcd.print("Low Battery");
            delay(20);
            lcd.clear();
         }
       }
       }
       
       if(HC05_connected())
       {
         lcd.setCursor(0,19);
         lcd.write(bluetooth_symb);
       }else{
         lcd.setCursor(0,19);
         lcd.write(blank_symb);
         while(!HC05_connected()){
          lcd.setCursor(1,2);
          lcd.print("Connect Bluetooth");
          delay(50);
          lcd.clear();
          }
       }

       get_loadcell_reading();
      
       if(read_gps())
       {   lcd.setCursor(0,12);
           lcd.print("GPS");
       }
   
      

        if(M_total_flag == 1)
        {
             char  lcd_data_buff[10];
             lcd.clear();
             String thisString = String(total_weight,3);
             thisString.toCharArray(lcd_data_buff,10);
             
             lcd.setCursor ( 1, 0 );
             lcd.print("T_Weight :");
             lcd.setCursor ( 1, 11 );
              if(total_weight < 0)
                lcd.print(&lcd_data_buff[1]);
              else
                lcd.print(lcd_data_buff);
             lcd.setCursor ( 1, 17 );
             lcd.print("kg");  
             delay(2000);
             lcd.clear();
             M_total_flag = 0;
        }
        if(send_flag == 1)
        { 
          lcd.clear();
          lcd.setCursor(1,4);
          lcd.print("Data Sent");
          send_flag = 0;
        }      
}
