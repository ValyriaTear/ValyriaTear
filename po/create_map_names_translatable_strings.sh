# Empties the map names translation files
echo "" > map_names_tr.lua

cat map_names_files |
while read a; do

    echo -n "Translate(" >> map_names_tr.lua
    cat  $a | grep 'map_name' | cut -d'=' -f2 | sed s/\ \"/\"/ >> map_names_tr.lua
    echo ")" >> map_names_tr.lua

    echo -n "Translate(" >> map_names_tr.lua
    cat  $a | grep 'map_subname' | cut -d'=' -f2 | sed s/\ \"/\"/ >> map_names_tr.lua
    echo ")" >> map_names_tr.lua
done
