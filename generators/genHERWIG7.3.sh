#! /bin/bash

bash_par="$1"
save_par="$2" # "n" or "neutron" inputs can save the RHICf neutron event with RHICf pi0 event

jobNum=110
#eventnum=1000000 # for only pi0 event (estimated event number: 150, 1 RHICf pi0 event per 3000 generate event)
eventnum=10000 # for pi0 and neutron event (estimated event number: 100, 1 RHICf pi0+neutron event per 24 generate event)

screenName=RHICfSim_HERWIG7
inputName=run_RHIC-MB
herwigPath=/home/public/herwig/bin/activate
handlerPath=/home/shlee/workspace/rhicf/generators/herwig7.3
dataPath=/home/shlee/workspace/rhicf/data

if [[ "${bash_par}" == "0" || "${bash_par}" == "TL" ]] || [[ "${bash_par}" == "1" || "${bash_par}" == "TS" ]] || [[ "${bash_par}" == "2" || "${bash_par}" == "TOP" ]] ; 
then
    typeName="-1"
    if [ "${bash_par}" == "TL" ] ;
    then
        typeName="0"

    elif [ "${bash_par}" == "TS" ] ;
    then
        typeName="1"

    elif [ "${bash_par}" == "TOP" ] ;
    then
        typeName="2"
    else 
        typeName=${bash_par}
    fi

    cd ${handlerPath}
    make

    for (( i=1; i<jobNum+1; i++ ))
    do  
        cd ${handlerPath}
        # make the HERWIG input files
        root -l -b -q 'makeHerwigInput.C('${i},\"${typeName}\",\"${dataPath}\",\"${save_par}\"')'

        # make the random seed
        randomSeed="$(($RANDOM% 99999))"

        # screen excute
        screen -dmS ${screenName}_${i} bash 
        screen -S ${screenName}_${i} -X stuff "source ${herwigPath}
"
        screen -S ${screenName}_${i} -X stuff "cd ${handlerPath}
"
        screen -S ${screenName}_${i} -X stuff "Herwig read ${inputName}_${i}.in
"
        screen -S ${screenName}_${i} -X stuff "Herwig run ${inputName}_${i}.run -N ${eventnum} -s ${randomSeed}
"
        echo "RHIfSim::Generators HERWIG7.3 job ${i} has completed !!! (Herwig run ${inputName}_${i}.run -N ${eventnum} -s ${randomSeed})"
    done


else
    # screen section kill
    if [ "${bash_par}" == "screenclear" ] ; 
    then

        for (( i=1; i<jobNum+1; i++ ))
        do
            screen -X -S ${screenName}_${i} quit
            echo "screen kill ${screenName}_${i}"
        done

    # copying file remove
    elif [ "${bash_par}" == "fileclear" ] ; 
    then 

        for files in ${handlerPath}/${inputName}_*
        do
            echo "rm ${files}"
            rm ${files}
        done

    # root file remove
    elif [ "${bash_par}" == "rootclear" ] ; 
    then 
        for files in ${dataPath}/Herwig7*
        do
            echo "rm ${files}"
            rm ${files}
        done

    # all clear 
    elif [ "${bash_par}" == "allclear" ] ; 
    then
        for (( i=1; i<jobNum+1; i++ ))
        do
            screen -X -S ${screenName}_${i} quit
            echo "screen kill ${screenName}_${i}"
        done

        for files in ${handlerPath}/${inputName}_*
        do
            echo "rm ${files}"
            rm ${files}
        done

        for files in ${dataPath}/Herwig7*
        do
            echo "rm ${files}"
            rm ${files}
        done
    fi


fi





