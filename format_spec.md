	
### Commands

#### Special Cases:

If any of the upper 3 bits of the command byte are non-zero, they are tested from the MSB downwards. Each bit specifies a different command, with the rest of the byte then acting as the argument:

```
1nnnnnnn	note on	(0-127)
01wwwwww	line end, wait for w more lines (0-63)
001-vvvv	volume change (0-15)
```

#### Regular commands:

If the upper three bits of the command byte are all 0, then the jump table below is used. Each chip has its own jump table so that commands can be handled differently or ignored.

```
000ccccc	command number (0-31)
```
|					| Command | Arg 1 | Arg 2 | Arg 3 |
| ----------------- | ----- | ----- | ----- | ----- | 
| Note on (see above)| 0x00      |       |       |       |
| Note off          | 0x01	|       |       |       | 
| Instrument change	| 0x02  | ins # |       |       | 
| Volume change (see above) | 0x03  |       |       |       |
| FM Drum change	| 0x04	| ins #  |       |      | 
| Noise Mode		| 0x05	| val   |       |       | 
| Pitch Slide Up	| 0x06	| amt   |       |       | 
| Pitch Slide Down	| 0x07	| amt   |       |       | 
| Pitch Slide Porta	| 0x08	| amt   |       |       | 
| Slide off			| 0x09  |       |       |       | 
| Arpeggio			| 0x0a	| val   |       |       | 
| Arpeggio off		| 0x0b  |       |       |       | 
| Vibrato			| 0x0c	| val   |       |       | 
| Vibrato off		| 0x0d  |       |       |       |  
| Legato on			| 0x0e  |       |       |       | 
| Legato off		| 0x0f  |       |       |       | 
| Panning (GG)		| 0x10  | pan   | mask  |       |
| AY3 Envelope On	| 0x11  |       |       |       | 
| AY3 Envelope Off	| 0x12  |       |       |       | 
| AY3 Envelope Shape | 0x13 | x     |       |       | 
| AY3 Envelope Period Hi | 0x14 | high   |       |       | 
| AY3 Envelope Period Lo | 0x15 | low    |       |       | 
| AY3 Envelope Chan Mix | 0x16 | x    |       |       | 
| AY3 Envelope Noise Pitch | 0x17 | x    |       |       | 
| AY3 Envelope Period Word | 0x18 | low	  | high  |       | 
| Order Jump		| 0x19 | order   |       |       | 
| Set Speed 1		| 0x1a | speed   |       |       | 
| Set Speed 2		| 0x1b | speed   |       |       | 
| Order Next		| 0x1c |         |       |       | 
| Note Delay		| 0x1d | tics    |       |       | 