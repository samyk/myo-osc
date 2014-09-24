// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.
#ifndef MYO_CXX_DEVICELISTENER_HPP
#define MYO_CXX_DEVICELISTENER_HPP

#include <stdint.h>

#include "Pose.hpp"

namespace myo {

class Myo;
template<typename T>
class Quaternion;
template<typename T>
class Vector3;

/// Enumeration identifying a right arm or left arm.
enum Arm {
    armLeft = libmyo_arm_left,
    armRight = libmyo_arm_right,
    armUnknown = libmyo_arm_unknown
};

/// Possible directions for Myo's +x axis relative to a user's arm.
enum XDirection {
    xDirectionTowardWrist = libmyo_x_direction_toward_wrist,
    xDirectionTowardElbow = libmyo_x_direction_toward_elbow,
    xDirectionUnknown = libmyo_x_direction_unknown
};

/// Firmware version of Myo.
struct FirmwareVersion {
    unsigned int firmwareVersionMajor; ///< Myo's major version must match the required major version.
    unsigned int firmwareVersionMinor; ///< Myo's minor version must match the required minor version.
    unsigned int firmwareVersionPatch; ///< Myo's patch version must greater or equal to the required patch version.
    unsigned int firmwareVersionHardwareRev; ///< Myo's hardware revision; not used to detect firmware version mismatch.
};

/// A DeviceListener receives events about a Myo.
/// @see Hub::addListener()
class DeviceListener {
public:
    virtual ~DeviceListener() {}

    /// Called when a Myo has been paired.
    virtual void onPair(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion) {}

    /// Called when a Myo has been unpaired.
    virtual void onUnpair(Myo* myo, uint64_t timestamp) {}

    /// Called when a paired Myo has been connected.
    virtual void onConnect(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion) {}

    /// Called when a paired Myo has been disconnected.
    virtual void onDisconnect(Myo* myo, uint64_t timestamp) {}

    /// Called when a paired Myo recognizes that it is on an arm.
    virtual void onArmRecognized(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection) {}

    /// Called when a paired Myo is moved or removed from the arm.
    virtual void onArmLost(Myo* myo, uint64_t timestamp) {}

    /// Called when a paired Myo has provided a new pose.
    virtual void onPose(Myo* myo, uint64_t timestamp, Pose pose) {}

    /// Called when a paired Myo has provided new orientation data.
    virtual void onOrientationData(Myo* myo, uint64_t timestamp, const Quaternion<float>& rotation) {}

    /// Called when a paired Myo has provided new accelerometer data in units of g.
    virtual void onAccelerometerData(Myo* myo, uint64_t timestamp, const Vector3<float>& accel) {}

    /// Called when a paired Myo has provided new gyroscope data in units of deg/s.
    virtual void onGyroscopeData(Myo* myo, uint64_t timestamp, const Vector3<float>& gyro) {}

    /// Called when a paired Myo has provided a new RSSI value.
    /// @see Myo::requestRssi() to request an RSSI value from the Myo.
    virtual void onRssi(Myo* myo, uint64_t timestamp, int8_t rssi) {}

    /// @cond LIBMYO_INTERNALS

    virtual void onOpaqueEvent(libmyo_event_t) {}

    /// @endcond
};

} // namespace myo

#endif // MYO_CXX_DEVICELISTENER_HPP
