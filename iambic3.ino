/*

  iambic keyer


  MAP:
  
     - commented 'sleep_bod_disable()' in yackpoower as function is not understood in arduino 1.0.6 IDE

  
  https://github.com/dhakajack/jackyack
  
  

                         ATtiny85               
                   +-------- --------+
              PB5 -|1(RST) |_|      8|- VCC
     DIT <--  PB3 -|2          (SCK)7|- PB2 --> CMD BTN -------- GND
     DAH <--  PB4 -|3         (MISO)6|- PB1 --> PIEZO  --------- GND   
              GND -|4         (MOSI)5|- PB0 --> TX OUT
                   +-----------------+

*/



// --- yack.h begin ---

/* ********************************************************************
 Program  : yack.h
 Author   : Jan Lategahn DK3LJ modified by Jack Welch AI4SV, adapted
              to single .ino file for programming with an Arduino as a
        programmer by Marcel Post VK2CEL        
 Purpose  : definition of keyer hardware
 Created  : 15.10.2010
 Update   : 11.01.2017
 Version  : 0.75
 
 Changelog
 ---------
 Version    Date    Change
 ----------------------------------------------------------------------
 
 
 Todo
 ----
 
 
 *********************************************************************/

// User configurable settings

// The following settings define the hardware connections to the keyer chip

// Definition of where the keyer itself is connected 
#define   KEYDDR      DDRB
#define   KEYPORT     PORTB
#define   KEYINP      PINB
#define   DITPIN      3
#define   DAHPIN      4

// Definition of where the transceiver keyer line is connected
#define   OUTDDR      DDRB
#define   OUTPORT     PORTB
#define   OUTPIN      0

// Definition of where the sidetone output is connected (beware,
// this is chip dependent and can not just be changed at will)
#define   STDDR     DDRB
#define   STPORT      PORTB
#define   STPIN     1

// Definition of where the control button is connected
#define   BTNDDR      DDRB
#define   BTNPORT     PORTB
#define   BTNINP      PINB
#define   BTNPIN      2


// The following defines the meaning of status bits in the yackflags and volflags 
// global variables

// Definition of the yackflags variable. These settings get stored in EEPROM when changed.
#define   NOTUSED1    0b00000001  // Available
#define     CONFLOCK    0b00000010  // Configuration locked down
#define   MODE      0b00001100  // 2 bits to define keyer mode (see next section)
#define   SIDETONE    0b00010000  // Set if the chip must produce a sidetone
#define   TXKEY     0b00100000  // Set if the chip keys the transmitter
#define   TXINV     0b01000000  // Set if TX key line is active low
#define   PDLSWAP     0b10000000  // Set if DIT and DAH are swapped

#define   IAMBICA     0b00000000  // IAMBIC A mode
#define   IAMBICB     0b00000100  // IAMBIC B mode (default)
#define   ULTIMATIC   0b00001000  // Ultimatic Mode
#define   DAHPRIO     0b00001100  // Always give DAH priority

#define   FLAGDEFAULT   IAMBICB | TXKEY | SIDETONE

// Definition of volflags variable. These flags do not get stored in EEPROM.
#define   DITLATCH    0b00000001  // Set if DIT contact was closed
#define     DAHLATCH    0b00000010  // Set if DAH contact was closed
#define     SQUEEZED        0b00000011  // DIT and DAH = squeezed
#define   DIRTYFLAG   0b00000100  // Set if cfg data was changed and needs storing
#define     CKLATCH         0b00001000  // Set if the command key was pressed at some point
#define   VSCOPY          0b00110000  // Copies of Sidetone and TX flags from yackflags


// The following defines timing constants. In the default version the keyer is set to operate in
// 10ms heartbeat intervals. If a higher resolution is required, this can be changed to a faster
// beat

// YACK heartbeat frequency (in ms)
#define   YACKBEAT    5
#define   YACKSECS(n)   (n*(1000/YACKBEAT)) // Beats in n seconds
#define   YACKMS(n)   (n/YACKBEAT) // Beats in n milliseconds

// Power save mode
#define     POWERSAVE       // Comment this line if no power save mode required
#define     PSTIME          30 // 30 seconds until automatic powerdown
#define     PWRWAKE         ((1<<PCINT3) | (1<<PCINT4) | (1<<PCINT2)) // Dit, Dah or Command wakes us up..

// These values limit the speed that the keyer can be set to
#define   MAXWPM      50  
#define   MINWPM      5
#define   DEFWPM      20

// Farnsworth parameters
#define     FARNSWORTH      1
#define     WPMSPEED        0
#define     MAXFARN         255

#define   WPMCALC(n)    ((1200/YACKBEAT)/n) // Calculates number of beats in a dot 

#define   DITLEN      1 // Length of a dot
#define   DAHLEN      3 // Length of a dash
#define   IEGLEN      1 // Length of inter-element gap
#define   ICGLEN      3 // Length of inter-character gap
#define   IWGLEN      7 // Length of inter-word gap

// Duration of various internal timings in seconds
#define   TUNEDURATION  20  // Duration of tuning keydown (in seconds)
#define     DEFTIMEOUT      5   // Default timeout 5 seconds
#define     MACTIMEOUT      15  // Timeout after playing back a macro

// The following defines various parameters in relation to the pitch of the sidetone

// CTC mode prescaler.. If changing this, ensure that ctc config
// is adapted accordingly
#define   PRESCALE    8
#define   CTCVAL(n)   ((F_CPU/n/2/PRESCALE)-1) // Defines how to compute CTC setting for
                                                     // a given frequency

// Default sidetone frequency
#define   DEFFREQ     700     // Default sidetone frequency
#define   MAXFREQ     1500    // Maximum frequency
#define   MINFREQ     400     // Minimum frequenc

#define   MAXCTC      CTCVAL(MAXFREQ) // CTC values for the above three values
#define   MINCTC      CTCVAL(MINFREQ) 
#define   DEFCTC      CTCVAL(DEFFREQ)

// The following are various definitions in use throughout the program
#define   RBSIZE      100     // Size of each of the two EEPROM buffers

#define   MAGPAT      0xA5    // If this number is found in EEPROM, content assumed valid

#define   DIT       1
#define   DAH             2

#define   UP        1
#define   DOWN      2

#define   ON        1
#define   OFF       0

#define   RECORD      1
#define   PLAY      2

#define   READ      1
#define   WRITE     2

#define   TRUE            1
#define   FALSE           0


// Generic functionality
#define   SETBIT(ADDRESS,BIT)     (ADDRESS |= (1<<BIT))
#define   CLEARBIT(ADDRESS,BIT)   (ADDRESS &= ~(1<<BIT))

typedef   uint8_t         byte;
typedef   uint16_t        word;


