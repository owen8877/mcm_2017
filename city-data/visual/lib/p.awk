{
    x=$1;
    y=$2;
    p=$3;
    t=$4;
    bus=$5;
    switch (t) {
        case 0:
            type="r";
            break;
        case 1:
            type="w";
            break;
        case 2:
            type="c";
            break;
        case 3:
            type="o";
            break;
        case 4:
            type="u";
            break;
    }
    printf "{\"x\":%d,\"y\":%d,\"type\":\"%s\",\"progress\":%d,\"bus\":%s},", x, y, type, p, bus ? "true" : "false";
}
