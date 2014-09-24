# [Myo-OSC](http://samy.pl/myo-osc/)

OSC bridge for Thalmic Myo gesture control armband for Windows and Mac OS X 10.8+.

By [Samy Kamkar](http://samy.pl)

Myo-OSC is a C++ application designed to take hand gestures, accelerometer, gyroscope and magnetometer data from the [Thalmic Labs Myo](https://www.thalmic.com/en/myo/) armband and output it over OSC. This allows incredible control of virtually any device or application just by waving or flailing your arm or hand around like a madman.

Binaries for both Windows and OS X are included in the root directory (myo-osc.win.exe / myo-osc.osx.bin).

I've built this and integrated it to control various things in my house and car using Raspberry Pi and Arduinos, including lights (over RF), TV (over IR), receiver (over IR), iTunes, coffee machine (over BLE), music in my car (over BLE), presentations (over IR), and Ableton/music production/DJing/VJing (over MIDI and OSC).

You can also send any OSC data over a network.

by [@SamyKamkar](https://twitter.com/samykamkar) // <code@samy.pl> // <http://samy.pl> // Mar 3, 2014

Code available on [github](http://samy.pl/myo-osc/)

------

## Troubleshooting
On OS X, if you receiver an error message when running the trainer (LSOpenURLsWithRole() failed with error -10810), run this from Terminal:

chmod +x myo-trainer.app/Contents/MacOS/myo-trainer

Then rerun the trainer.

## Download
You can acquire Myo-OSC from github (redirects to latest branch): <http://samy.pl/myo-osc/>

------

## Usage
$ Myo-OSC.exe ip.address osc.port

This will output the following OSC data

```
/myo/pose s MAC s pose

/myo/accel s MAC f X_vector3 f Y_vector3 f Z_vector3

/myo/gyro s MAC f X_vector3 f Y_vector3 f Z_vector3

/myo/orientation s MAC f X_quaternion f Y_quaternion f Z_quaternion f W_quaternion f roll f pitch f yaw
```

Examples:

```
/myo/pose s f2-e0-66-5d-90-8a s fist

/myo/accel s f2-e0-66-5d-90-8a f 0.95849609375 f 0.26953125 f 0.20068359375

/myo/gyro s f2-e0-66-5d-90-8a f -7.14111280441284 f -15.8081045150757 f 5.43212842941284

/myo/orientation s f2-e0-66-5d-90-8a f -0.00215625390410423 f 3.37712929902281e-43 f -0.0021759606897831 f 0 f 0.928672909736633 f -1.23411226272583 f 1.45198452472687
```


------

### Software

#### Myo-OSC
This software.

#### oscpack
We also use [oscpack](https://code.google.com/p/oscpack/), a C++ OSC (Open Sound Control) library.

### Hardware

#### Thalmic Labs Myo Alpha
Currently, Thalmic Labs have only released Myo Alpha to select developers. Sign up for the Developer Kit coming out later this year or pre-order the consumer version [here](https://www.thalmic.com/en/myo/).

## Questions?

Feel free to contact me with any questions!

You can reach me at <code@samy.pl>.

Follow [@SamyKamkar](https://twitter.com/samykamkar) on Twitter or check out <http://samy.pl> for my other projects.
