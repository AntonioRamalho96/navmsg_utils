from nav_utils import Crc


def hex_to_bits(hex_str):
    return ''.join(bin(int(ch, 16))[2:].zfill(4) for ch in hex_str)


def test_crc24():
    inav_page_hex = '020D22A8D62489FD03D4FA00DDB6C0834A54AB1ED6092AAAAA6599853F40'
    inav_page_bin = hex_to_bits(inav_page_hex)
    page_1_crc_protected_bits = inav_page_bin[:1 + 1 + 112] # even/odd bit + page_type bit + data 1
    page_2_crc_protected_bits = inav_page_bin[120:120 + 1 + 1 + 16 + 64] # even/odd bit + page_type bit + data 2 + Reserved/SAR/Spare
    page_2_crc = inav_page_bin[120 + 1 + 1 + 16 + 64:120 + 1 + 1 + 16 + 64 + 24]

    input = page_1_crc_protected_bits + page_2_crc_protected_bits

    out = Crc.crc24(input)

    print('in:      ', input)
    print('out:     ', out)
    print('expected:', page_2_crc)

    assert out == page_2_crc, 'CRC24 result does not match expected'


if __name__ == '__main__':
    test_crc24()