from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from setuptools import find_packages
from setuptools import setup

description="Tools for tuning multi-parton interactions in Pythia8"

setup(
    name="mpituning",
    version="0.1.0",
    description=description,
    long_description=description,
    author="HEPTrkx",
    license="Apache License, Version 2.0",
    keywords=["generator tuning"],
    url="https://gitlab.cern.ch/xju/mpituning.git",
    packages=find_packages(),
    install_requires=[
        'pandas',
        'numpy',
        'pyDOE',
    ],
    setup_requires=[],
    classifiers=[
        "Programming Language :: Python :: 3.7",
    ],
    scripts=[
        'scripts/change_yoda',
        'scripts/compare_yoda_files',
        'scripts/normalize_yoda',
        'scripts/create_diy_configs',
    ],
)