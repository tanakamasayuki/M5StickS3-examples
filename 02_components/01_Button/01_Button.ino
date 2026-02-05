/*
A: 1, B: 0
A: 1, B: 0
A: 0, B: 1
A: 0, B: 1
A: 1, B: 1
A: 0, B: 0
A: 0, B: 0
A: 1, B: 1
*/

#define BUTTON_A 11
#define BUTTON_B 12

void setup()
{
    Serial.begin(115200);
    pinMode(BUTTON_A, INPUT);
    pinMode(BUTTON_B, INPUT);
}

void loop()
{
    Serial.printf("A: %d, B: %d\n", digitalRead(BUTTON_A), digitalRead(BUTTON_B));
    delay(500);
}
