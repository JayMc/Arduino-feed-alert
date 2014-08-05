## What?
Arduino gets feed data from a remote server and displays it on the LCD. When the feed data changes it raises the flag and flashes the lights.
3D print files, photos and video can be found at the thingiverse page.

## Thingiverse
http://www.thingiverse.com/thing:417681

## Quirks
As the Mega only has 256KB of memory, subtract the 8KB boot load and the runtime code only leave a small amount for the text string for the live feed, so the resulting feed is sometimes cut off.

## Data feed
The arduino does a GET request at http://<your IP>/path/file and expects a raw output of "<the message>" (without the quotes).
The <> symbols are used for the arduino to mark start and end of the string before processing it.

If the string is different from the previous one it will raise the flag and flash the lights.
