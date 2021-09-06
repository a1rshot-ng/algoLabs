#!/usr/bin/python3

import sys
from random import randint

KEY_MAX = 100
N = 20


def parse_args():
    global N, KEY_MAX
    if len(sys.argv) >= 2:
        try:
            N = int(sys.argv[1])
            if (len(sys.argv)) >= 3:
                KEY_MAX = int(sys.argv[2])
        except ValueError:
            print("Usage:  ./gen_data.py [n] [max_key]")
            exit(0)


def main():
    for _ in range(N):
        print(f'{randint(-KEY_MAX, KEY_MAX)} {randint(-KEY_MAX, KEY_MAX)} '
              f'{randint(-100, 100) / 20} {randint(-100, 100) / 20} '
              f'{hex(randint(1 << 7*8, (1 << 8*8) - 1))[2:]}')


if __name__ == "__main__":
    parse_args()
    main()