// Forward declarations of public functions
void        yackinit (void);
void        yackchar(char c);
void        yackstring(const char *p);
char        yackiambic(byte ctrl);
void        yackpitch (uint8_t dir);
void        yacktune (void);
void        yackmode (uint8_t mode);
void        yackinhibit (uint8_t mode);
void        yackerror (void);
void        yacktoggle(byte flag);
byte        yackflag(byte flag);
void        yackbeat(void);
void        yackmessage(byte function, byte msgnr);
void        yacksave (void);
byte        yackctrlkey(byte mode);
void        yackreset (void);
word        yackuser (byte func, byte nr, word content);
void        yacknumber(word n);
word        yackwpm(void);
void        yackplay(byte i);
void        yackdelay(byte n);
void        yackfarns(void);
void        yackspeed (byte dir, byte mode);

#ifdef POWERSAVE
void        yackpower(byte n);
#endif

// --- yack.h end ---

//#include <avr/io.h>
//#include <util/delay.h>
//#include <avr/sleep.h>
//#include <avr/wdt.h>
//#include <avr/interrupt.h>

//#define LED   PB4
//#define SW    PB3
//#define BZR   PB0
//#define PIEZO PB1
//#define BTN   PB2

//#define HIGH   1
//#define LOW    0
//#define INPUT  1 
//#define OUTPUT 0


/*!
 
 @file      main.c
 @brief     CW Keyer application
 @author    Jan Lategahn DK3LJ jan@lategahn.com (C) 2010 modified by Jack Welch AI4SV

 This file implements a sample CW keyer application by using the yack.c
 library. It is targeted at the ATTINY45 microcontroller but can be used
 for other ATMEL controllers in the same way. Note the enclosed documentation
 for further defails.
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 @version   0.75
 
 @date      15.10.2010  - Created
 @date      16.12.2010  - Submitted to SVN
 @date      03.10.2013  - Last change
 
 */ 


#ifndef F_CPU
#error F_CPU undefined!! Please define in Makefile
#endif

#include <avr/io.h> 
#include <avr/pgmspace.h>
#include <util/delay.h>
//#include "yack.h"

// Time after which callsign training is assumed complete
#define   TRAINTIMEOUT  10      // 10 Seconds
#define   PITCHREPEAT   10    // 10 e's will be played for pitch adjust
#define     FARNSREPEAT     10      // 10 a's will be played for Farnsworth

// Some texts in Flash used by the application
const char  txok[] PROGMEM    =  "R";
const char  vers[] PROGMEM      = "V0.87";
const char  prgx[] PROGMEM    = "#";
const char  imok[] PROGMEM    = "73";

void setup()
{
  
}

void pitch(void)
/*! 
 @brief     Pitch change mode
 
 This function implements pitch change mode. A series of dots is played and pitch can 
 be adjusted using the paddle levers.
 
 Once 10 dots have been played at the same pitch, the mode terminates
 */
{
  word  timer=PITCHREPEAT;
  
  while (timer)       // while not yet timed out
  {
    timer--;
    yackchar('E');    // play an 'e'
    
    if(!(KEYINP & (1<<DITPIN))) // if DIT was keyed
      {
        yackpitch(DOWN);    // increase the pitch
        timer=PITCHREPEAT;
      }
    
    if(!(KEYINP & (1<<DAHPIN))) // if DAH was keyed
      {
        yackpitch(UP);  // lower the pitch
        timer=PITCHREPEAT;
      }
      
  }
  
}



void setfarns(void)
/*! 
 @brief     Farnsworth change mode
 
 This function implements farnsworth pause change mode. Farnsworth pauses can be added or removed
 with the paddle keys.
 
 */
{
  byte  timer=0;
  
  while (timer++ != FARNSREPEAT)      // while not yet timed out
  {
        
    if(!(KEYINP & (1<<DITPIN))) // if DIT was keyed
      {
        yackspeed(DOWN,FARNSWORTH);   // increase the pitch
        timer=0;
      }
    
    else if(!(KEYINP & (1<<DAHPIN))) // if DAH was keyed
      {
        yackspeed(UP,FARNSWORTH); // lower the pitch
        timer=0;
      }
    
        else 
            yackdelay(IWGLEN);
        
  }
  
}




word lfsr(byte n)
/*! 
 @brief     Simple random number generator
 
 The library function random number generator works fine but is 
 a nightmare in size as it utilizes various multiplications and
 divisions (+600 Byte). This routine implements a LFSR (linear
 feedback shift register) in the Galois method which is good enough 
 for this specific application.
 
 @param n    a number between 2 and 255
 @return     a random number between 0 and n-1
 */
{
  
  static word   lfsr = 0xACE1;
  byte      random;
  
    lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xB400u);    
  
  random = lfsr >> 8; // Byte = upper byte of word 
  
  while (random >= n) random -= n; // Cheap modulo :-)
  
  return random;
  
}





void rndcall(char* call)
/*! 
 @brief     Randomize a callsign
 
 This creates a random callsign with 2 letters + 1 digit + 2 letters
 
 @param call a pointer to a buffer of sufficient size to store the callsign
 */
{
  byte  i;
  
  // Generate a random callsign 2 Char, 1 Digit, 2 Char
  
  for (i=0;i<5;i++)
  {
    if (i == 2)
      call[i]=lfsr(10) + '0';
    else
      call[i]=lfsr(26) + 'A';
    
  }
}




void cstrain(void)
/*! 
 @brief     Callsign trainer mode
 
 This implements callsign training. The keyer plays a random callsign and the 
 user repeats it on the paddle. If a mistake happens, the error prosign is
 sounded, the callsign sent again and the user attempts one more time.
 */
{
  char  call[5];  // A buffer to store the callsign
  char  c;      // The character returned by IAMBIC keyer
  byte  i;      // Counter
  byte  n;      // Playback counter
  word  timer;    // Timeout timer
  
  while(1)  // Endless loop will exit throught RETURN statement only
    
  {
    rndcall(call); // Make up a callsign
    
    i=0; // i counts the number of chracters correctly guessed
    
    while(i<5)
    {
            if (!i) // If nothing guessed yet, play the callsign
      {
        yackdelay(2 * IWGLEN); // Give him some time to breathe b4 next callsign
        for (n=0;n<5;n++)
                {    
          yackchar(call[n]);
                    yackfarns(); // Add potential farnsworth delays
                    if(yackctrlkey(TRUE)) 
                        return; // Abort if requested..
                }
      }
      
      timer = YACKSECS(TRAINTIMEOUT);
      
      do 
      { 
        
        c=yackiambic(OFF);  // Wait for a character 
        yackbeat();     // FSM heartbeat
        timer--;      // Countdown
                
      } while ((!c) && timer && !(yackctrlkey(FALSE))); // Stop when character or timeout
      
      if (timer == 0 || yackctrlkey(TRUE))  // If termination because of timeout 
        return;       // then return
      
      if (call[i] == c)   // Was it the right character?
        i++;        // then increment counter
      else
      {
          yackerror();    // Send an error prosign
        i=0;        // And reset the counter
      }
      
    }
    
    yackchar ('R');
    
  }
}



