#!/usr/bin/python3

#       Lab 5  -  Graph visualization (*)
#
#     Utility used for visualization of dict-formatted graphs.
#   Upon launch, opens a new window in which the graph is plotted.
#
#   Usage:
#       ./viz.py <filename> [mode: plt/nx]

import sys
from ast import literal_eval

from matplotlib import pyplot as plt


def graph_nx(graph, filename):
    import networkx as nx
    plt.figure(num=f"Graph view - {filename}")
    g = nx.Graph()
    for key, node in graph.items():
        point, adj = node
        g.add_node(key)
        for weight, key2 in adj:
            g.add_edge(key, key2, weight=weight)
    nx.draw_networkx(g)
    plt.show()


def graph_plt(graph, filename):
    plt.figure(num=f"Graph view - {filename}")
    for key, node in graph.items():
        point, adj = node
        plt.scatter(*point)
        plt.annotate(key, point)
        for weight, key2 in adj:
            if key2 > key:
                point2 = graph[key2][0]
                plt.plot((point[0], point2[0]), (point[1], point2[1]))
                plt.annotate(str(weight), [(point[i] + point2[i])/2 for i in (0, 1)])
    plt.show()


def main():
    if len(sys.argv) < 2 or len(sys.argv) >= 3 and sys.argv[2] not in ('plt', 'nx'):
        print("Usage: ./viz.py <file> [mode: plt/nx]")
        exit(1)

    filename = sys.argv[1]

    if len(sys.argv) >= 3:
        mode = sys.argv[2]
    else:
        mode = 'plt'

    try:
        with open(filename) as f:
            graph = literal_eval(f.read())
            if mode == 'plt':
                graph_plt(graph, filename)
            else:  # == 'nx':
                graph_nx(graph, filename)
    except Exception:
        print("Failed to visualize the graph:")
        raise


if __name__ == "__main__":
    main()
