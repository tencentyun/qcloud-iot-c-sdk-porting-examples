if [ "$#" -eq "0" ]; then
    rm -rf output
    rm -rf build
    mkdir -p build
    cd build
    cmake .. 
    #-DSAMPLE_ONLY=OFF ..
    make
    exit
elif [ "$#" -eq "1"  -a $1 == "sample" ]; then
    if [ ! -d "output/release/bin" ]; then
        rm -rf output/release/bin
    elif [ ! -d "output/debug/bin" ]; then
        rm -rf output/debug/bin
    else
        echo "bin dir not found!"
        exit
    fi
    rm -rf build
    mkdir -p build
    cd build
    cmake -DSAMPLE_ONLY=ON ..
    make 
    exit
else
	echo "Usage: "$0"  <sample>"
	exit
fi
