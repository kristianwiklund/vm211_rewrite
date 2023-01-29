This is a from-the-bottom rewrite of firmware for the VM211 earth listener.
The original firmware started to become very hackish with my changes incorporated.

Requires platformio for build - I have not tested this with the Arduino CLI/IDE.

=MVP

The code is in an MVP state right now - in the sense that it is doing what it did for me before my rewrite, displaying the clock.
I haven't had the sensors connected for quite some time, those ended up in another device (as intended). Should of course rewire them for testing...

=Design goals

* Slim down the code a lot, remove the reliance to the touch screen, and display as much as possible around the clock.
* Configuration shall be done in files instead of in user interfaces (those that are able to rebuild with this code have no problem doing so)
* Get the weather forecast in to the device and display it at the bottom of the screen.
