while [ 1 ]
do
        for ((i=0; i < 11;i++))
        do
                rmdir /sys/fs/cgroup/cpu/test/test-$i
        done
done
