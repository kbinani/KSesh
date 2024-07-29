set -ue

echo 'import { Insertion } from "./insertion";'
cat "$1" | sed 's/\(#.*\)//g' | sed 's@insertions@// prettier-ignore\nexport const insertions: {[key: string]: Insertion}@g' | grep -v '^$'
