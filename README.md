# attiny85_cw_keyer

Modified from:  https://github.com/dhakajack/jackyack

Arduno ATTint85 chip gives you 4 x 100char memories, iambic a, b and ultic
also provides a training programe.  All this on a chip costing < £2!
  
  
                         ATtiny85               
                   +-------- --------+
              PB5 -|1(RST) |_|      8|- VCC
     DIT <--  PB3 -|2          (SCK)7|- PB2 --> CMD BTN -------- GND
     DAH <--  PB4 -|3         (MISO)6|- PB1 --> PIEZO  --------- GND   
              GND -|4         (MOSI)5|- PB0 --> TX OUT
                   +-----------------+

I have put this is a small case and added 10pF capacitors to all the socket pins to ground ( and button) to stop stray RF.

Also run it off the 13.5V radio supplies though an LM7805.

 ![Main Screem](https://github.com/sipsmi/attiny85_cw_keyer/blob/main/keyer.jpg?raw=true)
