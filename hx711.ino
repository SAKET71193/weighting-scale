#include <HX711.h>

// HX711.DOUT  - pin #A1
// HX711.PD_SCK - pin #A0
   // parameter "gain" is ommited; the default value 128 is used by the library

void hx711_init() {
  //accesing 22 bit adc
   scale.set_scale(13020.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  // for init calibrationn
 // scale.set_scale();                      // this value is obtained by calibrating the scale with known weights; see the README for detail
  scale.tare();               // reset the scale to 0

}

int get_loadcell_reading() {
  char  lcd_data_buff[10];
  
  result_weight = scale.get_units(10);
  String thisString = String(result_weight,3);
  thisString.toCharArray(lcd_data_buff,10);
  lcd.setCursor ( 1, 0 );
  lcd.print("Weight :");
  lcd.setCursor ( 1, 9 );
  if(result_weight < 0)
  lcd.print(&lcd_data_buff[1]);
  else
  lcd.print(lcd_data_buff);

  if(result_weight > 9)
  {
    lcd.setCursor ( 1, 15);
    lcd.print("kg");
  }
  else
  {
    lcd.setCursor ( 1, 14);
    lcd.print(" kg");
  }
  
//  scale.power_down();             // put the ADC in sleep mode
//  delay(30);
//  scale.power_up();
return result_weight;
}
/**************************************************** end ***************************************************************************/
