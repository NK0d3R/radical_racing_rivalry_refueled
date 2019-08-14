echo Checking source files
PROJECT_FOLDER=radical_racing_rivalry_refueled
PROJECT_SRC_FOLDER=$PROJECT_FOLDER/src
TOOLS_FOLDER=tools
GOOGLE_LINT=$TOOLS_FOLDER/third-party/cpplint/cpplint.py
SRC_FOLDERS="$PROJECT_SRC_FOLDER
             $PROJECT_SRC_FOLDER/engine
             $PROJECT_SRC_FOLDER/game
             $PROJECT_SRC_FOLDER/game/states
             $PROJECT_SRC_FOLDER/res"
for srcdir in $SRC_FOLDERS; do
    echo "Checking folder $srcdir"
    for filename in $srcdir/*; do
        if [ -d "$filename" ]; then
            continue
        fi
        echo "Checking $filename"
        while :
        do
            python $GOOGLE_LINT --root="$srcdir"                    \
                    --repository="$srcdir" --filter=-build/include  \
                    $filename
            if [ $? = 0 ]; then
                echo File is OK !
                break
            else
                echo "Errors found ! (r)etry (n)ext (q)uit"
                read res
                if [ $res = "n" ]; then
                    break
                fi
                if [ $res = "q" ]; then
                    exit 255
                fi
            fi
        done
    done
done
echo Done
