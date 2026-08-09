from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "bld"))

from nav_utils import RinexManager
import rinex_record_pb2


def test_rinex_manager_loads_records():
    rinex_path = Path(__file__).resolve().parent / "test_files" / "rinex_4.rnx"
    manager = RinexManager()
    manager.load(str(rinex_path))

    records = manager.get_serialized_rinex_records()
    assert len(records) == 3

    first = rinex_record_pb2.RinexNavRecord()
    first.ParseFromString(records[0])
    first.constellation == rinex_record_pb2.GPS
    assert first.svid == 1

    second = rinex_record_pb2.RinexNavRecord()
    second.ParseFromString(records[1])
    second.constellation == rinex_record_pb2.GALILEO
    assert second.svid == 12

if __name__ == "__main__":
    test_rinex_manager_loads_records()