void beacon(byte mode)
/*! 
 @brief     Beacon mode
 
 This routine can read a beacon transmission interval up to 
 9999 seconds and store it in EEPROM (RECORD mode)
 In PLAY mode, when called in the YACKBEAT loop, it plays back
 message 2 in the programmed interval
 
 @param mode RECORD (read and store the beacon interval) or PLAY (beacon)

 @see main
 
*/
{

  static word interval = 65000; // A dummy value that can not be reached
  static word timer;
  char c;

  
  if (interval == 65000) // 
    interval = yackuser(READ, 1, 0);  
  
  if (mode == RECORD)
  {
    interval = 0; // Reset previous settings
    timer = YACKSECS(DEFTIMEOUT);
    
    yackchar('N');
    
    while(--timer)
    { 
      c=yackiambic(FALSE);
      yackbeat();
      
      if (c>='0' && c<='9')
      {
        interval *= 10;
        interval += c - '0';
        timer = YACKSECS(DEFTIMEOUT);
      }
    }
    
    if (interval >= 0 && interval <= 9999)
    {
      yackuser(WRITE, 1, interval); // Record interval
      yacknumber(interval); // Playback number
    }
    else 
    {
      yackerror();
    }
    
  }

  
  if ((mode == PLAY) && interval)
  {

#ifdef POWERSAVE

        // If we execute this, the interval counter is positive which means we are waiting
        // for a message playback. In this case we must not allow the CPU to enter sleep mode.
        
        yackpower(FALSE); // Inhibit sleep mode
        
#endif
        
    if (timer) timer--; // Countdown until a second has expired
    else 
    {
      timer = YACKSECS(1); // Reset timer
      
      if ((--interval)==0) // Interval was > 0. Did decrement bring it to 0?
      {

        interval = yackuser(READ, 1, 0); // Reset the interval timer
        yackmessage(PLAY,2); // And play message 2

        
      } 
      
    }
        
  }
  
}




void commandmode(void)
/*! 
 @brief     Command mode
 
 This routine implements command mode. Entries are read from the paddle
 and interpreted as commands.
 
*/
{
  
  char  c;        // Character from Morse key
    word    timer;          // Exit timer
  
  yackinhibit(ON);    // Sidetone = on, Keyer = off
  
  yackchar('?');      // Play Greeting
  
    timer = YACKSECS(DEFTIMEOUT); // Time out after 10 seconds
    
    while ((yackctrlkey(TRUE)==0) && (timer-- > 0))
  {
    
    c=yackiambic(OFF);
        if (c) timer = YACKSECS(DEFTIMEOUT); // Reset timeout if character read
        
    yackbeat();
        
        lfsr(255);          // Keep seeding the LFSR so we get different callsigns
    
        if (!yackflag(CONFLOCK)) // No Configuration lock?
        {    
            switch (c) // These are the lockable configuration commands
            {
                    
                case  'R': // Reset
                    yackreset();
                    c = TRUE;
                    break;
                    
                case  'A': // IAMBIC A
                    yackmode(IAMBICA);
                    c = TRUE;
                    break;
                    
                case  'B': // IAMBIC B
                    yackmode(IAMBICB);
                    c = TRUE;
                    break;

                case  'L': // ULTIMATIC
                    yackmode(ULTIMATIC);
                    c = TRUE;
                    break;
                    
                case    'D': // DAHPRIO
                    yackmode(DAHPRIO);
                    c = TRUE;
                    break;
                    
                case  'X': // Paddle swapping
                    yacktoggle(PDLSWAP);
                    c = TRUE;
                    break;
                    
                case    'S': // Sidetone toggle
                    yacktoggle(SIDETONE);
                    c = TRUE;
                    break;
                    
                case    'K': // TX keying toggle
                    yacktoggle(TXKEY);
                    c = TRUE;
                    break;
                    
                case    'Z': // Farnsworth pause
                    setfarns();
                    c = TRUE;
                    break;
                        
                case    'F': // TX level inverter toggle
                    yacktoggle(TXINV);
                    c = TRUE;
                    break;
                    
                case  '1': // Record Macro 1
                    yackchar('1');
                    yackmessage(RECORD,1); 
                    c = TRUE;
                    break;
                    
                case  '2': // Record Macro 2
                    yackchar('2');
                    yackmessage(RECORD,2); 
                    c = TRUE;
                    break;
              
               case  '3': // Record Macro 3
                    yackchar('3');
                    yackmessage(RECORD,3); 
                    c = TRUE;
                    break;

              case  '4': // Record Macro 4
                    yackchar('4');
                    yackmessage(RECORD,4); 
                    c = TRUE;
                    break;     
                                 
                case  'N': // Automatic Beacon
                    beacon(RECORD);
                    c = TRUE;
                    break;
                    
            }
            
        }
        
        switch (c) // Commands that can be used anytime
        {
                
            case    'V': // Version
                yackstring(vers);
                c = TRUE;
                break;
    
                
            case  'P': // Pitch
                pitch();
                c = TRUE;
                break;
                
            case  'U': // Tune
                yackinhibit(OFF);
                yacktune();
                yackinhibit(ON);
                c = TRUE;
                break;
                
            case  'C': // Callsign training
                cstrain();
                c = TRUE;
                break;
                
            case    '0': // Lock changes
                yacktoggle(CONFLOCK);
                c = TRUE;
                break;
                
            case  'E': // Playback Macro 1
                yackinhibit(OFF);
                yackmessage(PLAY,1);
                yackinhibit(ON);
                timer = YACKSECS(MACTIMEOUT);
                c = FALSE;
                break;
                
            case  'T': // Playback Macro 2
                yackinhibit(OFF);
                yackmessage(PLAY,2);
                yackinhibit(ON);
                timer = YACKSECS(MACTIMEOUT);
                c = FALSE;
                break;

         case  'I': // Playback Macro 3
                yackinhibit(OFF);
                yackmessage(PLAY,3);
                yackinhibit(ON);
                timer = YACKSECS(MACTIMEOUT);
                c = FALSE;
                break;
       
      case  'M': // Playback Macro 4
                yackinhibit(OFF);
                yackmessage(PLAY,4);
                yackinhibit(ON);
                timer = YACKSECS(MACTIMEOUT);
                c = FALSE;
                break;  
                
            case  'N': // Automatic Beacon
                beacon(RECORD);
                c = TRUE;
                break;
                
            case    'W': // Query WPM
                yacknumber(yackwpm());
                c = TRUE;
                break;
                
                
        }
        
        if (c == TRUE) // If c still contains a string, the command was not handled properly
            yackstring(txok);
        else if (c)
            yackerror();
        
            
  }
        
        
  yackstring(prgx);       // Sign off

  yackinhibit(OFF);   // Back to normal mode
  
}


void loop() 
/*! 
 @brief     Trivial main routine
 
 Yack library is initialized, command mode is entered on request and both 
 beacon and keyer routines are called in 10 ms intervals.
 
 @return Not relevant
*/
{
  
  yackinit();           // Initialize YACK hardware
  
  yackinhibit(ON);  //side tone greeting to confirm the unit is alive and kicking
  yackstring(imok);  // greeting is '73' at startup (but outputs over TX pin PB0)
  yackinhibit(OFF);
  
  while(1) // Endless core loop of the keyer app
  {
    
    if (yackctrlkey(TRUE)) // If command key pressed, go to command mode
      commandmode();
    
        yackbeat();
    beacon(PLAY); // Play beacon if requested
        yackiambic(OFF);
        
  }
  
  
}


