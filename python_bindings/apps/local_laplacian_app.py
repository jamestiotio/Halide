"""
Shell for running Local Laplacian.
"""

from local_laplacian import local_laplacian
from local_laplacian_Mullapudi2016 import local_laplacian_Mullapudi2016
import halide.imageio
import numpy as np
import sys
from timeit import Timer


def main():
    if len(sys.argv) < 6:
        print(
            "Usage: %s input.png input.png levels alpha beta output.png" % sys.argv[0]
        )
        print("e.g. %s input.png 8 1 1 output.png 10" % sys.argv[0])
        sys.exit(1)

    input_path = sys.argv[1]
    levels = int(sys.argv[2])
    alpha = float(sys.argv[3])
    beta = float(sys.argv[4])
    output_path = sys.argv[5]
    timing_iterations = 10

    print("Reading from %s ..." % input_path)
    input_buf_u8 = halide.imageio.imread(input_path)
    assert input_buf_u8.dtype == np.uint8
    # Convert to uint16 in range [0..1]
    input_buf = input_buf_u8.astype(np.uint16) * 257
    h = input_buf.shape[1]
    w = input_buf.shape[2]
    output_buf = np.empty([3, h, w], dtype=input_buf.dtype)

    tests = {
        "Manual": local_laplacian,
        "Mullapudi2016": local_laplacian_Mullapudi2016,
    }

    for name, fn in tests.items():
        print("Running %s... " % name, end="")
        t = Timer(lambda: fn(input_buf, levels, alpha / (levels - 1), beta, output_buf))
        avg_time_sec = t.timeit(number=timing_iterations) / timing_iterations
        print("time: %fms" % (avg_time_sec * 1e3))

    output_buf_u8 = (output_buf // 257).astype(np.uint8)

    print("Saving to %s ..." % output_path)
    halide.imageio.imwrite(output_path, output_buf_u8)

    print("Success!")
    sys.exit(0)


if __name__ == "__main__":
    main()
