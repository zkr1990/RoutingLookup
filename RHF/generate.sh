#!/bin/sh
#获取文件夹下所有文件
#
folder="./Input_data/Data_10000/"
funcFolder="./routerFuncData/Function_10000/"

if [ ! -d "$funcFolder" ];then
mkdir ${funcFolder}
echo "build ${funcFolder} successfull."
else
echo "${funcFolder} already exists, overwrite all files in it..."
fi
echo ""
softFolder=$(ls $folder)
for subFolder in ${softFolder}
do 
    # build the same dirs for funcFolder
    # echo "subFolder: ${folder}${subFolder}"
    if [ ! -d "${funcFolder}${subFolder}" ];then
        mkdir ${funcFolder}${subFolder}
    fi
    sFile=$(ls ${folder}${subFolder})
    for inFIle in ${sFile}
    do
        # echo "inFIle: ${folder}${subFolder}/${inFIle} ${funcFolder}${subFolder}/out_${inFIle}"
        ./router_mega/Generate 1 ${folder}${subFolder}/${inFIle} ${funcFolder}${subFolder}/out_${inFIle}
    done
done

