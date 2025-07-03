#include "RC Car Controller.h"



const int RPM_needle_anchor_coordinates[2] = {32, 32};    // Coordinates for RPM needle pivot (center of gauge)
const int speed_needle_anchor_coordinates[2] = {96, 32};  // Coordinates for speed needle pivot (center of gauge)
const int gear_indicator_coordinates[2] = {55, 40}; //Coordinates for gear indicator text
const int headlight_indicator_coordinates[2] = {30, 50}; //Coordinates for headlight indicator icon
const int left_turn_signal_indicator_coordinates[2] = {30, 40}; //Coordinates for left turn signal indicator icon
const int right_turn_signal_indicator_coordinates[2] = {77, 40}; //Coordinates for right turn signal indicator icon
const int connection_indicator_coordinates[2] = {77, 50}; //Coordinates for connection indicator icon
const int gear_indicator_font_size = 3;

char selected_gear = 'N';
int engine_RPM = 0;
int vehicle_speed = 0;
int steering_angle = 0;
int throttle_position = 0;
int brake_position = 0;
int clutch_position = 0;
int headlight_state = 1;
int turn_signal_mode = 3; //0 = off, 1 = left, 2 = right, 3 = hazard

int turn_signal_cycle_duration = 500;  //Duration of on/off cycle for turn signals


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//Function to handle blinker flashing.  Returns true while blinker should be active, false otherwise.
bool blinker_clock(){
	static long last_blink_time = 0;
	static bool blinker_state = false;

	if(millis() - last_blink_time >= turn_signal_cycle_duration){
		blinker_state = !blinker_state; //Toggle blinker
		last_blink_time = millis(); //Update last blink time
	}

	return blinker_state;
}


void prepare_display_buffer() {
  display.setTextColor(SSD1306_WHITE); //Set text color to white
  display.setCursor(0, 0);
  
  //Display guage cluster background
  display.drawBitmap(0, 0, Sprites::gauge_cluster, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  
  //Display gear indicator text
  display.setCursor(gear_indicator_coordinates[0], gear_indicator_coordinates[1]);
  display.setTextSize(gear_indicator_font_size); // Set text size for gear indicator
  display.print(selected_gear); // Display the selected gear

  //Display headlight icon if headlights are on
  if(headlight_state){
	display.drawBitmap(headlight_indicator_coordinates[0], headlight_indicator_coordinates[1], Sprites::headlight_indicator, 15, 10, SSD1306_WHITE);
  }

  //Display turn signal indicators
  if(blinker_clock()){
	//Left turn signal indicator is only active if turn signal mode is 1 or 3 (hazard)
	if(turn_signal_mode == 1 || turn_signal_mode == 3){
		display.drawBitmap(left_turn_signal_indicator_coordinates[0], left_turn_signal_indicator_coordinates[1], Sprites::left_turn_signal_indicator, 15, 10, SSD1306_WHITE);
  	}
	//Right turn signal indicator is only active if turn signal mode is 2 or 3 (hazard)
	if(turn_signal_mode == 2 || turn_signal_mode == 3){
		display.drawBitmap(right_turn_signal_indicator_coordinates[0], right_turn_signal_indicator_coordinates[1], Sprites::right_turn_signal_indicator, 15, 10, SSD1306_WHITE);
	}
}

  //Set RPM needle position
  display.drawLine(
    RPM_needle_anchor_coordinates[0],
    RPM_needle_anchor_coordinates[1],
    RPM_needle_anchor_coordinates[0] + round(20 * cos((180.0 - engine_RPM) * (PI / 180.0))),
    RPM_needle_anchor_coordinates[1] - round(20 * sin((180.0 - engine_RPM) * (PI / 180.0))),
    SSD1306_WHITE
    );

  //Set speed needle position
  display.drawLine(
    speed_needle_anchor_coordinates[0],
    speed_needle_anchor_coordinates[1],
    speed_needle_anchor_coordinates[0] + round(20 * cos((180.0 - vehicle_speed) * (PI / 180.0))),
    speed_needle_anchor_coordinates[1] - round(20 * sin((180.0 - vehicle_speed) * (PI / 180.0))),
    SSD1306_WHITE
    );

  //Display connection indicator
  if(true){
	display.drawBitmap(connection_indicator_coordinates[0], connection_indicator_coordinates[1], Sprites::connection_indicator, 15, 10, SSD1306_WHITE);
  }
}


void update_display(){
  display.clearDisplay();
  prepare_display_buffer();
  display.display();
}


void setup() {
  Serial.begin(9600); // Initialize serial communication for debugging
  //Initialize the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever if display init fails
  }

  Serial.println(F("OLED initialized successfully!"));

  //Initialize the display
  update_display();
}


void loop() {

    if(Serial.available()){
      int RPM_input = Serial.parseInt(); //Read RPM input from serial
      Serial.read();   //Clear buffer
      engine_RPM = map(RPM_input, 0, 2000, 0, 180); //Map RPM input to angle (0-180 degrees)
      Serial.print(F("Received RPM: "));
      Serial.println(RPM_input);
      Serial.println(F("Updated RPM needle variable!"));
    }

    update_display();
}