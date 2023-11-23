echo "running base"
./base ../WarAndPeace.txt
echo "running base_cache"
./base_cache ../WarAndPeace.txt
echo "setting OMP_NUM_THREADS=4"
export OMP_NUM_THREADS=4
echo "running base_omp"
./base_omp ../WarAndPeace.txt
echo "running base_omp_cache"
./base_omp_cache ../WarAndPeace.txt
echo "setting OMP_NUM_THREADS=1"
export OMP_NUM_THREADS=1
echo "running base_omp"
./base_omp ../WarAndPeace.txt
echo "running base_omp_cache"
./base_omp_cache ../WarAndPeace.txt
echo "done"
