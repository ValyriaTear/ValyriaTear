# Empties the map names translation files
echo -n "" > map_names_tr.lua

cat map_names_files |
while read a; do

    # Get map name or map subname
    MAP_NAME=""
    MAP_NAME=$(cat  $a | grep 'map_name' | cut -d'=' -f2 | sed s/\ \"/\"/)
    if [ "$MAP_NAME" = "\"\"" -o "$MAP_NAME" = "" ]; then
        MAP_NAME=$(cat  $a | grep 'map_subname' | cut -d'=' -f2 | sed s/\ \"/\"/)
    fi
    # Add map name if it's not empty
    if [[ "$MAP_NAME" != "\"\"" && "$MAP_NAME" != "" ]]; then
        echo "/// tr: Map name" >> map_names_tr.lua
        echo "Translate("$MAP_NAME")" >> map_names_tr.lua
    fi
done
