# CSV Format Documentation

This document describes the format of the CSV file used to configure the `word_serializer`.

## Overview
The CSV file defines the structure of a word, including the fields, their sizes, scales, signedness, and offsets. Each row in the CSV corresponds to a field in the word.

## Format
The CSV file must have the following columns:

1. **Value Name**: The name of the field. This must match one of the member names of the `RinexNavRecord` protobuf structure. Additionally, the special names are allowed:
 - `EMPTY` - will always produce a value of `0`
 - `VAL:${X}` - will prouce an integer (whatever is placed instead of ${X})
 - `WN`- Week number of the Galileo week of the start of the transmission of the page
 - `TOW` - Time of Week (in seconds) of the start of the transmission of the page
2. **Size (bits)**: The number of bits allocated for the field. This must be a positive integer.
3. **Scale** (optional): The scale factor applied to the field value. Valid configurations include:
   - A numeric integer value (e.g., `2`).
   - A power of integers, expressed as `b**n` (e.g., `2**-8`, `10**3`).
   - The special value `semi-circle`, which computes the scale as `(2**-(n_bits-1)) * π`. This is only valid for signed fields.
   - Default: `1`.
4. **Signed/Unsigned** (optional): Specifies whether the field is signed or unsigned. Valid values are:
   - `signed` (default)
   - `unsigned`
5. **Offset** (optional): The offset applied to the field value before scaling. Default: `0`.

## Example
```csv
Value Name,Size (bits),Scale,Signed/Unsigned,Offset
svid,6
C_ic,8,2**-8,unsigned,0.1
omega,16,semi-circle,signed
sqrt_a,4,,unsigned
EMPTY,8
```

### Explanation
- `svid`: A 6-bit signed field with no scaling or offset.
- `C_ic`: An 8-bit unsigned field scaled by `2**-8` with an offset of `0.1`.
- `omega`: A 16-bit signed field scaled as `semi-circle`.
- `sqrt_a`: A 4-bit unsigned field with no scaling or offset.
- `EMPTY`: An 8-bit field that always outputs `0`.

## Notes
- The header row is required, since the first non empty line is always skipped.
- Trailing commas in rows are allowed and will be treated as empty values.
- Invalid configurations (e.g., `semi-circle` for unsigned fields) will result in an error.

## Defaults
- **Scale**: `1`
- **Signed/Unsigned**: `signed`
- **Offset**: `0`

## Possible names

  - svid - Satellite vehicle ID
  - t_tm - Transmission time of the message (seconds)
  - year - Year of transmission of epoch
  - month - Month of transmission of epoch
  - day - Day of transmission of epoch
  - hour - Hour of transmission of epoch
  - minute - Minute of transmission of epoch
  - second - Second of transmission of epoch

  - sv_clock_bias - Satellite clock bias (seconds)
  - sv_clock_drift - Satellite clock drift (seconds/second)
  - sv_clock_drift_rate - Rate of change of satellite clock drift (seconds/second²)
  - iode - Issue of Data Ephemeris
  - toe - Time of Ephemeris (seconds)
  - Delta_n - Mean motion difference from computed value (radians/second)
  - M0 - Mean anomaly at reference time (radians)
  - e - Eccentricity of the orbit
  - sqrt_a - Square root of the semi-major axis of the orbit (meters^0.5)
  - omega - Argument of perigee (radians)
  - i0 - Inclination angle at reference time (radians)
  - i_dot - Rate of change of inclination angle (radians/second)
  - OMEGA0 - Longitude of ascending node of orbit plane at weekly epoch (radians)
  - OMEGA_dot - Rate of change of right ascension (radians/second)
  - C_ic - Amplitude of the cosine harmonic correction term to the angle of inclination (radians)
  - C_is - Amplitude of the sine harmonic correction term to the angle of inclination (radians)
  - C_uc - Amplitude of the cosine harmonic correction term to the argument of latitude (radians)
  - C_us - Amplitude of the sine harmonic correction term to the argument of latitude (radians)
  - C_rc - Amplitude of the cosine harmonic correction term to the orbit radius (meters)
  - C_rs - Amplitude of the sine harmonic correction term to the orbit radius (meters)

  - EMPTY - Zeros