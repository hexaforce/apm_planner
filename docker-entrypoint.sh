#!/bin/bash
set -e

 qmake apm_planner.pro

make -j`nproc`
