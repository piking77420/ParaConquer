import os
import subprocess
import argparse

# Parse arguments
parser = argparse.ArgumentParser(description="Configure build")

parser.add_argument(
    "--platform",
    default="NONE",
    help="Target platform (e.g., Windows, Linux, Mac)"
)
parser.add_argument(
    "--simd",
    default="NONE",
    help="SIMD instruction set (AUTO, SSE2, AVX, AVX2, NEON, SVE, NONE)"
)
parser.add_argument(
    "--profiling",
    default="OFF",
    help="Enable Tracy Profiling ON/OFF"
)
parser.add_argument(
    "--build_test",
    default="OFF",
    help="Build UnitTest ON/OFF"
)

args = parser.parse_args()

# Create build folder
build_dir = "Build"
os.makedirs(build_dir, exist_ok=True)
os.chdir(build_dir)

# Prepare CMake command
cmake_command = [
    "cmake", "..",
    f"-DSIMD_INSTRUCTION_SET={args.simd}",
    f"-DPROFILING={args.profiling}",
    f"-DBUILD_TEST={args.build_test}",
    f"-DPLATFORM={args.platform}",
]

print("Running:", " ".join(cmake_command))

# Run CMake
subprocess.run(cmake_command, check=True)
