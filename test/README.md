### Testing

Hardware used:
 - USB Stick CatWan from Electronic cats
 - Gateway Larid Sentrius RG101

Frequencies:
 - Used US_915  BAND
 - Using SUB BAND 1 [ 903.9 - 905.3 Mhz] for Uplink
 - Using [923.3 - 927.5 Mhz] to Downlink
 - Using datarates acording to US_915

In this example is used testFullFrequency program in test folder

### Results

The images show the usb send and receive in left, at the right we see what the server receive.

![TST01](img/01.png?raw=true)

![TST02](img/02.png?raw=true)

The uplink works in every frequency.

The dowlink works in almost cases, this is because how is handling the interrupt in data reception