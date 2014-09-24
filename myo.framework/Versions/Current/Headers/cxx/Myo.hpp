// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.
#ifndef MYO_CXX_MYO_HPP
#define MYO_CXX_MYO_HPP

#include <myo/libmyo.h>

namespace myo {

/// Represents a Myo device with a specific MAC address.
/// This class can not be instantiated directly; instead, use Hub to get access to a Myo.
/// There is only one Myo instance corresponding to each device; thus, if the addresses of two Myo instances compare
/// equal, they refer to the same device.
class Myo {
public:
    /// Types of vibration supported by the Myo.
    enum VibrationType {
        vibrationShort  = libmyo_vibration_short,
        vibrationMedium = libmyo_vibration_medium,
        vibrationLong   = libmyo_vibration_long
    };

    /// Vibrate the Myo.
    void vibrate(VibrationType type);

    /// Request the RSSI of the Myo. An onRssi event will likely be generated with the value of the RSSI.
    /// @see DeviceListener::onRssi()
    void requestRssi() const;

    /// @cond MYO_INTERNALS

    /// Return the internal libmyo object corresponding to this device.
    libmyo_myo_t libmyoObject() const;

    /// @endcond

private:
    Myo(libmyo_myo_t myo);
    ~Myo();

    libmyo_myo_t _myo;

    // Not implemented.
    Myo(const Myo&);
    Myo& operator=(const Myo&);

    friend class Hub;
};

} // namespace myo

#include "impl/Myo_impl.hpp"

#endif // MYO_CXX_MYO_HPP
