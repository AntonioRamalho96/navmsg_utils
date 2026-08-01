import sys
import nav_utils
import rinex_record_pb2
import math

def make_record():
    rec = rinex_record_pb2.RinexNavRecord()
    rec.svid = 6
    rec.C_ic = 0.2
    rec.omega = math.pi - 2**-14
    rec.sqrt_a = 20
    rec.e = -20
    rec.toe = 1
    rec.OMEGA_dot = -5
    rec.i_dot = 5
    rec.i0 = 10
    return rec

ser = nav_utils.WordSerializer(sys.argv[1])
print(ser.serialize(make_record().SerializeToString()), 0, 0)
