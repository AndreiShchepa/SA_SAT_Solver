#!/bin/bash

whitebox() {
    cool_coef=(0.9 0.95 0.975 0.99)
    equilibrium=(100 200 300 400 500)
    temp_factor=(50 100 150 200)

    for val1 in "${cool_coef[@]}"; do
        for val2 in "${equilibrium[@]}"; do
            for val3 in "${temp_factor[@]}"; do
                python3 whitebox.py --program-path ../build/sasat++ --factor-temp "$val3" --equilibrium "$val2" --cool-coef "$val1" &
            done
        done
    done
}


blackbox() {
    main_dirs=("wuf20-71" "wuf20-91" "wuf50-218")
    sub_dirs=("M" "Q" "R" "N")

    for main_dir in "${main_dirs[@]}"; do
        for sub_dir in "${sub_dirs[@]}"; do
            local full_path="../data/blackbox/$main_dir/$sub_dir"
            python3 blackbox.py --program-path ../build/sasat++ --dir-path $full_path &
        done
    done
}


cd python_scripts

if [ "$1" == "whitebox" ]; then
    whitexbox
elif [ "$1" == "blackbox" ]; then
    blackbox
else
    whitebox
    blackbox
fi

cd ..
