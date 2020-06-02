#!/usr/bin/env python

from plotting import read_plot_keys
import pprint

pp = pprint.PrettyPrinter(indent=4)

def test_plot_keys():
    filename = "ATLAS_2014_I1315949.plot"
    res = read_plot_keys(filename)
    pp.pprint(res['/ATLAS_2014_I1315949/d96'])

if __name__ == "__main__":
    test_plot_keys()