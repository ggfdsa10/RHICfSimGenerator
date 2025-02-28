#! /bin/bash

bash_par="$1"
save_par="$2" # "n" or "neutron" inputs can save the RHICf neutron event with RHICf pi0 event

jobNum=110
# eventnum=1500000 # for only pi0 event (1 per 6000 for EPOSLHC, 1 per 7500 for QGSJETII04)
eventnum=18000 # for pi0 and neutron event (1 per 60 for EPOSLHC, 1 per 40 QGSJETII04)

modelIdx=1
# model: 0=epos-lhc, 1=qgsjetII-04, 2=qgsjetIII

screenName=RHICfSim_CRMC
crmcPath=/home/shlee/workspace/rhicf/generators/crmc2.1/build
dataPath=/home/shlee/workspace/rhicf/data
modelName=("EPOSLHC" "QGSJETII04" "QGSJETIII")


if [[ "${bash_par}" == "0" || "${bash_par}" == "TL" ]] || [[ "${bash_par}" == "1" || "${bash_par}" == "TS" ]] || [[ "${bash_par}" == "2" || "${bash_par}" == "TOP" ]] ; 
then
    typeName="-1"
    if [ "${bash_par}" == "0" ] ;
    then
        typeName="TL"

    elif [ "${bash_par}" == "1" ] ;
    then
        typeName="TS"

    elif [ "${bash_par}" == "2" ] ;
    then
        typeName="TOP"
    else 
        typeName=${bash_par}
    fi

    if [ "${save_par}" == "" ] ; 
    then 
        save_par="0"
    fi

    cd ${crmcPath}
    make -j14

    for (( i=1; i<jobNum+1; i++ ))
    do  
        # screen excute
        screen -dmS ${screenName}_${modelName[modelIdx]}_${i} bash 

        screen -S ${screenName}_${modelName[modelIdx]}_${i} -X stuff "cd ${crmcPath}
"
        tmpModelIdx=0
        if [ ${modelIdx} -eq 1 ] ;
        then
            tmpModelIdx=7

        elif [ ${modelIdx} -eq 2 ] ;
        then
            tmpModelIdx=13
        fi

        screen -S ${screenName}_${modelName[modelIdx]}_${i} -X stuff "./crmc -o hepmc -S 510 -R ${typeName} -v ${save_par} -O ${dataPath} -n ${eventnum} -m ${tmpModelIdx}
"
        echo "screen ${i} th ========= job has completed !!! ./crmc -o hepmc -S 510 -R ${typeName} -v ${save_par} -O ${dataPath} -n ${eventnum} -m ${tmpModelIdx}"

        sleep 0.3

    done


else
    # screen section kill
    if [ "${bash_par}" == "screenclear" ] ; 
    then

        for (( i=1; i<jobNum+1; i++ ))
        do
            screen -X -S ${screenName}_${modelName[modelIdx]}_${i} quit
            echo "screen kill ${screenName}_${modelName[modelIdx]}_${i}"
        done

    # generating hepmc file remove
    elif [ "${bash_par}" == "fileclear" ] ; 
    then 

        for files in ${dataPath}/*.hepmc
        do
            echo "rm ${files}"
            rm ${files}
        done

    # root file remove
    elif [ "${bash_par}" == "rootclear" ] ; 
    then 

        for files in ${dataPath}/${modelName[modelIdx]}*
        do
            echo "rm ${files}"
            rm ${files}
        done

    # all clear 
    elif [ "${bash_par}" == "allclear" ] ; 
    then

        for (( i=1; i<jobNum+1; i++ ))
        do
            screen -X -S ${screenName}_${modelName[modelIdx]}_${i} quit
            echo "screen kill ${screenName}_${modelName[modelIdx]}_${i}"
        done

        for files in ${dataPath}/*.hepmc
        do
            echo "rm ${files}"
            rm ${files}
        done

        for files in ${dataPath}/${modelName[modelIdx]}*
        do
            echo "rm ${files}"
            rm ${files}
        done
    fi


fi





