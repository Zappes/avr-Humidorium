The Humidorium Controls Humidor Humidity
========================================

What Is this?
-------------
The Humidorium is a simple sensor/display combo that controls the humidity inside a box used for storing cigars, a so-called humidor. 

![Image of the Humidorium](https://github.com/Zappes/avr-Humidorium/blob/master/Pictures/index.jpg?raw=true)

The Humidorium uses an ATmega328 microcontroller, a dirt cheap 16x2 lcd module from Ebay and 2 LEDs (a red and a yellow one) to interface with what some people call "the real world". The sensor has to be attached to the inside of the humidor, the rest of the electronics should be put into some enclosure outside of the actual box.

As long as the relative humidity measured by the sensor is between 67% and 74%, no LED will be active. Between 62% and 66% and between 75% and 75%, the yellow LED will light up. Go below or above that and the red LED will indicate that your cigars are dying.

The LCD shows humidity and temperature as well as a status message.

You will find more images and a video here:

* https://www.youtube.com/watch?v=8ul-UXaS1zw
* https://plus.google.com/+GregorOttmann/posts/SEYbfbkTWDa
* https://plus.google.com/+GregorOttmann/posts/FgXsiRtwFJq
* https://www.youtube.com/watch?feature=player_embedded&v=oTg8d18DWwY

The videos are german. I guess you'll have to live with that. :)

Hardware
--------

The hardware is really simple. I included the EAGLE file in the folder "Humidorium.eagle".

![Schematic of the Humidorium](https://github.com/Zappes/avr-Humidorium/blob/master/Pictures/Schematic.png?raw=true)

The Schematic doesn't include the LCD, sensor or LEDs. Keep in mind that those are required when you go shopping for parts.