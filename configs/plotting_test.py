#!/usr/bin/env python

from plotting import read_plot_keys
import pprint
import yoda

pp = pprint.PrettyPrinter(indent=4)

def test_plot_keys():
    filename = "ATLAS_2014_I1315949.plot"
    res = read_plot_keys(filename)
    pp.pprint(res['/ATLAS_2014_I1315949/d96'])


def read_yoda():
    filename = "Rivet.yoda"
    keyname = "/ATLAS_2014_I1315949/d71-x01-y01"
    data = yoda.read(filename)
    p1 = data[keyname]
    print(p1)
    s2d = p1.mkScatter()
    print(s2d, type(s2d))
    for p in s2d.points():
        print(p.x(), p.y())

if __name__ == "__main__":
    # test_plot_keys()
    read_yoda()