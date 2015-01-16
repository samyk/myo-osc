// Myo-OSC, by Samy Kamkar
// outputs all data from the Myo armband over OSC
// usage: myo-osc osc.ip.address osc.port
//
// http://samy.pl - code@samy.pl - 03/03/2014
//
// also makes use of oscpack from http://www.rossbencina.com/code/oscpack
//
// some code originally from hello-myo:
// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.

// stop oscpack sprintf warnings
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//#define MAC myo->macAddressAsString().c_str()
#define MAC "00:00:00:00:00:00" // beta release 1 removed macs :(

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>

// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>

// add oscpack
#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

#define OUTPUT_BUFFER_SIZE 1024

UdpTransmitSocket* transmitSocket;

// Classes that inherit from myo::DeviceListener can be used to receive events from Myo devices. DeviceListener
// provides several virtual functions for handling different kinds of events. If you do not override an event, the
// default behavior is to do nothing.
class DataCollector : public myo::DeviceListener {
public:
    DataCollector()
    : onArm(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose()
    {
    }

    
	// units of g
	void onAccelerometerData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& accel)
	{
		a_x = accel.x();
		a_y = accel.y();
		a_z = accel.z();
        
		osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);
		p << osc::BeginMessage("/myo/accel")
        << MAC
        << a_x << a_y << a_z << osc::EndMessage;
		transmitSocket->Send(p.Data(), p.Size());
	}
    
	// units of deg/s
	void onGyroscopeData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& gyro)
	{
		g_x = gyro.x();
		g_y = gyro.y();
		g_z = gyro.z();
        
		osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);
		p << osc::BeginMessage("/myo/gyro")
        << MAC
        << g_x << g_y << g_z << osc::EndMessage;
		transmitSocket->Send(p.Data(), p.Size());
	}

    // onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
    // as a unit quaternion.
    void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
    {
        using std::atan2;
        using std::asin;
        using std::sqrt;

        // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
        float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                           1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
        float pitch = asin(2.0f * (quat.w() * quat.y() - quat.z() * quat.x()));
        float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                        1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));

        osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);
				p << osc::BeginMessage("/myo/orientation")
        << MAC
        << quat.x() << quat.y() << quat.z() << quat.w() << roll << pitch << yaw << osc::EndMessage;
		transmitSocket->Send(p.Data(), p.Size());
    
        // Convert the floating point angles in radians to a scale from 0 to 20.
        roll_w = static_cast<int>((roll + (float)M_PI)/(M_PI * 2.0f) * 18);
        pitch_w = static_cast<int>((pitch + (float)M_PI/2.0f)/M_PI * 18);
        yaw_w = static_cast<int>((yaw + (float)M_PI)/(M_PI * 2.0f) * 18);
    }

    // onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
    // making a fist, or not making a fist anymore.
    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
        currentPose = pose;

		osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);
		p << osc::BeginMessage("/myo/pose")
        << MAC
        << currentPose.toString().c_str() << osc::EndMessage;
		transmitSocket->Send(p.Data(), p.Size());
        
		// Vibrate the Myo whenever we've detected that the user has made a fist.
        if (pose == myo::Pose::fist) {
            myo->vibrate(myo::Myo::vibrationShort);
        }
    }

    // onArmRecognized() is called whenever Myo has recognized a setup gesture after someone has put it on their
    // arm. This lets Myo know which arm it's on and which way it's facing.
    void onArmRecognized(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
    {
        onArm = true;
        whichArm = arm;

		osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);
		p << osc::BeginMessage("/myo/onarm")
        << MAC
        << (whichArm == myo::armLeft ? "L" : "R") << osc::EndMessage;
		transmitSocket->Send(p.Data(), p.Size());
    }

    // onArmLost() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
    // it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
    // when Myo is moved around on the arm.
    void onArmLost(myo::Myo* myo, uint64_t timestamp)
    {
        onArm = false;
		osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);
		p << osc::BeginMessage("/myo/onarmlost")
        << MAC
        << osc::EndMessage;
		transmitSocket->Send(p.Data(), p.Size());
    }

    // There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
    // For this example, the functions overridden above are sufficient.

    // We define this function to print the current values that were updated by the on...() functions above.
    void print()
    {
        // Clear the current line
        std::cout << '\r';

        // Print out the orientation. Orientation data is always available, even if no arm is currently recognized.
        std::cout << '[' << std::string(roll_w, '*') << std::string(18 - roll_w, ' ') << ']'
                  << '[' << std::string(pitch_w, '*') << std::string(18 - pitch_w, ' ') << ']'
                  << '[' << std::string(yaw_w, '*') << std::string(18 - yaw_w, ' ') << ']';

        if (onArm) {
            // Print out the currently recognized pose and which arm Myo is being worn on.

            // Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
            // output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
            // that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().
            std::string poseString = currentPose.toString();

            std::cout << '[' << (whichArm == myo::armLeft ? "L" : "R") << ']'
                      << '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
        } else {
            // Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
            std::cout << "[?]" << '[' << std::string(14, ' ') << ']';
        }

        std::cout << std::flush;
    }

    // These values are set by onArmRecognized() and onArmLost() above.
    bool onArm;
    myo::Arm whichArm;

    // These values are set by onOrientationData() and onPose() above.
    int roll_w, pitch_w, yaw_w;
    float w, x, y, z, roll, pitch, yaw, a_x, a_y, a_z, g_x, g_y, g_z;
    myo::Pose currentPose;
    char buffer[OUTPUT_BUFFER_SIZE];
};