/// --- yack.c begin ---
/*!
 
 @file      yack.c
 @brief     CW Keyer library
 @author    Jan Lategahn DK3LJ jan@lategahn.com (C) 2011; modified by Jack Welch AI4SV
 
 @version   0.75
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 @date      15.10.2010  - Created
 @date      03.10.2013  - Last update
 
 @todo      Make the delay dependent on T/C 1 

*/ 


#include <avr/io.h> 
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdint.h>
//#include "yack.h"

// Forward declaration of private functions
static      void key( byte mode); 
static      char morsechar(byte buffer);
static      void keylatch(void);

// Enumerations

enum FSMSTATE { 
                IDLE,   //!< Not keyed, waiting for paddle
                KEYED,  //!< Keyed, waiting for duration of current element
                IEG     //!< In Inter-Element-Gap 
              };   

// Module local definitions

static    byte  yackflags;    // Permanent (stored) status of module flags
static    byte  volflags=0;   // Temporary working flags (volatile)
static    word  ctcvalue;   // Pitch
static    word  wpmcnt;     // Speed
static      byte    wpm;            // Real wpm
static      byte    farnsworth;     // Additional Farnsworth pause

// EEPROM Data

byte    magic EEMEM = MAGPAT; // Needs to contain 'A5' if mem is valid
byte    flagstor EEMEM = ( IAMBICB | TXKEY | SIDETONE); //  Defaults  
word    ctcstor EEMEM = DEFCTC; // Pitch = 700Hz
byte    wpmstor EEMEM = DEFWPM; // 15 WPM
byte        fwstor  EEMEM = 0; // No farnsworth pause
word    user1 EEMEM = 0; // User storage
word    user2 EEMEM = 0; // User storage

char    eebuffer1[100] EEMEM = "message 1";
char    eebuffer2[100] EEMEM = "message 2";
char    eebuffer3[100] EEMEM = "message 3";
char    eebuffer4[100] EEMEM = "message 4"; 

// Flash data

//! Morse code table in Flash

//! Encoding: Each byte is read from the left. 0 stands for a dot, 1
//! stands for a dash. After each played element the content is shifted
//! left. Playback stops when the leftmost bit contains a "1" and the rest
//! of the bits are all zero.
//!
//! Example: A = .-
//! Encoding: 01100000
//!           .-
//!             | This is the stop marker (1 with all trailing zeros)

const byte morse[] PROGMEM = 
{
  
  0b11111100, // 0
  0b01111100, // 1
  0b00111100, // 2
  0b00011100, // 3
  0b00001100, // 4
  0b00000100, // 5
  0b10000100, // 6
  0b11000100, // 7
  0b11100100, // 8
  0b11110100, // 9
  0b01100000, // A
  0b10001000, // B
  0b10101000, // C
  0b10010000, // D
  0b01000000, // E
  0b00101000, // F
  0b11010000, // G
  0b00001000, // H
  0b00100000, // I                                
  0b01111000, // J
  0b10110000, // K
  0b01001000, // L
  0b11100000, // M
  0b10100000, // N
  0b11110000, // O
  0b01101000, // P
  0b11011000, // Q
  0b01010000, // R
  0b00010000, // S
  0b11000000, // T
  0b00110000, // U
  0b00011000, // V
  0b01110000, // W
  0b10011000, // X
  0b10111000, // Y
  0b11001000, // Z
  0b00110010, // ?
  0b01010110, // .
  0b10010100, // /
  0b11101000, // ! (American Morse version, commonly used in ham circles)
  0b11001110, // ,
  0b11100010, // :
  0b10101010, // ;
  0b01001010, // "
  0b00010011, // $
  0b01111010, // ' (Apostrophe)
  0b10110100, // ( or [ (also prosign KN)
  0b10110110, // ) or ]
  0b10000110, // - (Hyphen or single dash)
  0b01101010, // @
  0b00110110, // _ (Underline)
  0b01010010, // Paragaraph break symbol
  0b10001100, // = and BT
  0b00010110, // SK
  0b01010100, // + and AR
  0b10001011, // BK
  0b01000100, // AS
  0b10101100, // KA (also ! in alternate Continental Morse)
  0b00010100, // VE
  0b01011000  // AA
  
};


// The special characters at the end of the above table can not be decoded
// without a small table to define their content. # stands for SK, $ for AR

// To add new characters, add them in the code table above at the end and below
// Do not forget to increase the legth of the array..

// was: const char spechar[6] PROGMEM = "?=.#$/";
// was const char spechar[] PROGMEM = "?=.#$/";
const char spechar[24] PROGMEM = "?./!,:;~$^()-@_|=#+*%&<>";


// Functions

// ***************************************************************************
// Control functions
// ***************************************************************************

void yackreset (void)
/*! 
 @brief     Sets all yack parameters to standard values

 This function resets all YACK EEPROM settings to their default values as 
 stored in the .h file. It sets the dirty flag and calls the save routine
 to write the data into EEPROM immediately.
*/
{

  ctcvalue=DEFCTC; // Initialize to 700 Hz
    wpm=DEFWPM; // Init to default speed
  wpmcnt=(1200/YACKBEAT)/DEFWPM; // default speed
    farnsworth=0; // No Farnsworth gap
  yackflags = FLAGDEFAULT;  

  volflags |= DIRTYFLAG;
  yacksave(); // Store them in EEPROM

}


void yackinit (void)
/*! 
 @brief     Initializes the YACK library
 
 This function initializes the keyer hardware according to configurations in the .h file.
 Then it attempts to read saved configuration settings from EEPROM. If not possible, it
 will reset all values to their defaults.
 This function must be called once before the remaining fuctions can be used.
*/
{
  
  byte magval;
  
  // Configure DDR. Make OUT and ST output ports
  SETBIT (OUTDDR,OUTPIN);    
  SETBIT (STDDR,STPIN);
  
  // Raise internal pullups for all inputs
  SETBIT (KEYPORT,DITPIN);  
  SETBIT (KEYPORT,DAHPIN);
  SETBIT (BTNPORT,BTNPIN);
  
  magval = eeprom_read_byte(&magic); // Retrieve magic value
  
  if (magval == MAGPAT) // Is memory valid
  {
    ctcvalue = eeprom_read_word(&ctcstor); // Retrieve last ctc setting
    wpm = eeprom_read_byte(&wpmstor); // Retrieve last wpm setting
        wpmcnt=(1200/YACKBEAT)/wpm; // Calculate speed
    farnsworth = eeprom_read_byte(&fwstor); // Retrieve last wpm setting  
    yackflags = eeprom_read_byte(&flagstor); // Retrieve last flags 
  }
  else
  {
    yackreset();
  } 
  
  yackinhibit(OFF);

#ifdef POWERSAVE
    
    PCMSK |= PWRWAKE;    // Define which keys wake us up
    GIMSK |= (1<<PCIE);  // Enable pin change interrupt
    
#endif
    
    // Initialize timer1 to serve as the system heartbeat
    // CK runs at 1MHz. Prescaling by 64 makes that 15625 Hz.
    // Counting 78 cycles of that generates an overflow every 5ms
    
    OCR1C = 78; // 77 counts per cycle
    TCCR1 |= (1<<CTC1) | 0b00000111 ; // Clear Timer on match, prescale ck by 64
    OCR1A = 1; // CTC mode does not create an overflow so we use OCR1A
    
}

