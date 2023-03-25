#include <MIDI.h>
#include <TM1637Display.h>

#define PIN_RAW_INPUT 2
#define PIN_LED_NOTE 6
#define PIN_LED_TEMPO 7
#define PIN_TRANSPOSE_POT 1
#define DIO 12
#define CLK 13

static uint8_t  ticks = 0;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);
TM1637Display display(CLK, DIO);
int transpose = 0;

int getTranspose()
{
    uint32_t pot_val;
  
    pot_val = analogRead(PIN_TRANSPOSE_POT);
  
    return map(pot_val,0,1024,-7,8);
}

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
    if (pitch==120) {
        MIDI.sendRealTime(MIDI_NAMESPACE::Start);
    }
    if (pitch==121) {
        MIDI.sendRealTime(MIDI_NAMESPACE::Stop);      
    }
    //int transpose = getTranspose();
    byte newPitch = pitch;
    newPitch = pitch + (16 * transpose);
    MIDI.sendNoteOn(newPitch, velocity, channel);
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
    //int transpose = getTranspose();
    byte newPitch = pitch;
    newPitch = pitch + (16 * transpose);
    MIDI.sendNoteOff(newPitch, velocity, channel);
}

void handleClock()
{
    ticks++;
                
    if(ticks < 6)
    {
        digitalWrite(PIN_LED_TEMPO, LOW);    
    }
    else if(ticks == 6)
    {
        digitalWrite(PIN_LED_TEMPO, HIGH);
    }
    else if(ticks >= 24)
    {
        ticks = 0;
    }

    MIDI.sendRealTime(MIDI_NAMESPACE::Clock);
}

void handleStart()
{
    MIDI.sendRealTime(MIDI_NAMESPACE::Start);
}

void handleStop()
{
    MIDI.sendRealTime(MIDI_NAMESPACE::Stop);
}

void setup() 
{
    pinMode(PIN_LED_TEMPO, OUTPUT);
    digitalWrite(PIN_LED_TEMPO, HIGH);
    pinMode(PIN_LED_NOTE, OUTPUT);
    digitalWrite(PIN_LED_NOTE, HIGH);
    
    MIDI.begin(MIDI_CHANNEL_OMNI);
    MIDI.turnThruOff();
    
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.setHandleClock(handleClock);
    MIDI.setHandleStart(handleStart);
    MIDI.setHandleStop(handleStop);
    display.setBrightness(1);
}

void loop() 
{
    MIDI.read();
    int newTranspose = getTranspose();
    if (transpose != newTranspose)
    {
        transpose = newTranspose;
        // MIDI.sendSystemReset();
        for (byte c = 0; c < 17; c++){
            MIDI.sendControlChange(123,0,c);
        }
    }
    display.showNumberDec(transpose, false);
}
