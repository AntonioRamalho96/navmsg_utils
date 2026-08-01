import os
import tempfile
import math
import rinex_record_pb2
import nav_utils

# Helper to write a temp CSV file
csv_content = (
'''Value Name,Size (bits),Scale,signed/unsigned,Offset
svid,6
C_ic,8,2**-8,unsigned,0.1
omega,16,semi-circle,signed,
sqrt_a,4,,unsigned,
e,4,,signed,
toe,8
OMEGA_dot,8,,signed,
i_dot,8,,unsigned,
i0,8,,signed,5
WN,4,,unsigned,
TOW,4,60,unsigned,
''')

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

with tempfile.NamedTemporaryFile('w', delete=False) as f:
    f.write(csv_content)
    csv_path = f.name

rec = make_record()

# Test C++ implementation
cpp_serializer = nav_utils.WordSerializer(csv_path)
result = cpp_serializer.serialize(rec.SerializeToString(), 12, 185)

# Split result into chunks for each field
chunks = []
bit_sizes = [6,8,16,4,4,8,8,8,8,4,4] # svid, C_ic, omega, sqrt_a, e, toe, OMEGA_dot, i_dot, i0
start = 0
for size in bit_sizes:
    chunks.append(result[start:start+size])
    start += size

# svid: 6 bits, value 6
assert chunks[0] == '000110', '# svid: 6 -> 000110'
# C_ic: 8 bits, value (0.02-0.01)/(2**-8)=25.6, unsigned, should be 25
assert chunks[1] == '00011001', '# C_ic: 25 -> 00011001'
# omega: 16 bits, semi-circle, value math.pi, should be 0111111111111111
assert chunks[2] == '0111111111111111', f"# omega: semi-circle, almost pi -> 0111111111111111 , {chunks[2]}"
# sqrt_a: 4 bits, value 20, unsigned, overflow, should be 1111
assert chunks[3] == '1111', '# sqrt_a: overflow -> 1111'
# e: 4 bits, value -20, signed, underflow, should be 1111
assert chunks[4] == '1111', '# e: underflow -> 1111'
# toe: 8 bits, value 1, default scale, signed, should be 00000001
assert chunks[5] == '00000001', '# toe: 1 -> 00000001'
# OMEGA_dot: 8 bits, value -5, signed, should be 11111011
assert chunks[6] == '11111011', '# OMEGA_dot: -5 -> 11111011'
# i_dot: 8 bits, value 5, unsigned, should be 00000101
assert chunks[7] == '00000101', '# i_dot: 5 -> 00000101'
# i0: 8 bits, value 10, offset 5, signed, (10-5)/1=5, should be 00000101
assert chunks[8] == '00000101', '# i0: offset 5, 10 -> 00000101'
# wn: 4 bits, value 12, unsigned, should be 1100
assert chunks[9] == '1100', '# wn -> 1100'
# tow: 4 bits, value 185, unsigned, scale: 60, should be 0011
assert chunks[10] == '0011', '# tow/60 -> 0011'

os.unlink(csv_path)
