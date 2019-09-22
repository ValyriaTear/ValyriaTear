# Empties the map names translation files

input_filename="$1"
output_filename="$input_filename-tr.lua"

echo -n "" > $output_filename

# Get map name and map subname
cat $input_filename |
while read a; do
    MAP_NAME=""
    # Add map name if it's not empty
    MAP_NAME=$(cat  $a | grep 'map_name' | cut -d'=' -f2 | sed s/\ \"/\"/)

    if [[ "$MAP_NAME" != "\"\"" && "$MAP_NAME" != "" ]]; then
        echo "/// tr: Map name" >> $output_filename
        echo "Translate("$MAP_NAME")" >> $output_filename
    fi
    MAP_NAME=$(cat  $a | grep 'map_subname' | cut -d'=' -f2 | sed s/\ \"/\"/)
    if [[ "$MAP_NAME" != "\"\"" && "$MAP_NAME" != "" ]]; then
        echo "/// tr: Map sub-name" >> $output_filename
        echo "Translate("$MAP_NAME")" >> $output_filename
    fi
done
