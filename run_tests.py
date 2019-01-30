from __future__ import print_function
import subprocess
import sys

tests = [
    # "pa4/tests/test-mesh.xml",
    # "pa4/tests/test-mesh-furnace.xml",
    # "pa5/tests/chi2test-microfacet.xml",
    # "pa5/tests/ttest-microfacet.xml",
    # "pa5/tests/test-direct.xml",
    # "pa5/tests/test-furnace.xml",
]

total = len(tests)
passed = 0

failed = []

for t in tests:
    path = "scenes/" + t
    ret = subprocess.call(["./build/nori", path])
    if ret == 0:
        passed += 1
    else:
        failed.append(t)

print("")
if passed < total:
    print("\033[91m" + "Passed " + str(passed) + " / " + str(total) + " tests." + "\033[0m")
    print("Failed tests:")
    for t in failed:
        print("\t" + t)

    sys.exit(1)
else:
    print("\033[92m" + "Passed " + str(passed) + " / " + str(total) + " tests." + "\033[0m")
    print("\tNice!")

    sys.exit(0)
