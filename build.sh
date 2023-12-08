echo "rebuilding..."
rm -rf build/*
mkdir build
cd build
echo "building base.cpp"
g++ -O3 ../base.cpp -o base
echo "building base_cache.cpp"
g++ -O3 ../base_cache.cpp -o base_cache
# echo "building base_omp.cpp"
# g++ -O0 ../base_omp.cpp -o base_omp -fopenmp
echo "building base_omp_TLS.cpp"
g++ -O3 ../base_omp_TLS.cpp -o base_omp_TLS -fopenmp
echo "building base_omp_TLS_cache.cpp"
g++ -O3 ../base_omp_TLS_cache.cpp -o base_omp_TLS_cache -fopenmp
# echo "building base_omp_cache.cpp"
# g++ -O0 ../base_omp_cache.cpp -o base_omp_cache -fopenmp
# echo "copying testrun.sh"
# cp ../testrun.sh ./testrun.sh
# chmod +x ./testrun.sh
echo "done"