int main(int argc, char** argv)
{
    // We catch any exceptions that might occur below -- see the catch statement for more details.
    try
    {
        if (argc != 3 && argc != 2 && argc != 1)
        {
            std::cout << "\nusage: " << argv[0] << " [IP address] <port>\n\n" <<
			"Myo-OSC sends OSC output over UDP from the input of a Thalmic Myo armband.\n" <<
			"IP address defaults to 127.0.0.1/localhost\n\n" <<
			"by Samy Kamkar -- http://samy.pl -- code@samy.pl\n";
            exit(0);
        }

				if (argc == 1)
				{
					int port = 7777;
					std::cout << "Sending Myo OSC to 127.0.0.1:7777\n";
					transmitSocket = new UdpTransmitSocket(IpEndpointName("127.0.0.1", port));
				}
				else if (argc == 2)
				{
					std::cout << "Sending Myo OSC to 127.0.0.1:" << argv[1] << "\n";
					transmitSocket = new UdpTransmitSocket(IpEndpointName("127.0.0.1", atoi(argv[1])));
				}
				else if (argc == 3)
				{
					std::cout << "Sending Myo OSC to " << argv[1] << ":" << argv[2] << "\n";
			    transmitSocket = new UdpTransmitSocket(IpEndpointName(argv[1], atoi(argv[2])));
				}
				else
				{
					std::cout << "well this awkward -- weird argc: " << argc << "\n";
					exit(0);
				}


    // First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
    // publishing your application. The Hub provides access to one or more Myos.
    myo::Hub hub("com.samy.myo-osc");

    std::cout << "Attempting to find a Myo..." << std::endl;

    // Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
    // immediately.
    // waitForAnyMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
    // if that fails, the function will return a null pointer.
    myo::Myo* myo = hub.waitForMyo(10000);

    // If waitForAnyMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
    if (!myo) {
        throw std::runtime_error("Unable to find a Myo!");
    }

    // We've found a Myo.
    std::cout << "Connected to a Myo armband!" << std::endl << std::endl;

    // Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
    DataCollector collector;

    // Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
    // Hub::run() to send events to all registered device listeners.
    hub.addListener(&collector);

    // Finally we enter our main loop.
    while (1) {
        // In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
        // In this case, we wish to update our display 20 times a second, so we run for 1000/20 milliseconds.
        hub.run(1000/20);
        // After processing events, we call the print() member function we defined above to print out the values we've
        // obtained from any events that have occurred.
        collector.print();
    }

    // If a standard exception occurred, we print out its message and exit.
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
        return 1;
    }
}
