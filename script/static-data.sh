set -ue

echo '// prettier-ignore'
echo 'export const staticData = {'
i=0
name=
file=
for var in "$@"; do
    even=$((i % 2))
    if [ $even = 1 ]; then
      file=$var
      if [ $i -gt 2 ]; then
        echo ','
      fi
      echo -n "  $name: '"
      cat "$file" | base64 | tr -d '\n'
      echo -n "'"
    else
      name=$var
    fi
    i=$((i + 1))
done
echo
echo '};'
