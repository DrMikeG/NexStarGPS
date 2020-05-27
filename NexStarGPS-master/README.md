NexStarGPS
==========

Uses TinyGPS from http://arduiniana.org/libraries/tinygps/ as GPS provider.

Uses customized versions of SoftwareSerial and SendOnlySoftwareSerial libraries.

Does not use CTS/RTS pins since they are not used on NexStar 5SE. CTS is always 3V, RTS is always 12V (!). Commonly available 4-wire telephone cables can be used to connect GPS unit to telescope (but you need 6p6c plug on telescope side anyway, just do not connect pins 1 and 6). Also 5SE has TX and RX pins shortcutted so relatively weird logic with two software serial instances (one for TX, another for RX) is used to allow telescope firmware to work properly.

Without the work of Andre Paquette you find at http://www.paquettefamily.ca/nexstar/NexStar_AUX_Commands_10.pdf this library would have been impossible. Andre you rock!

Licence
=======
Copyright 2014 Thomas Peuss <thomas at peuss dot de>

Copyright 2015 Andrew Evdokimov <i at iflyhigh dot ru>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
