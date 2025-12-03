FILE_NAME=$1

awk '
{
    ts = $4
    sub(/:/, "", ts)
    if (NR % 2 == 1) {
        odd = ts
    } else {
        diff = ts - odd
        printf("%d : %.6f - %.6f = %.6f\n", NR/2, ts, odd, diff)
        sum += diff
    }
}
END {
    printf("\\sum {2i}-{2i-1}: %.6f\n", sum)
}
'  $FILE_NAME