#ifdef POWERSAVE

ISR(PCINT0_vect)
/*! 
 @brief     A dummy pin change interrupt
 
 This function is called whenever the system is in sleep mode and there is a level change on one of the contacts 
 we are monitoring (dit, dah and the command key). As all handling is already taken care of by polling in the main 
 routines, there is nothing we need to do here.
 */
{
    // Nothing to do here. All we want is to wake up..
}


void yackpower(byte n)
/*! 
 @brief     Manages the power saving mode
 
 This is called in yackbeat intervals with either a TRUE or FALSE as parameter. Whenever the
 parameter is TRUE a beat counter is advanced until the timeout level is reached. When timeout
 is reached, the chip shuts down and will only wake up again when issued a level change interrupt on
 either of the input pins.
 
 When the parameter is FALSE, the counter is reset.
 
 @param n   TRUE: OK to sleep, FALSE: Can not sleep now
 
*/

{
    static uint32_t shdntimer=0;
    
    if (n) // True = we could go to sleep
    {
        if(shdntimer++ == YACKSECS(PSTIME))
        {
            shdntimer=0; // So we do not go to sleep right after waking up..

            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // MAP          sleep_bod_disable();
            sleep_enable();
            sei();
            sleep_cpu();
            cli();
            
            // There is no technical reason to CLI here but it avoids hitting the ISR every time
            // the paddles are touched. If the remaining code needs the interrupts this is OK to remove.
            
        }
        
    }
    else // Passed parameter is FALSE
    {
        shdntimer=0;
    }

}

#endif



void yacksave (void)
/*! 
 @brief     Saves all permanent settings to EEPROM
 
 To save EEPROM write cycles, writing only happens when the flag DIRTYFLAG is set.
 After writing the flag is cleared
 
 @callergraph
 
 */
{
  
  if(volflags & DIRTYFLAG) // Dirty flag set?
  { 
    
    eeprom_write_byte(&magic, MAGPAT);
    eeprom_write_word(&ctcstor, ctcvalue);
    eeprom_write_byte(&wpmstor, wpm);
    eeprom_write_byte(&flagstor, yackflags);
        eeprom_write_byte(&fwstor, farnsworth);
    
    volflags &= ~DIRTYFLAG; // Clear the dirty flag
  }
  
}



void yackinhibit (byte mode)
/*! 
 @brief     Inhibits keying during command phases
 
 This function is used to inhibit and re-enable TX keying (if configured) and enforce the internal 
 sidetone oscillator to be active so that the user can communicate with the keyer.
 
 @param mode   ON inhibits keying, OFF re-enables keying 
 
 */
{
  
  if (mode)
  {
    volflags &= ~(TXKEY | SIDETONE);
    volflags |= SIDETONE;
  }
  
  else
    
  {
    volflags &= ~(TXKEY | SIDETONE);
    volflags |= (yackflags & (TXKEY | SIDETONE));
        key(UP);
  }
  
}



word yackuser (byte func, byte nr, word content)
/*! 
 @brief     Saves user defined settings
 
 The routine using this library is given the opportunity to save up to two 16 bit sized
 values in EEPROM. In case of the sample main function this is used to store the beacon interval 
 timer value. The routine is not otherwise used by the library.
 
 @param func    States if the data is retrieved (READ) or written (WRITE) to EEPROM
 @param nr      1 or 2 (Number of user storage to access)
 @param content The 16 bit word to write. Not used in read mode.
 @return        The content of the retrieved value in read mode.
 
 */
{
  
    
  if (func == READ)
  {
    if (nr == 1) 
      return (eeprom_read_word(&user1));
    else if (nr == 2)
      return (eeprom_read_word(&user2));
  }
  
  if (func == WRITE)
  {
        
    if (nr == 1)
      eeprom_write_word(&user1, content);
    else if (nr == 2)
      eeprom_write_word(&user2, content);
  }

    return (FALSE);
    
}



word yackwpm(void)
/*! 
 @brief     Retrieves the current WPM speed
 
 This function delivers the current WPM speed. 

 @return        Current speed in WPM
 
 */
{
    
    return wpm; 
    
}


void yackspeed (byte dir, byte mode)
/*! 
 @brief     Increases or decreases the current WPM speed
 
 The amount of increase or decrease is in amounts of wpmcnt. Those are close to real
 WPM in a 10ms heartbeat but can significantly differ at higher heartbeat speeds.
 
 @param dir     UP (faster) or DOWN (slower)
 
 */
{
    
    if (mode == FARNSWORTH)
    {
        if ((dir == UP) && (farnsworth > 0))
            farnsworth--;
        
        if ((dir == DOWN) && (farnsworth < MAXFARN))
            farnsworth++;
    }
    else // WPMSPEED
    {
        if ((dir == UP) && (wpm < MAXWPM))
            wpm++;
        
        if ((dir == DOWN) && (wpm > MINWPM))
            wpm--;
        
        wpmcnt=(1200/YACKBEAT)/wpm; // Calculate beats

  }
  
  volflags |= DIRTYFLAG; // Set the dirty flag  
    
    yackplay(DIT);
    yackdelay(IEGLEN);  // Inter Element gap  
    yackplay(DAH);
    yackdelay(ICGLEN);  // Inter Character gap  
    yackfarns(); // Additional Farnsworth delay
    
}



void yackbeat (void)
/*! 
 @brief     Heartbeat delay
 
 Several functions in the keyer are timing dependent. The most prominent example is the
 yackiambic function that implements the IAMBIC keyer finite state machine.
 The same expects to be called in intervals of YACKBEAT milliseconds. How this is 
 implemented is left to the user. In a more complex application this would be done
 using an interrupt or a timer. For simpler cases this is a busy wait routine
 that delays exactly YACKBEAT ms.
 
 */
{
    while((TIFR & (1<<OCF1A)) == 0); // Wait for Timeout
    TIFR |= (1<<OCF1A);                // Reset output compare flag
}



void yackpitch (byte dir)
/*! 
 @brief     Increases or decreases the sidetone pitch
 
 Changes are done not in Hz but in ctc control values. This is to avoid extensive 
 calculations at runtime. As is all calculations are done by the preprocessor.
 
 @param dir     UP or DOWN
 
 */
{
  if (dir == UP)
    ctcvalue--;
  if (dir == DOWN)
    ctcvalue++;
  
  if (ctcvalue < MAXCTC)
    ctcvalue = MAXCTC;
  
  if (ctcvalue > MINCTC)
    ctcvalue = MINCTC;
  
  volflags |= DIRTYFLAG; // Set the dirty flag  
  
}




