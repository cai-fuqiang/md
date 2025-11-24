create_cgroup()
{
        GROUP_NAME=$1
        mkdir -p /sys/fs/cgroup/cpu/$GROUP_NAME
}

move_cgroup()
{
        TASK=$1
        TO=$2
        create_cgroup $TO
        echo $TASK > /sys/fs/cgroup/cpu/$TO/tasks
}

main()
{
        TASK=$1
        create_cgroup test
        while [ 1 ]
        do
                num=$((RANDOM % 11))
                move_cgroup $TASK "test/test-$num"
        done
}


main $@
