#!/usr/bin/python3

#       K-d tree plot program (*)
#
#     Utility used for visualization of dict-formatted K-d trees.
#   Upon launch, opens a new window in which the tree is plotted.
#
#   Usage:
#       ./viz.py <filename> [max_key]

import sys
from ast import literal_eval

from matplotlib import pyplot as plt

K = 2
KEY_MAX = 0x7fffffff

GAP = 0.08
p_min = [KEY_MAX, KEY_MAX]
p_max = [-KEY_MAX, -KEY_MAX]


def load_tree(filename):
    try:
        with open(filename) as f:
            tree = f.read()
            tree = literal_eval(tree)
            return tree
    except Exception as e:
        print("Failed to load tree:\n", e)
        exit(1)


def plot_tree(tree, p1=(-KEY_MAX, -KEY_MAX), p2=(KEY_MAX, KEY_MAX), prev=(None, None), root=False):
    global p_min, p_max
    for i in range(K):
        x1, y1 = p1
        x2, y2 = p2
        keys = list(tree.keys())

        if len(keys) <= i:
            return
        if tree.get(keys[i]) is None:
            continue

        key = keys[i]
        bound, cd = key
        if cd == 0:
            if not root:
                if i == 0:  y2 = prev[1]
                else:       y1 = prev[1]
            plt.vlines(bound, y1, y2)
        elif cd == 1:
            if not root:
                if i == 0:  x2 = prev[0]
                else:       x1 = prev[0]
            plt.hlines(bound, x1, x2)

        if type(tree[key]) == list:  # leaf
            for el in tree[key]:
                plt.scatter(*el[0], data=el[1:])
                p_min = [min(p_min[0], el[0][0]), min(p_min[1], el[0][1])]
                p_max = [max(p_max[0], el[0][0]), max(p_max[1], el[0][1])]

        elif type(tree[key]) == dict:  # subtree
            rx = bound if cd == 0 else prev[0]
            ry = bound if cd == 1 else prev[1]
            plot_tree(tree[key], (x1, y1), (x2, y2), (rx, ry))


def main():
    global KEY_MAX
    if len(sys.argv) < 2:
        print("Usage: ./viz.py <file> [max_key]")
        exit(1)

    if len(sys.argv) >= 3:
        KEY_MAX = int(sys.argv[2])

    plt.figure(num=f"K-d tree view - {sys.argv[1]}")

    tree = load_tree(sys.argv[1])
    plot_tree(tree, root=True)

    box = [p_max[i]-p_min[i] for i in range(K)]
    plt.xlim(p_min[0] - GAP*box[0], p_max[0] + GAP*box[0])
    plt.ylim(p_min[1] - GAP*box[1], p_max[1] + GAP*box[1])
    plt.show()


if __name__ == "__main__":
    main()