void yacktune (void)
/*! 
 @brief     Activates Tuning mode
 
 This produces a solid keydown for TUNEDURATION seconds. After this the TX is unkeyed.
 The same can be achieved by presing either the DIT or the DAH contact or the control key.
 
*/
{
  word timer = YACKSECS(TUNEDURATION);
  
  key(DOWN);
  
  while(timer && (KEYINP & (1<<DITPIN)) && (KEYINP & (1<<DAHPIN)) && !yackctrlkey(TRUE) )
  {
    timer--;
    yackbeat();
  }
  
  key(UP);
}




void yackmode (byte mode)
/*! 
 @brief     Sets the keyer mode (e.g. IAMBIC A)
 
 This allows to set the content of the two mode bits in yackflags. Currently only
 two modes are supported, IAMBIC A and IAMBIC B.
 
 @param mode    IAMBICA or IAMBICB
 @return    TRUE is all was OK, FALSE if configuration lock prevented changes
 
 */
{

  yackflags &= ~MODE;
  yackflags |= mode;
  
  volflags |= DIRTYFLAG; // Set the dirty flag  

}


byte yackflag(byte flag)
/*! 
 @brief     Query feature flags
 
 @param flag A byte which indicate which flags are to be queried 
 @return     0 if the flag(s) were clear, >0 if flag(s) were set
 
 */
{
    return yackflags & flag;
}



void yacktoggle(byte flag)
/*! 
 @brief     Toggle feature flags
 
 When passed one (or more) flags, this routine flips the according bit in yackflags and
 thereby enables or disables the corresponding feature.
 
 @param flag    A byte where any bit to toggle is set e.g. SIDETONE 
 @return    TRUE if all was OK, FALSE if configuration lock prevented changes
 
 */
{
    
    yackflags ^= flag;      // Toggle the feature bit
    volflags |= DIRTYFLAG;  // Set the dirty flag 

}




void yackerror (void)
/*! 
 @brief     Creates a series of 8 dits
 
 The error prosign (8 dits) can not be encoded in our coding table. A call to this
 function produces it..
 
 */
{
  byte i;
  
  for (i=0;i<8;i++)
  {
    yackplay(DIT);
    yackdelay(DITLEN);
  }
  yackdelay(DAHLEN);
  
}




// ***************************************************************************
// CW Playback related functions
// ***************************************************************************

static void key(byte mode) 
/*! 
 @brief     Keys the transmitter and produces a sidetone
 
 .. but only if the corresponding functions (TXKEY and SIDETONE) have been set in
 the feature register. This function also handles a request to invert the keyer line
 if necessary (TXINV bit).
 
 This is a private function.

 @param mode    UP or DOWN
 
 */
{
  
    if (mode == DOWN) 
    {
        if (volflags & SIDETONE) // Are we generating a Sidetone?
        {
            OCR0A = ctcvalue;   // Then switch on the Sidetone generator
            OCR0B = ctcvalue;
            
            // Activate CTC mode
            TCCR0A |= (1<<COM0B0 | 1<<WGM01);
            
            // Configure prescaler
            TCCR0B = 1<<CS01;
        }
        
        if (volflags & TXKEY) // Are we keying the TX?
        {
            if (yackflags & TXINV) // Do we need to invert keying?
                CLEARBIT(OUTPORT,OUTPIN);
            else
                SETBIT(OUTPORT,OUTPIN);
        }

    }
    
    if (mode == UP) 
    {

        if (volflags & SIDETONE) // Sidetone active?
        {
            TCCR0A = 0;
            TCCR0B = 0;
        }
        
        if (volflags & TXKEY) // Are we keying the TX?
        {
            if (yackflags & TXINV) // Do we need to invert keying?
                SETBIT(OUTPORT,OUTPIN);
            else
                CLEARBIT(OUTPORT,OUTPIN);
        }

    }
    
}



void yackfarns(void)
/*! 
 @brief     Produces an additional waiting delay for farnsworth mode.
 
 */
{
  
    word i=farnsworth;
  
  while (i--)
  {
      yackdelay(1);
  }
  
}



void yackdelay(byte n)
/*! 
 @brief     Produces an active waiting delay for n dot counts
 
 This is used during the playback functions where active waiting is needed
 
 @param n   number of dot durations to delay (dependent on current keying speed!
 
 */
{
  
  byte i=n;
  byte x;
  
  while (i--)
  {
    x=wpmcnt;
    while (x--)    
      yackbeat();
  }
  
}




void yackplay(byte i) 
/*! 
 @brief     Key the TX / Sidetone for the duration of a dit or a dah
 
 @param i   DIT or DAH
 
 */
{
  
    key(DOWN); 

#ifdef POWERSAVE
    
    yackpower(FALSE); // Avoid powerdowns when keying
    
#endif
    
  switch (i)
  {
    case DAH:
      yackdelay(DAHLEN);
      break;
      
    case DIT:
      yackdelay(DITLEN);
      break;
  }
    
    key(UP);
  
}




void yackchar(char c)
/*! 
 @brief     Send a character in morse code
 
 This function translates a character passed as parameter into morse code using the 
 translation table in Flash memory. It then keys transmitter / sidetone with the characters
 elements and adds all necessary gaps (as if the character was part of a longer word).
 
 If the character can not be translated, nothing is sent.
 
 If a space is received, an interword gap is sent.
  
 @param c   The character to send
 
*/


{
  byte  code=0x80; // 0x80 is an empty morse character (just eoc bit set)
  byte  i; // a counter
  
  // First we need to map the actual character to the encoded morse sequence in
  // the array "morse"
  if(c>='0' && c<='9') // Is it a numerical digit?
    code = pgm_read_byte(&morse[c-'0']); // Find it in the beginning of array
    
  if(c>='a' && c<='z') // Is it a character?
    code = pgm_read_byte(&morse[c-'a'+10]); // Find it from position 10
  
  if(c>='A' && c<='Z') // Is it a character in upper case?
    code = pgm_read_byte(&morse[c-'A'+10]); // Same as above
  
  // Last we need to handle special characters. There is a small char
  // array "spechar" which contains the characters for the morse elements
  // at the end of the "morse" array (see there!)
  for(i=0;i<sizeof(spechar);i++) // Read through the array
    if (c == pgm_read_byte(&spechar[i])) // Does it contain our character
      code = pgm_read_byte(&morse[i+36]); // Map it to morse code
  
  if(c==' ') // Do they want us to transmit a space (a gap of 7 dots)
    yackdelay(IWGLEN-ICGLEN); // ICG was already played after previous char
  else
  {
      while (code != 0x80) // Stop when EOC bit has reached MSB
      {
      if (yackctrlkey(FALSE)) // Stop playing if someone pushes key
        return;
      
        if (code & 0x80)  // MSB set ?
            yackplay(DAH);      // ..then play a dash
        else        // MSB cleared ?
            yackplay(DIT);    // .. then play a dot
      
        yackdelay(IEGLEN);  // Inter Element gap  
            
        code = code << 1; // Shift code on position left (to next element)
      }
    
      yackdelay(ICGLEN - IEGLEN); // IEG was already played after element

        yackfarns(); // Insert another gap for farnsworth keying

  }
  
}



