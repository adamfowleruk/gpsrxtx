# GPS Transmitter-Receiver Project

This repo holds all the code and outlines of my Duke of Edinburgh GPS Transmitter Receiver radio project.

All code, Eagle files, documentation etc. is licensed under the [Creative Commons Sharealike License V3 (CC-by-SA)](http://creativecommons.org/licenses/by-sa/3.0/).

## Final code

The src folder holds all production ready code. This has two subfolders, one for the transmitter the other the
receiver module. Each is slightly different - with a lot more around user interface and interaction on the receiver
currently. This may change in future if I decide to add two way communication support to the trackers. (Seems a waste
  not to given the capabilities of the 3DR Radios within them!)

## Prototype code

The prototypes folder holds many individual test Arduino sketches. These may prove useful if you have a similar project
but don't want to wade through the code for the whole thing to see, for example, how I got GPX recording on the
ArduLogger working with the OpenLogger software.

Each individual proof point has its own code sample. These range from radio to gps to symmetric encryption, sd card
logging and LCD interaction.
