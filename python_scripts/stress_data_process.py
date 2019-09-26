# process stress data generated by different CAD software to uniformly style
import sys
import os
import numpy as np


def readHypermeshStyle(lines):
    stress = np.zeros((1, 6))
    # line_number = int(sline[0])
    n = len(lines)
    k = 0
    res = np.zeros((n, 6))
    stress = np.zeros(6)
    for line in lines:
        sline = line.split(',')
        # if this line contains data
        if sline[0].isdigit():
            digs = sline[1].split(' ')
            for i in range(6):
                stress[i] = float(digs[i])
            res[k] = stress
            k += 1
    return k, res[:k]


def readGuassStyle(lines):
    order = [0, 1, 2, 4, 5, 3]
    n = len(lines)
    k = 0
    res = np.zeros((n, 6))
    stress = np.zeros(6)
    for line in lines:
        sline = line.split()
        # if this line contains data
        try:
            float(sline[0])
        except ValueError:
            continue
        else:
            for i in range(6):
                stress[order[i]] = float(sline[i])
            res[k] = stress
            k += 1
    return k, res[:k]


if __name__ == "__main__":
    file_name = sys.argv[1]
    base_name = os.path.basename(file_name)
    # ext = os.path.splitext(file_name)[1]
    save_file_name = "processed_" + base_name
    with open(file_name, 'r') as fin:
        header = fin.readline()
        lines = fin.readlines()
        if header.find("GAUSS") >= 0:
            k, res = readGuassStyle(lines)
        else:
            k, res = readHypermeshStyle(lines)
        # header = str(k)
        if len(sys.argv) > 2:
            np.savetxt(save_file_name, res, fmt="%.4e",
                       delimiter=' ', header=sys.argv[2]+" " + str(k))
        else:
            np.savetxt(save_file_name, res, fmt="%.4e",
                       delimiter=' ', header="element " + str(k))