void yackstring(const char *p)
/*! 
 @brief     Sends a 0-terminated string in CW which resides in Flash
 
 Reads character by character from flash, translates into CW and keys the transmitter
 and/or sidetone depending on feature bit settings.
 
 @param p   Pointer to string location in FLASH 
 
 */
{
  
  char c;
  
  while ((c = pgm_read_byte(p++))&& !(yackctrlkey(FALSE)) )
    // While end of string in flash not reached and ctrl not pressed
    yackchar(c);            // Play the read character
                // abort now if someone presses command key
  
}



void yacknumber(word n)
/*! 
 @brief     Sends a number in CW
 
 Transforms a number up to 65535 into its digits and sends them in CW
 
 @param n   The number to send
 
 */

{
    
    char buffer[5];
    byte i = 0;
  
  while (n) // Until nothing left
  {
    buffer[i++] = n%10+'0'; // Store rest of division by 10
    n /= 10;                // Divide by 10
  }
  
    while (i)
    {
        yackchar(buffer[--i]);
    }
    
    yackchar (' ');
    
}



// ***************************************************************************
// CW Keying related functions
// ***************************************************************************


static void keylatch(void)
/*! 
 @brief     Latches the status of the DIT and DAH paddles
 
 If either DIT or DAH are keyed, this function sets the corresponding bit in 
 volflags. This is used by the IAMBIC keyer to determine which element needs to 
 be sounded next.
 
 This is a private function.

 */
{
  
  byte  swap;  // Status of swap flag
  
  swap    = ( yackflags & PDLSWAP);
  
  if (!( KEYINP & (1<<DITPIN)))
    volflags |= (swap?DAHLATCH:DITLATCH);
  
  if (!( KEYINP & (1<<DAHPIN)))
    volflags |= (swap?DITLATCH:DAHLATCH);
  
}



byte yackctrlkey(byte mode)
/*! 
 @brief     Scans for the Control key
 
 This function is regularly called at different points in the program. In a normal case
 it terminates instantly. When the command key is found to be closed, the routine idles
 until it is released again and returns a TRUE return value.
 
 If, during the period where the contact was closed one of the paddles was closed too,
 the wpm speed is changed and the keypress not interpreted as a Control request. 

 @param mode    TRUE if caller has taken care of command key press, FALSE if not
 @return        TRUE if a press of the command key is not yet handled. 
 
 @callergraph
 
 */
{
  
  byte volbfr;
  
    volbfr = volflags; // Remember current volatile settings
    
  if (!(BTNINP & (1<<BTNPIN))) // If command button is pressed
  {
        
        volbfr |= CKLATCH; // Set control key latch
    
    // Apparently the control key has been pressed. To avoid bouncing
    // We will now wait a short while and then busy wait until the key is
    // released.
    // Should we find that someone is keying the paddle, let him change
    // the speed and pretend ctrl was never pressed in the first place..

    yackinhibit(ON); // Stop keying, switch on sidetone.
    
    _delay_ms(50);
    
    while(!(BTNINP & (1<<BTNPIN))) // Busy wait for release
    {
            
      if (!( KEYINP & (1<<DITPIN))) // Someone pressing DIT paddle
      {
        yackspeed(DOWN,WPMSPEED);
                volbfr &= ~(CKLATCH); // Ignore that control key was pressed
      } 
      
      if (!( KEYINP & (1<<DAHPIN))) // Someone pressing DAH paddle
      {
        yackspeed(UP,WPMSPEED);
                volbfr &= ~(CKLATCH);
      } 
      
    }
        
    _delay_ms(50); // Trailing edge debounce  
    
  }

    volflags = volbfr; // Restore previous state

    if (mode==TRUE) // Does caller want us to reset latch?
    {
        volflags &= ~(CKLATCH);
    }
    
    yacksave(); // In case we had a speed change
    
  return((volbfr&CKLATCH)!=0); // Tell caller if we had a ctrl button press
  
}



static char morsechar(byte buffer)
/*! 
 @brief     Reverse maps a combination of dots and dashes to a character
 
 This routine is passed a sequence of dots and dashes in the format we use for morse
 character encoding (see top of this file). It looks up the corresponding character in
 the Flash table and returns it to the caller. 
 
 This is a private function.
 
 @param buffer    A character in YACK CW notation
 @return          The mapped character or /0 if no match was found  
 
 */
{
  byte i;
  
  for(i=0;i<sizeof(morse);i++)
  {
    
    if (pgm_read_byte(&morse[i]) == buffer)
    {
      if (i < 10) return ('0' + i);     // First 10 chars are digits
      if (i < 36) return ('A' + i - 10);  // Then follow letters
      return (pgm_read_byte(&spechar[i - 36])); // Then special chars
    }
    
  }
  
  return '\0';
}



void yackmessage(byte function, byte msgnr)
/*! 
 @brief     Handles EEPROM stored CW messages (macros)
 
 When called in RECORD mode, the function records a message up to 100 characters and stores it in 
 EEPROM. The routine stops recording when timing out after DEFTIMEOUT seconds. Recording
 can be aborted using the control key. If more than 100 characters are recorded, the error prosign
 is sounded and recording starts from the beginning. After recording and timing out the message is played
 back once before it is stored. To erase a message, do not key one.
 
 When called in PLAY mode, the message is just played back. Playback can be aborted using the command
 key.
 
 @param     function    RECORD or PLAY
 @param     msgnr       1 or 2
 @return    TRUE if all OK, FALSE if lock prevented message recording
 
 */
{
  unsigned char rambuffer[RBSIZE];  // Storage for the message
  unsigned char c;          // Work character
  
  word      extimer = 0;    // Detects end of message (10 sec)
  
  byte      i = 0;          // Pointer into RAM buffer
  byte      n;          // Generic counter
  
  if (function == RECORD)
  {

    extimer = YACKSECS(DEFTIMEOUT); // 5 Second until message end
      while(extimer--)  // Continue until we waited 10 seconds
      {
      if (yackctrlkey(FALSE)) return;
      
      if ((c = yackiambic(ON))) // Check for a character from the key
      {
        rambuffer[i++] = c; // Add that character to our buffer
        extimer = YACKSECS(DEFTIMEOUT); // Reset End of message timer
      }
      
      if (i>=RBSIZE) // End of buffer reached?
      {
        yackerror();
        i = 0;
      }
      
      yackbeat(); // 10 ms heartbeat
    } 
    
    // Extimer has expired. Message has ended
    
    if(i) // Was anything received at all?
    {
      rambuffer[--i] = 0; // Add a \0 end marker over last space
      
      // Replay the message
      for (n=0;n<i;n++)
        yackchar(rambuffer[n]);
      
      // Store it in EEPROM

      if (msgnr == 1)
          eeprom_write_block(rambuffer,eebuffer1,RBSIZE);
      if (msgnr == 2)
          eeprom_write_block(rambuffer,eebuffer2,RBSIZE);
      if (msgnr == 3)
          eeprom_write_block(rambuffer,eebuffer3,RBSIZE);
      if (msgnr == 4)
          eeprom_write_block(rambuffer,eebuffer4,RBSIZE);
    }
    else
      yackerror();
  }
  
  
  if (function == PLAY)
  {
    // Retrieve the message from EEPROM
    // Retrieve the message from EEPROM
    if (msgnr == 1)
        eeprom_read_block(rambuffer,eebuffer1,RBSIZE);
    if (msgnr == 2)
        eeprom_read_block(rambuffer,eebuffer2,RBSIZE);
    if (msgnr == 3)
        eeprom_read_block(rambuffer,eebuffer3,RBSIZE);
    if (msgnr == 4)
        eeprom_read_block(rambuffer,eebuffer4,RBSIZE);
    
    // Replay the message
    for (n=0;(c=rambuffer[n]);n++) // Read until end of message
      yackchar(c); // play it back 
    
  }
  
}




