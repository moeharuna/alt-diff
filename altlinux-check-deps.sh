#/bin/sh
file="alt-linux-dep-list.txt"

while read -r line; do
    rpm -q $line
    if [ $? != 0 ]; then
       echo "package $line not installed"
    fi
done <$file