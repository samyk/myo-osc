// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.
#include "../Myo.hpp"
#include "../detail/ThrowOnError.hpp"

#include <stdexcept>

namespace myo {

inline
void Myo::vibrate(VibrationType type)
{
    libmyo_vibrate(_myo, static_cast<libmyo_vibration_type_t>(type), ThrowOnError());
}

inline
void Myo::requestRssi() const
{
    libmyo_request_rssi(_myo, ThrowOnError());
}

inline
libmyo_myo_t Myo::libmyoObject() const
{
    return _myo;
}

inline
Myo::Myo(libmyo_myo_t myo)
: _myo(myo)
{
    if (!_myo) {
        throw std::invalid_argument("Cannot construct Myo instance with null pointer");
    }
}

inline
Myo::~Myo()
{
}

} // namespace myo
