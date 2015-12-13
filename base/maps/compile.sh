#!/bin/sh

case $1 in
	-map2bsp)
		../../xmap.x86 -map2bsp -v -leaktest $2.map
		;;

	-fastvis)
		../../xmap.x86 -vis -fast $2.map
		;;
		
	-vis)
		../../xmap.x86 -vis $2.map
		;;

	-light)
		../../xmap.x86 -light -v -patchshadows $2.map
		;;

	-vlight)
		../../xmap.x86 -vlight -v $2.map
		;;

	-onlyents)
		../../xmap.x86 -map2bsp -onlyents $2.map
		;;

	-aas)
		../../bspc.x86 -bsp2aas $2.bsp
		;;
	
	-all)
		../../xmap.x86 -map2bsp -v -leaktest $2.map
		../../xmap.x86 -vis $2.map
		#../../xmap.x86 -vlight -v $2.map
		../../bspc.x86 -bsp2aas $2.bsp
		;;
	*)
		echo "specify command: -map2bsp, -fastvis, -vis, -light, -xlight, -vlight, -onlyents, -aas or -all <mapname>"
		;;
esac

