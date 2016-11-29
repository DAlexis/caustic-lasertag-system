#!/usr/bin/env python3

import math
import sys
from optparse import OptionParser

def parse_gr_line(str, line_index):
    result = {}
    start_token = "(start"
    pos_start_x = str.find(start_token) + len(start_token) + 1
    pos_start_x_end = str.find(" ", pos_start_x)
    pos_start_y = pos_start_x_end + 1
    pos_start_y_end = str.find(")", pos_start_y)
    start_x = float(str[pos_start_x:pos_start_x_end])
    start_y = float(str[pos_start_y:pos_start_y_end])
    
    end_token = "(end"
    pos_end_x = str.find(end_token) + len(end_token) + 1
    pos_end_x_end = str.find(" ", pos_end_x)
    pos_end_y = pos_end_x_end + 1
    pos_end_y_end = str.find(")", pos_end_y)
    end_x = float(str[pos_end_x:pos_end_x_end])
    end_y = float(str[pos_end_y:pos_end_y_end])
    
    result["start"] = {"x": start_x, "y": start_y, "sub_str_begin": pos_start_x, "sub_str_end": pos_start_y_end}
    result["end"] = {"x": end_x, "y": end_y, "sub_str_begin": pos_end_x, "sub_str_end": pos_end_y_end}
    result["str"] = str
    result["line"] = line_index
    return result

def fix_gr_line_str(data):
    result = data["str"][0:data["start"]["sub_str_begin"]]
    result += str(data["start"]["x"]) + " " + str(data["start"]["y"])
    result += data["str"][data["start"]["sub_str_end"]:data["end"]["sub_str_begin"]]
    result += str(data["end"]["x"]) + " " + str(data["end"]["y"])
    result += data["str"][data["end"]["sub_str_end"]:]
    data["str"] = result
    return data

def dist(entry1, entry2, p1, p2):
    return math.sqrt( (entry1[p1]["x"] - entry2[p2]["x"])**2 + (entry1[p1]["y"] - entry2[p2]["y"])**2)

def is_horizontal(entry):
    return entry["start"]["y"] == entry["end"]["y"]

def is_vertical(entry):
    return entry["start"]["x"] == entry["end"]["x"]

def connect(entry1, entry2, p1, p2):
    if is_horizontal(entry1) and is_vertical(entry2):
        entry1[p1]["x"] = entry2[p2]["x"]
        entry2[p2]["y"] = entry1[p1]["y"]
    elif is_vertical(entry1) and is_horizontal(entry2):
        entry1[p1]["y"] = entry2[p2]["y"]
        entry2[p2]["x"] = entry1[p1]["x"]
    else:
        entry2[p2]["x"] = entry1[p1]["x"]
        entry2[p2]["y"] = entry1[p1]["y"]

def test_connect(entry1, entry2, max_dist):
    ends = ["start", "end"]
    for p1 in ends:
        for p2 in ends:
            d = dist(entry1, entry2, p1, p2)
            if (d < max_dist):
                connect(entry1, entry2, p1, p2)
                return 1
    return 0

cmd_line_parser = OptionParser()
cmd_line_parser.add_option("-i", "--input", dest="input",
                         help="Input file name", metavar="FILENAME.kicad_pcb")

cmd_line_parser.add_option("-o", "--output", dest="output",
                         help="Output file name", metavar="FILENAME.kicad_pcb")

cmd_line_parser.add_option("-d", "--distance", dest="distance", default="0.5",
                         help="Maximal autoconnection distance", metavar="DISTANCE")
                        
(cmd_line_options, cmd_line_args) = cmd_line_parser.parse_args()

max_connect_dist = float(cmd_line_options.distance)

if not cmd_line_options.input:
    print("Input file not set, aborting")
    sys.exit(-1)

input_file_name = cmd_line_options.input
output_file_name = "edges_fixed_" + input_file_name
if cmd_line_options.output:
    output_file_name = cmd_line_options.output
else:
    print("Output file not set, using default: %s" % output_file_name)

content = ""

# Reading file
with open(input_file_name, 'r') as content_file:
    content = content_file.read()

content_lines = content.splitlines()

objects = []

# Searching objects
for i, line in enumerate(content_lines):
    pos = line.find("(gr_line")
    if pos != -1:
        objects.append(parse_gr_line(line, i))

counter = 0
# Connecting
for i in range(0, len(objects)):
    for j in range(i+1, len(objects)):
        counter += test_connect(objects[i], objects[j], max_connect_dist)

# Changing strings
for obj in objects:
    fix_gr_line_str(obj)
    content_lines[obj["line"]] = obj["str"]

# Writing to result file
with open(output_file_name, 'w') as output_file:
    for line in content_lines:
        output_file.write("%s\n" % line)

print("Successfuly fixed %d connections" % counter)
    


