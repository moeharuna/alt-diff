#/bin/sh
file="alt-linux-dep-list.txt"

while read -r line; do
    rpm -q $line
done <$file