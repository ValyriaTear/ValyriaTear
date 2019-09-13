# Empties the map names translation files
echo -n "" > map_names_tr.lua

# Get map name and map subname
cat map_names_files |
while read a; do
    MAP_NAME=""
    # Add map name if it's not empty
    MAP_NAME=$(cat  $a | grep 'map_name' | cut -d'=' -f2 | sed s/\ \"/\"/)

    if [[ "$MAP_NAME" != "\"\"" && "$MAP_NAME" != "" ]]; then
        echo "/// tr: Map name" >> map_names_tr.lua
        echo "Translate("$MAP_NAME")" >> map_names_tr.lua
    fi
    MAP_NAME=$(cat  $a | grep 'map_subname' | cut -d'=' -f2 | sed s/\ \"/\"/)
    if [[ "$MAP_NAME" != "\"\"" && "$MAP_NAME" != "" ]]; then
        echo "/// tr: Map sub-name" >> map_names_tr.lua
        echo "Translate("$MAP_NAME")" >> map_names_tr.lua
    fi
done
