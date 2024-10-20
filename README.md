# Main program
1. mkdir build && cd build/ && cmake .. && make
2. sasat++ -f data/whitebox/fa/wuf50-218/wuf-R-data/wuf50-0657.mwcnf -T 50 -e 300 -a 0.975

# Python scripts
1. python3 whitebox.py
2. python3 blackbox.py
3. python3 plot.py

### Hint just run chmod +x run.sh && ./run.sh

For correct working of some scripts could be necessary to have some folders in project:
results/{whitebox/table_results,blackbox}
data/{whitebox/fa,blackbox}

each folder in data should contain wuf-data folder with problems and wuf-opt.dat file with answers
