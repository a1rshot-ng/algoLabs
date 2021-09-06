#!/usr/bin/python3

import os
import json
from math import sqrt


NEIGH_DIST2 = 4e-5
WEIGHT_FACTOR = 150
WEIGHT_CONST = 1
WEIGHT_CHANGE = 6


def dist2(p1, p2):
    return (p1[0]-p2[0])**2 + (p1[1]-p2[1])**2


nodes = set()
edges = set()

with open('metro.json') as f:
    metro = json.load(f)
    lines = dict(metro)['lines']

with open('metro_data.txt', 'w') as f:
    f.write("create\n")
    for line in lines:
        line_id = line['id']
        prev_node = None
        for station in line['stations']:
            name = station['name'].lower()
            name = name.replace(' ', '_').replace('ё', 'е')
            lat = station['lat']
            lng = station['lng']

            f.write(f"add node {name} {lng} {lat}\n")
            if prev_node:
                distance = sqrt(dist2(prev_node[1:], (lng, lat)))
                f.write(f"add edge {prev_node[0]} {name} {round(distance*WEIGHT_FACTOR)+WEIGHT_CONST}\n")
            prev_node = (name, lng, lat)

            for change in nodes:
                if dist2(change[1:], (lng, lat)) < NEIGH_DIST2 and name != change[0]:
                    f.write(f"add edge {change[0]} {name} {WEIGHT_CHANGE}\n")
            nodes.add((name, lng, lat))

    f.write("save metro_graph.txt\n")
    f.write("exit\n")

os.system("cat metro_data.txt | ./lab5 2> /dev/null")
os.system("rm metro_data.txt")
