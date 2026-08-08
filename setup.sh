

(
    set -e
    
    echo Setup python environment...
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    python3 -m venv .venv
    source .venv/bin/activate
    export PYTHONPATH="$SCRIPT_DIR/bld:$PYTHONPATH"
    pip install protobuf > /dev/null
    pip install pybind11 > /dev/null

    echo Build python library...
    make > /dev/null

    echo Run tests...
    make test > /dev/null

    echo Creating wheel...
    make wheel > /dev/null
    echo Done! Wheel can be found here:
    echo "   " $(ls -d1 ./dist/* )
)