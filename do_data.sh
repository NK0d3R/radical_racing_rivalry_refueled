echo --- GFX ---
PROJECT_FOLDER=radical_racing_rivalry_refueled
RES_FOLDER=$PROJECT_FOLDER/src/res
GFX_FOLDER=gfx
STRINGS_FOLDER=strings
TOOLS_FOLDER=tools
COPYRIGHT_FILE=$TOOLS_FOLDER/copyright.inc

python $TOOLS_FOLDER/bin2c.py -i $GFX_FOLDER/environment.tny -o $RES_FOLDER/env_sprite.h -m 8 -c $COPYRIGHT_FILE
python $TOOLS_FOLDER/bin2c.py -i $GFX_FOLDER/car.tny -o $RES_FOLDER/car_sprite.h -m 8 -c $COPYRIGHT_FILE
python $TOOLS_FOLDER/bin2c.py -i $GFX_FOLDER/hud.tny -o $RES_FOLDER/hud_sprite.h -m 8 -c $COPYRIGHT_FILE
python $TOOLS_FOLDER/bin2c.py -i $GFX_FOLDER/font.tny -o $RES_FOLDER/font.h -m 8 -c $COPYRIGHT_FILE
python $TOOLS_FOLDER/bin2c.py -i $GFX_FOLDER/menu.tny -o $RES_FOLDER/menu_sprite.h -m 8 -c $COPYRIGHT_FILE
python $TOOLS_FOLDER/colors2palette.py -i $GFX_FOLDER/fire.pal -o $RES_FOLDER/firepal.h -c $COPYRIGHT_FILE
python $TOOLS_FOLDER/fontmap.py -i $GFX_FOLDER/fontmap.txt -o $RES_FOLDER/fontmap.h -c $COPYRIGHT_FILE
python $TOOLS_FOLDER/strings2cpp.py -i $STRINGS_FOLDER/strings.txt -o $RES_FOLDER/strings.hpp -m 8 -c $COPYRIGHT_FILE

echo Done
