XyloLibrary
===========

The XyloLibrary is an Arduino library for sending commands to "Xylobands" LED bracelets using a XyloShield (https://github.com/StefanKrupop/XyloShield).
The radio protocol used by the Xylobands is mostly unknown, but efforts to reverse it are undertaken here: http://xylobandhacking.freeforums.net/
Besides sending arbitrary commands, the library currently implements methods to wake up Xylobands or to send them to sleep mode.

Installation
------------

After downloading, uncompress the downloaded archive. Rename the uncompressed folder to "XyloLibrary". Place the XyloLibrary folder in your <arduinosketchfolder>/libraries/ folder. You may need to create the libraries subfolder if this is your first library. Then restart the Arduino environment.

Examples
--------

The library comes with two example sketches:

XyloTest wakes up Xylobands, then turns them red, green and blue for two seconds each and finally sends them to sleep again.

XyloSender forwards packets received via the Arduino's serial port to Xylobands. This allows sending arbitrary commands from a PC.