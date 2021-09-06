import sys
import os

from random import randint

def generate_data(amount, keyrange, filename):
    try:
        with open(filename, "w") as f:
            for _ in range(amount):
                key = randint(keyrange[0], keyrange[1])
                f1 = randint(keyrange[0], 100*keyrange[1]) * 0.732341
                s = hex(randint(1000*keyrange[0], 1000000*keyrange[1]))[2:]
                f.write(f"{key}\n{f1} {s}\n")
    except FileError as e:
        print(f"Failed to open {filename}:\n", e)
        return
    print(f"Written {amount} elements to {filename}.")

def main(argc, argv):
    if argc < 3:
        print("Usage: ./gen-data.py <amount> <file> [keymin] [keymax]")
        return
    if argc < 5:
        keymin = 1
        keymax = 100
    else:
        keymin = int(argv[3])
        keymax = int(argv[4])
        
    generate_data(int(argv[1]), (keymin, keymax), argv[2])


if __name__ == "__main__":
    sys.exit(main(len(sys.argv), sys.argv))

