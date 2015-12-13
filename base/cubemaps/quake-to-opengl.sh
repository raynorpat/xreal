echo "converting $1 ..."

convert $1_rt.tga -flip -rotate 90 $1_px.tga
convert $1_lf.tga -flip -rotate -90 $1_nx.tga

convert $1_bk.tga -flip $1_py.tga
convert $1_ft.tga -flop $1_ny.tga

convert $1_up.tga -flip -rotate 90 $1_pz.tga
convert $1_dn.tga -flop -rotate -90 $1_nz.tga
