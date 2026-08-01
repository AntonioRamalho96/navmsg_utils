
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
python3 -m venv .venv
source .venv/bin/activate
export PYTHONPATH="$SCRIPT_DIR/bld:$PYTHONPATH"
pip install protobuf
pip install pybind11
make
make test