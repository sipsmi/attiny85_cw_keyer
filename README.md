# attiny85_cw_keyer

Modified from:  https://github.com/dhakajack/jackyack

Arduino ATTint85 chip gives you 4 x 100char memories, iambic a, b and ultic
also provides a training program.  All this on a chip costing < £2!
  
  
                         ATtiny85               
                   +-------- --------+
              PB5 -|1(RST) |_|      8|- VCC
     DIT <--  PB3 -|2          (SCK)7|- PB2 --> CMD BTN -------- GND
     DAH <--  PB4 -|3         (MISO)6|- PB1 --> PIEZO  --------- GND   
              GND -|4         (MOSI)5|- PB0 --> TX OUT
                   +-----------------+

I have put this in a small case and added 10pF capacitors to all the socket pins to ground ( and button) to stop stray RF.  Initially, this just used a MOSFET 2N7000 to switch low the keying line however I do have an old Hybrid transceiver (TS530SP) which needs to key the -75V screen grid.   I added that by using a high-voltage PNP transistor and used diodes to block the two keying outputs from each other.  This has been tested, works a treat, and saves on the expense of a solid-state relay. 
 ![Main Screem](https://github.com/sipsmi/attiny85_cw_keyer/blob/main/keyercircuit.png?raw=true)
 
I also run it off the 13.5V radio supplies through an LM7805 voltage regulator.
 
 ![Main Screem](https://github.com/sipsmi/attiny85_cw_keyer/blob/main/powercircuit.png?raw=true)

 ![Main Screem](https://github.com/sipsmi/attiny85_cw_keyer/blob/main/keyer.jpg?raw=true)
