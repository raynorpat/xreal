#!/bin/sh
echo "converting $1 ..."

convert $1_forward.tga -flip -rotate 90 $1_px.png
convert $1_back.tga -flip -rotate -90 $1_nx.png

convert $1_left.tga -flip $1_py.png
convert $1_right.tga -flop $1_ny.png

convert $1_up.tga -flip -rotate 90 $1_pz.png
convert $1_down.tga -flop -rotate -90 $1_nz.png
