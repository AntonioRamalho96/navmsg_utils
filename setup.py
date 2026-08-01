from pathlib import Path

from setuptools import setup

ROOT = Path(__file__).parent
BLD_DIR = ROOT / "bld"

setup(
    name="navmsg-utils",
    version="0.0.0",
    description="Utils to create satellite navigation messages",
    packages=["navmsg_utils"],
    package_dir={"navmsg_utils": "bld"},
    package_data={"navmsg_utils": ["*.so", "*.py", "*.pyi"]},
    install_requires=["protobuf"],
    author="Antonio Ramalho",
)