char yackiambic(byte ctrl)
/*! 
 @brief     Finite state machine for the IAMBIC keyer
 
 If IAMBIC (squeeze) keying is requested, this routine, which usually terminates
 immediately needs to be called in regular intervals of YACKBEAT milliseconds.
 
 This can happen though an outside busy waiting loop or a counter mechanism.
 
 @param ctrl    ON if the keyer should recognize when a word ends. OFF if not.
 @return        The character if one was recognized, /0 if not
 
 */
{
  
  static enum FSMSTATE  fsms = IDLE;  // FSM state indicator
  static    word    timer;      // A countdown timer
  static    byte    lastsymbol;   // The last symbol sent
  static    byte    buffer = 0;   // A place to store a sent char
  static    byte    bcntr = 0;    // Number of elements sent
  static    byte    iwgflag = 0;  // Flag: Are we in interword gap?
    static      byte        ultimem = 0;    // Buffer for last keying status
        char    retchar;    // The character to return to caller
  
  // This routine is called every YACKBEAT ms. It starts with idle mode where
  // the morse key is polled. Once a contact close is sensed, the TX key is 
  // closed, the sidetone oscillator is fired up and the FSM progresses
  // to the next state (KEYED). There it waits for the timer to expire, 
  // afterwards progressing to IEG (Inter Element Gap).
  // Once the IEG has completed, processing returns to the IDLE state.
  
  // If the FSM remains in idle state long enough (one dash time), the
  // character is assumed to be complete and a decoding is attempted. If
  // succesful, the ascii code of the character is returned to the caller
  
  // If the FSM remains in idle state for another 4 dot times (7 dot times 
  // altogether), we assume that the word has ended. A space char
  // is transmitted in this case.
  
  if (timer) timer--; // Count down
  
  if (ctrl == OFF) iwgflag = 0; // No space detection
  
  switch (fsms)
  {
      
    case IDLE:
      
      keylatch();
           
#ifdef POWERSAVE            
            
            yackpower(TRUE); // OK to go to sleep when here.

#endif            
            
            // Handle latching logic for various keyer modes
            switch (yackflags & MODE)
            {
                case IAMBICA:
                case IAMBICB:
                    // When the paddle keys are squeezed, we need to ensure that
                    // dots and dashes are alternating. To do that, whe delete
                    // any latched paddle of the same kind that we just sent.
                    // However, we only do this ONCE
                    
                    volflags &= ~lastsymbol;
                    lastsymbol = 0;
                    break;                    
                 
                case ULTIMATIC:
                    // Ultimatic logic: The last paddle to be active will be repeated indefinitely
                    // In case the keyer is squeezed right out of idle mode, we just send a DAH 
                    if ((volflags & SQUEEZED) == SQUEEZED) // Squeezed?
                    {
                        if (ultimem)
                          volflags &= ~ultimem; // Opposite symbol from last one
                        else
                          volflags &= ~DITLATCH; // Reset the DIT latch
                    }
                    else
                    {
                        ultimem = volflags & SQUEEZED; // Remember the last single key
                    }

                    break;
                            
                case DAHPRIO:            
                    // If both paddles pressed, DAH is given priority
                    if ((volflags & SQUEEZED) == SQUEEZED)
                    {
                        volflags &= ~DITLATCH; // Reset the DIT latch
                    }
                    break;
            }        
            
            
      // The following handles the inter-character gap. When there are
      // three (default) dot lengths of space after an element, the
      // character is complete and can be returned to caller
      if (timer == 0 && bcntr != 0) // Have we idled for 3 dots
        // and is there something to decode?
      {
        buffer = buffer << 1;   // Make space for the termination bit
        buffer |= 1;        // The 1 on the right signals end
        buffer = buffer << (7-bcntr); // Shift to left justify
        retchar = morsechar(buffer); // Attempt decoding
        buffer = bcntr = 0;     // Clear buffer
        timer = (IWGLEN - ICGLEN) * wpmcnt; // If 4 further dots of gap,
        // this might be a Word gap.
        iwgflag = 1;                // Signal we are waiting for IWG                          
        return (retchar);     // and return decoded char
      }
      
      // This handles the Inter-word gap. Already 3 dots have been
      // waited for, if 4 more follow, interpret this as a word end
      if (timer == 0 && iwgflag) // Have we idled for 4+3 = 7 dots?
      {
        iwgflag = 0;   // Clear Interword Gap flag
        return (' ');  // And return a space
      }
      
      // Now evaluate the latch and determine what to send next
      if ( volflags & (DITLATCH | DAHLATCH)) // Anything in the latch?
      {
        iwgflag = 0; // No interword gap if dit or dah
                bcntr++;  // Count that we will send something now
        buffer = buffer << 1; // Make space for the new character
        
        if (volflags & DITLATCH) // Is it a dit?
        {
          timer = DITLEN * wpmcnt; // Duration = one dot time
          lastsymbol = DITLATCH; // Remember what we sent
        }
        else // must be a DAH then..
        {
          timer = DAHLEN * wpmcnt; // Duration = one dash time
          lastsymbol = DAHLATCH; // Remember
          buffer |= 1; // set LSB to remember dash
        }
        
        key(DOWN); // Switch on the side tone and TX
        volflags &= ~(DITLATCH | DAHLATCH); // Reset both latches
        
        fsms  = KEYED; // Change FSM state
      }
      
      break;
      
    case KEYED:

#ifdef POWERSAVE
      
            yackpower(FALSE); // can not go to sleep when keyed

#endif
            
      if ((yackflags & MODE) == IAMBICB) // If we are in IAMBIC B mode
        keylatch();                      // then latch here already 
      
      if(timer == 0) // Done with sounding our element?
      {
        key(UP); // Then cancel the side tone
        timer = IEGLEN * wpmcnt; // One dot time for the gap
        fsms  = IEG; // Change FSM state
      }
      
      break;
      
      
    case IEG:
      
      keylatch(); // Latch any paddle movements (both A and B)
      
      if(timer == 0) // End of gap reached?
      {
        fsms  = IDLE; // Change FSM state
        // The following timer determines what the IDLE state
                // accepts as character. Anything longer than 2 dots as gap will be
                // accepted for a character end.
        timer = (ICGLEN - IEGLEN -1) * wpmcnt; 
      }
      break;
      
  }
  
  return '\0'; // Nothing to return if not returned in above routine
  
}
