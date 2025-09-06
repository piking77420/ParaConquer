import os
import subprocess
import argparse


## Parse Arg
parser = argparse.ArgumentParser(description="Configure build")
parser.add_argument("--plateform", default="NONE", help="TODO")
parser.add_argument("--simd", default="NONE", help="SIMD instruction set (AUTO, SSE2, AVX, AVX2, NEON, SVE, NONE)")
parser.add_argument("--profiling", default="OFF", help="Enable Tracy Profiling ON/OFF")
parser.add_argument("--built_test", default="OFF", help="Build UnitTest ON/OFF")

args = parser.parse_args()

build_dir = "Build"

os.makedirs(build_dir, exist_ok=True)

os.chdir(build_dir)

cmake_command = [
    "cmake", "..",
    f"-DSIMD_INSTRUCTION_SET={args.simd}",
    f"-DPROFILING={args.profiling}",
    f"-DBUILD_TEST={args.built_test}",
]

subprocess.run(cmake_command, check=True)